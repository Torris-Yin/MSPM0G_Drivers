#ifndef COMMON_CMSIS_DSP
#define COMMON_CMSIS_DSP
#include "arm_math.h"
#include "common_cmsis_dsp.h"
typedef struct {
    q15_t* fft_input_buf;
    q15_t* fft_output_buf;
    uint16_t   fft_buf_len;
} fft_item;
void convert_to_q15(int16_t *input, q15_t *output, uint32_t length);
void fill_buf(fft_item* item , void* buf);
void do_rfft(fft_item* item);// do real domain fft

#endif