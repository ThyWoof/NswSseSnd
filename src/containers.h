#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include ".\lib\dsptool\dsptool.h"

typedef struct 
{
    uint32_t num_samples;
    uint32_t num_nibbles;
    uint32_t sample_rate;
    uint16_t loop_flag;
    uint16_t format; /* 0 for ADPCM */
    uint32_t loop_start;
    uint32_t loop_end;
    uint32_t ca;
    int16_t coef[16];
    int16_t gain;
    int16_t ps;
    int16_t hist1;
    int16_t hist2;
    int16_t loop_ps;
    int16_t loop_hist1;
    int16_t loop_hist2;
    uint16_t pad[11];
} ADPCM_HEADER;

typedef struct 
{
    uint32_t num_channels; // 1 (0x01 0x00 0x00 0x00)
    uint32_t header_size;  // 12 (0x0C 0x00 0x00 0x00)
    uint32_t data_size;
} MCADPCM_HEADER_MONO;

typedef struct 
{
    uint32_t num_channels; // 2 (0x02 0x00 0x00 0x00)
    uint32_t header_size;  // 16 (0x0C 0x00 0x00 0x00)
    uint32_t ch0_data_size;
    uint32_t ch1_data_size;
} MCADPCM_HEADER_STEREO;

typedef struct
{
    uint32_t signature; // FUSE
    uint32_t version; // 1 (0x01 0x00 0x00 0x00)
    uint32_t lipSize;
} FUZ_HEADER_PC;

typedef struct
{
    uint32_t signature; // FUSE
    uint32_t version; // 1 (0x01 0x00 0x00 0x00)
    uint32_t lipSize;
    uint32_t soundOffset;
} FUZ_HEADER_NSW;

void fillAdpcmHeader(ADPCM_HEADER* header, ADPCM_INFO* cxt, uint32_t samples, uint32_t sampleRate);
void fillAdpcmHeaderForMcAdpcm(ADPCM_HEADER* header, ADPCM_INFO* cxt, uint32_t samples, uint32_t sampleRate);
void fillMcAdpcmHeaderMono(MCADPCM_HEADER_MONO* header, ADPCM_INFO* cxt);

#ifdef __cplusplus
}
#endif