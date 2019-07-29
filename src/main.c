#define DR_WAV_IMPLEMENTATION

#include <io.h>
#include <stdio.h>
#include <stdint.h>
#include "dr_wav.h"
#include "containers.h"
#include ".\dsptool\dsptool.h"

#define _ACCESS_READ 0x02
#define _ACCES_WRITE 0x04

const char* getFileExt(const char *filename) {
    char *ext = strrchr (filename, '.');
    return ext;
}

int main(int argc, char** argv)
{
    // validate input
    if (argc < 3)
    {
        printf("Usage: %s inputFile outputFile\n", *argv);
        exit(EXIT_FAILURE);
    }

    // validate input file
    char* inputFile = argv[1];
    const char* inputFileExt = getFileExt(inputFile);
    if(stricmp(inputFileExt, ".fuz") && stricmp(inputFileExt, ".wav") && stricmp(inputFileExt, ".wma")) {
        fprintf(stderr, "ABORT: input file must be FUZ, WAV or WMA.\n");
        exit(EXIT_FAILURE);
    }

    // validate output file
    char* outputFile = argv[2];
    const char* outputFileExt = getFileExt(outputFile);
    if(stricmp(outputFileExt, ".fuz") && stricmp(outputFileExt, ".dsp") && stricmp(outputFileExt, ".mcadpcm") && stricmp(outputFileExt, ".dspadpcm")) {
        fprintf(stderr, "ABORT: output file must be FUZ, WAV or WMA.\n");
        exit(EXIT_FAILURE);
    }

    uint32_t channels;
    uint32_t sampleRate;
    uint64_t samples;
    int16_t* inputBufferPCM;

    if(!stricmp(inputFileExt, ".fuz")) {
    // process a FUZ input file

        fprintf(stderr, "ABORT: FUZ conversion not implemented yet.\n");
        exit(EXIT_FAILURE);
        // read FUZ
        // split FUZ in WMA and LIP
        // convert WMA to WAV in memory (or use XWmaConverter as a last resort if no good C lib for WMA)
        // use drwav to open WAV in memory


    } else if(!stricmp(inputFileExt, ".wma")) {
    // process a WMA input file

        fprintf(stderr, "ABORT: WMA conversion not implemented yet.\n");
        exit(EXIT_FAILURE);
        // convert WMA to WAV in memory
        // use drwav to open WAV in memory (or use XWmaConverter as a last resort if no good C lib for WMA)

    } else if(!stricmp(inputFileExt, ".wav")) {
    // process a WAV input file

        inputBufferPCM = drwav_open_file_and_read_pcm_frames_s16(inputFile, &channels, &sampleRate, &samples);
        if (inputBufferPCM == NULL) {
            printf("ABORT: cannot open the WAV file %s\n", inputFile);
            exit(EXIT_FAILURE);
        }
    }

    //
    // convert from PCM16 to Nintendo DSP
    //

    uint32_t bufferSizeADPCM = getBytesForAdpcmBuffer((uint32_t)samples);
    uint8_t* outputBufferADPCM = malloc(bufferSizeADPCM);
    ADPCM_INFO adpcmInfo = {0};
    encode(inputBufferPCM, outputBufferADPCM, &adpcmInfo, samples);
 
    //
    // writes the DSP stream to the proper container output
    //

    FILE* outputFileHandle = fopen(outputFile, "wb");
    if (!outputFileHandle)
    {
        fprintf(stderr, "ABORT: cannot save the output file '%s\n", outputFile);
        return 1;
    }

    // save a DSP or DSPADPCM 100% compatible with VGAAudioCli output
    if (!stricmp(outputFileExt, ".dsp") || !stricmp(outputFileExt, ".dspadpcm")) {
        ADPCM_HEADER adpcmHeader = {0};
        fillAdpcmHeader(&adpcmHeader, &adpcmInfo, samples, sampleRate);
        fwrite(&adpcmHeader, sizeof(ADPCM_HEADER), 1, outputFileHandle);
        fwrite(outputBufferADPCM, 1, bufferSizeADPCM, outputFileHandle);
    }
    drwav_free(inputBufferPCM);
    free(outputBufferADPCM);
    fclose(outputFileHandle);
} 