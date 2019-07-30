#include "containers.h"
#include ".\lib\dsptool\dsptool.h"

void fillAdpcmHeader(ADPCM_HEADER* header, ADPCM_INFO* cxt, uint32_t samples, uint32_t sampleRate) {

    header -> num_samples = _byteswap_ulong(samples);
    header -> num_nibbles = _byteswap_ulong(getNibblesForNSamples(samples));
    header -> sample_rate = _byteswap_ulong(sampleRate);
    header -> format = 0x0000;
    header -> loop_start = _byteswap_ulong(getNibbleAddress(0));
    header -> loop_end = _byteswap_ulong(getNibbleAddress(samples - 1));
    header -> ca = _byteswap_ulong(getNibbleAddress(0));
    for (int i=0 ; i<16 ; ++i)
        header -> coef[i] = _byteswap_ushort(cxt -> coef[i]);
    header -> gain = _byteswap_ushort(cxt -> gain);
    header -> ps = _byteswap_ushort(cxt -> pred_scale);
    header -> hist1 = _byteswap_ushort(cxt -> yn1);
    header -> hist2 = _byteswap_ushort(cxt -> yn2);
    header -> loop_ps = _byteswap_ushort(cxt -> loop_pred_scale);
    header -> loop_hist1 = _byteswap_ushort(cxt -> loop_yn1);
    header -> loop_hist2 = _byteswap_ushort(cxt -> loop_yn2);
}

void fillAdpcmHeaderForMcAdpcm(ADPCM_HEADER* header, ADPCM_INFO* cxt, uint32_t samples, uint32_t sampleRate) {

    header -> num_samples = (samples);
    header -> num_nibbles = (getNibblesForNSamples(samples));
    header -> sample_rate = (sampleRate);
    header -> format = 0x0000;
    header -> loop_start = (getNibbleAddress(0));
    header -> loop_end = (getNibbleAddress(samples - 1));
    header -> ca = (getNibbleAddress(0));
    for (int i=0 ; i<16 ; ++i)
        header -> coef[i] = (cxt -> coef[i]);
    header -> gain = (cxt -> gain);
    header -> ps = (cxt -> pred_scale);
    header -> hist1 = (cxt -> yn1);
    header -> hist2 = (cxt -> yn2);
    header -> loop_ps = (cxt -> loop_pred_scale);
    header -> loop_hist1 = (cxt -> loop_yn1);
    header -> loop_hist2 = (cxt -> loop_yn2);
}