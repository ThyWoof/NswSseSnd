#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "dsptool\dsptool.h"

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
    uint32_t num_samples;
    uint32_t num_nibbles;
    uint32_t sample_rate;
    uint16_t loop_flag;
    uint16_t format;
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
} MCADPCM_HEADER;

void fillAdpcmHeader(ADPCM_HEADER* header, ADPCM_INFO* cxt, uint32_t samples, uint32_t sampleRate);
void fillAdpcmHeaderForMcAdpcm(ADPCM_HEADER* header, ADPCM_INFO* cxt, uint32_t samples, uint32_t sampleRate);

#ifdef __cplusplus
}
#endif