#include "common_cmsis_dsp.h"
#include "arm_math.h"
#include <cstdint>
#include <cstring>

void convert_to_q15(int16_t *input, q15_t *output, uint32_t length) {
    for (int i = 0 ; i < length ; i++){
        float32_t t = input[i]/4095;
        arm_float_to_q15(&t , &output[i], length);
    }
}//converting array of int16_t to q15 format

void fill_buf(fft_item* item , uint16_t * buf , uint16_t len){
    for (int i = 0 ; i < (item->fft_buf_len) ;i++){
        if (i<len){
            item->fft_input_buf[i] = buf[i];
        }else
        item->fft_input_buf[i] = 0;// fill the extra ones with zeros
    }
}

void do_rfft(fft_item* item) {
    // 创建RFFT实例
    arm_rfft_instance_q15 rfft_instance;
    arm_status status = arm_rfft_init_q15(&rfft_instance, item->fft_buf_len, 0, 1);

    if (status == ARM_MATH_SUCCESS) {
        // 执行RFFT
        arm_rfft_q15(&rfft_instance, item->fft_input_buf, item->fft_output_buf);

        // 计算幅值
        arm_cmplx_mag_q15(item->fft_output_buf, item->fft_output_buf, item->fft_buf_len / 2);
    }
}