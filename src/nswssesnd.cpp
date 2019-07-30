/*
nswssesnd: command line to convert PC Skyrim SE sounds to Nintendo Switch
Copyright (c) 2019 Thy Woof

OptimizeForSSE function from https://github.com/ousnius/BodySlide-and-Outfit-Studio

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define DR_WAV_IMPLEMENTATION

#include <fstream>
#include ".\lib\cxxopts\cxxopts.hpp"
#include ".\lib\dr_lib\dr_wav.h"
#include ".\lib\dsptool\dsptool.h"
#include ".\containers.h"

#define _ACCESS_READ 0x02
#define _ACCES_WRITE 0x04

using namespace std;

void validateFile(string filename) {

    struct stat buffer;
    if (stat(filename.c_str(), &buffer)) {
        cout << "ABORT: cannot open file." << endl;
        ::exit(EXIT_FAILURE);
    }
}

inline string getOutputFilename(string filename, string extension){

    size_t found = filename.find_last_of(".");
    return filename.substr(0,found + 1).append(extension);
}

 void validateFileExtension(string filename, const char* extension){

    size_t found = filename.find_last_of(".");
    if (stricmp(filename.substr(found + 1).c_str(), extension)) {
        cout << "ABORT: invalid file extension." << endl;
        ::exit(EXIT_FAILURE);
    }
}

uint32_t loadFileinMemory(string filename, char** buffer) {

    streampos size;

    ifstream file (filename, ios::in|ios::binary|ios::ate);
    if (file.is_open())
    {
        size = file.tellg();
        *buffer = new char[size];
        file.seekg (0, ios::beg);
        file.read(*buffer, size);
        file.close();
        return (uint32_t) size;
    }
    return 0;
}

void commandUnfuz(string filename)
{
    const uint8_t FUZ_SIGNATURE[4] = {'F', 'U', 'Z', 'E'};
    const uint8_t FUZ_VERSION[4] = {0x01, 0x00, 0x00, 0x00};
    const uint8_t LIP_SIGNATURE[4] = {0x01, 0x00, 0x00, 0x00};
    const uint8_t SOUND_SIGNATURE[4] = {'R', 'I', 'F', 'F'};
    const uint32_t FUZ_HEADER_SIZE_PC = 0x0C;
    const uint32_t FUZ_VERSION_POS = 0X04; // from file start
    const uint32_t FUZ_LIP_SIZE_POS = 0x08; // from file start
    const uint32_t FUZ_LIP_DATA_POS = 0x0C; // from file start
    const uint32_t SOUND_FORMAT_POS = 0x08; // from sound start

    char* buffer;
    uint32_t size = loadFileinMemory(filename, &buffer);

    // check for a valid FUZ signature
    if (memcmp(buffer, FUZ_SIGNATURE, sizeof(FUZ_SIGNATURE)) != 0) {
        cout << "ABORT: cannot find a correct FUZ signature." << endl;
      	::exit(EXIT_FAILURE);
    }

    // check for a valid FUZ version
    if (memcmp(buffer + FUZ_VERSION_POS, FUZ_VERSION, sizeof(FUZ_VERSION)) != 0) {
        cout << "ABORT: cannot find a supported FUZ version." << endl;
      	::exit(EXIT_FAILURE);
    }

    // get the LIP data
    uint32_t lipSize = *(uint32_t*)(buffer + FUZ_LIP_SIZE_POS);

    if (lipSize != 0) {
        char* lipDataPos = buffer + FUZ_LIP_DATA_POS;

        // check for a valid LIP signature
        if (memcmp(lipDataPos, LIP_SIGNATURE, sizeof(LIP_SIGNATURE)) != 0) {
            cout << "ABORT: cannot find a correct LIP signature." << endl;
            ::exit(EXIT_FAILURE);
        }

        // save the LIP file
        string lipFilename = getOutputFilename(filename, "LIP");
        ofstream lipFile(lipFilename, ios::binary);
        lipFile.write(lipDataPos, (streamsize) lipSize);
        lipFile.close();
    }

    // get the SOUND data
    char* soundDataPos = buffer + FUZ_LIP_DATA_POS + lipSize;

    // check for a valid SOUND signature
    if (memcmp(soundDataPos, SOUND_SIGNATURE, sizeof(SOUND_SIGNATURE)) != 0) {
        cout << "ABORT: cannot find a correct SOUND signature." << endl;
      	::exit(EXIT_FAILURE);
    }

    // check for a valid SOUND format
    char* soundFormat = soundDataPos + SOUND_FORMAT_POS;
    if (memcmp(soundFormat, "WAVE", 4) != 0 && memcmp(soundFormat, "XWMA", 4) != 0) {
        cout << "ABORT: cannot find a WAVE or XWMA sound format." << endl;
      	::exit(EXIT_FAILURE);
    }

    // save the SOUND file
    char soundExtension[3];
    memcpy(soundExtension, soundFormat, 3);
    string soundFilename = getOutputFilename(filename, soundExtension);
    ofstream soundFile(soundFilename, ios::binary);
    soundFile.write(buffer + FUZ_LIP_DATA_POS + lipSize, (streamsize) (size - FUZ_HEADER_SIZE_PC - lipSize));
    soundFile.close();

    delete[] buffer;
    return;
}

void commandConvertADPCM(string filename)
{
    uint32_t channels;
    uint32_t sampleRate;
    uint64_t samples;
    int16_t* inputBufferPCM;

    inputBufferPCM = drwav_open_file_and_read_pcm_frames_s16(filename.c_str(), &channels, &sampleRate, &samples);
    if (inputBufferPCM == NULL) {
        cout << "ABORT: cannot parse the WAV file." << endl;
      	::exit(EXIT_FAILURE);
    }

    // encode from PCM16 to DSP ADPCM
    uint32_t bufferSizeADPCM = getBytesForAdpcmBuffer((uint32_t)samples);
    uint8_t* outputBufferADPCM = (uint8_t*) malloc(bufferSizeADPCM);
    ADPCM_INFO adpcmInfo = {0};
    encode(inputBufferPCM, outputBufferADPCM, &adpcmInfo, samples);

    // setup the header
    ADPCM_HEADER adpcmHeader = {0};
    fillAdpcmHeader(&adpcmHeader, &adpcmInfo, samples, sampleRate);

    // save the MCADPCM file
    string mcadpcmFilename = getOutputFilename(filename, "ADPCM");
    ofstream mcadpcmFile(mcadpcmFilename, ios::binary);
    mcadpcmFile.write((const char*) &adpcmHeader, (streamsize) sizeof(ADPCM_HEADER));
    mcadpcmFile.write((const char*) outputBufferADPCM, (streamsize) bufferSizeADPCM);
    mcadpcmFile.close();

    free(outputBufferADPCM);
    drwav_free(inputBufferPCM);
}

void commandConvertMCADPCM(string filename)
{
    const uint8_t MCADPCM_HEADER_MONO[8] = {0x01, 0x00, 0x00, 0x00, 0x0C,0x00, 0x00, 0x00}; // uint32_t channels uint32_t header_size
    const uint8_t MCADPCM_HEADER_STEREO[8] = {0x02, 0x00, 0x00, 0x00, 0x14,0x00, 0x00, 0x00}; // uint32_t channels uint32_t header_size
    uint32_t channels;
    uint32_t sampleRate;
    uint64_t samples;
    int16_t* inputBufferPCM;

    inputBufferPCM = drwav_open_file_and_read_pcm_frames_s16(filename.c_str(), &channels, &sampleRate, &samples);
    if (inputBufferPCM == NULL) {
        cout << "ABORT: cannot parse the WAV file." << endl;
      	::exit(EXIT_FAILURE);
    }

    if (channels > 1) {
        cout << "ABORT: cannot support more than one channel... Yet." << endl;
      	::exit(EXIT_FAILURE);
    }

    uint32_t bufferSizeADPCM = getBytesForAdpcmBuffer((uint32_t)samples);
    uint8_t* outputBufferADPCM = (uint8_t*) malloc(bufferSizeADPCM);
    ADPCM_INFO adpcmInfo = {0};
    encode(inputBufferPCM, outputBufferADPCM, &adpcmInfo, samples);

    ADPCM_HEADER adpcmHeader = {0};
    fillAdpcmHeaderForMcAdpcm(&adpcmHeader, &adpcmInfo, samples, sampleRate);

    string mcadpcmFilename = getOutputFilename(filename, "MCADPCM");
    ofstream mcadpcmFile(mcadpcmFilename, ios::binary);
    mcadpcmFile.write((const char*) &MCADPCM_HEADER_MONO, (streamsize) sizeof(MCADPCM_HEADER_MONO));
    mcadpcmFile.write((const char*) &bufferSizeADPCM, (streamsize) sizeof(uint32_t));
    mcadpcmFile.write((const char*) &adpcmHeader, (streamsize) sizeof(ADPCM_HEADER));
    mcadpcmFile.write((const char*) outputBufferADPCM, (streamsize) bufferSizeADPCM);
    mcadpcmFile.close();

    free(outputBufferADPCM);
    drwav_free(inputBufferPCM);
}

void commandFuz(string filename)
{
    //fprintf(stderr, filename);
    return;
}

int main(int argc, char** argv)
{

    bool flagRemove = false;
	cxxopts::Options options(argv[0], "Zappastuff's Nintendo Switch Sound Converter");

    const char* CMD_FUZ = "fuz";
    const char* CMD_UNFUZ = "unfuz";
    const char* CMD_ADPCM = "adpcm";
    const char* CMD_MCADPCM = "mcadpcm";

	try
	{
		options
			.positional_help("[optional args]")
      		.show_positional_help()
			.add_options()
				(CMD_FUZ, "Pack NS SSE FUZ from MCADPCM and LIP", cxxopts::value<string>())
				(CMD_UNFUZ, "Unpack PC SSE FUZ into WMA and LIP", cxxopts::value<string>())
				(CMD_ADPCM, "Convert WAV into ADPCM", cxxopts::value<string>())
				(CMD_MCADPCM, "Convert WAV into MCADPCM", cxxopts::value<string>())
				("r,remove", "Remove input files", cxxopts::value<bool>(flagRemove));

		auto result = options.parse(argc, argv);

        // can only use one and only one of them
	    if (result.count(CMD_UNFUZ) + result.count(CMD_FUZ) + result.count(CMD_ADPCM) + result.count(CMD_MCADPCM) != 1) {
      		cout << options.help() << endl;
      		::exit(EXIT_FAILURE);
    	}

        if (result.count(CMD_FUZ)) {
            string filename = result[CMD_FUZ].as<string>();
            validateFile(filename);
            validateFileExtension(filename, "WAV");
            commandFuz(filename);

        } else if (result.count(CMD_UNFUZ)) {
            string filename = result[CMD_UNFUZ].as<string>();
            validateFile(filename);
            validateFileExtension(filename, "FUZ");
            commandUnfuz(filename);

        } else if (result.count(CMD_ADPCM)) {
            string filename = result[CMD_ADPCM].as<string>();
            validateFile(filename);
            validateFileExtension(filename, "WAV");
            commandConvertADPCM(filename);

        } else if (result.count(CMD_MCADPCM)) {
            string filename = result[CMD_MCADPCM].as<string>();
            validateFile(filename);
            validateFileExtension(filename, "WAV");
            commandConvertMCADPCM(filename);
        }
        ::exit(EXIT_SUCCESS);
    }
	catch (const cxxopts::OptionException& e)
	{
		cout << "ABORT: " << e.what() << endl << endl << options.help() << endl;
		::exit(EXIT_FAILURE);
  	}
}