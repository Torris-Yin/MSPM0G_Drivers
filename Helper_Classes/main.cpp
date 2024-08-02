#include "arm_math.h"
#include "ti_msp_dl_config.h"
#include "m0_helper.h"
#include <cstdint>
#include <cstring>
#include "stdio.h"
#include "stdlib.h"
#include "common_def.h"

void add_event(uint32_t delay, void (*callback)(void*), void* param) {
    if (event_count < MAX_EVENTS) {
        event_list[event_count].trigger_time = current_time + delay;
        event_list[event_count].callback = callback;
        event_list[event_count].param = param;
        event_count++;
    }
}

void example_callback(void* param) {
    //__BKPT();
}

int main(void)
{
    SYSCFG_DL_init();
    //Serial Port
    NVIC_ClearPendingIRQ(SProbe_INST_INT_IRQN);
    NVIC_EnableIRQ(SProbe_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(Serial_Screen_INST_INT_IRQN);
    NVIC_EnableIRQ(Serial_Screen_INST_INT_IRQN);

    //ADC12
    ADCM0 _ADC0(DMA_ADC0_CHAN_ID , ADC0_DATA_SEGMENT , ADC_DATA_SEGMENT_LENGTH,
    TIMER_ADC0_INST , ADC12_0_INST);
    _ADC0.DMA_Init();
    _ADC0.DMA_Enable();
    
    ADCM0 _ADC1(DMA_ADC1_CHAN_ID , ADC1_DATA_SEGMENT , ADC_DATA_SEGMENT_LENGTH,
    TIMER_ADC1_INST , ADC12_1_INST);
    _ADC1.DMA_Init();
    _ADC1.DMA_Enable();

    NVIC_ClearPendingIRQ(ADC12_0_INST_INT_IRQN);
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
    NVIC_ClearPendingIRQ(ADC12_1_INST_INT_IRQN);
    NVIC_EnableIRQ(ADC12_1_INST_INT_IRQN);


    //helperDACInit();

    //DAC12
    for (int i = 0 ; i < 64 ; i++){
        int16_t step = ((1.0 + arm_sin_f32(i/32.0*PI))/2.0)*4095;
        DAC_DATA_SEGMENT[i] = step;
    }

    helperDACDMAInit(DAC_DATA_SEGMENT , DAC_DATA_SEGMENT_LENGTH);
    helperDACDMAenable();


    delay_ms(1000);//等待一秒 DAC ADC稳定

    DL_TimerG_startCounter(TIMER_ADC0_INST);
    DL_TimerG_startCounter(TIMER_ADC1_INST);//start ADC sampling
    



    while (1) {

/*         sprintf(SSCMD, "aminuosi%d\xff\xff\xff", 996);
        UART_SendString(SSCMD, strlen(SSCMD), Serial_Screen_INST); */

        //UART_SendString("Hello, world!", 12 , Serial_Screen_INST);
/*         for ( int i = 0 ; i < 4095 ; i++){
            analogWrite(i);
        } */

/*         IO51 PA24(IO0_PORT , IO0);
        PA24 = 1;
        delay_ms(500);
        PA24 = 0;
        delay_ms(500); */

/*         IO51 PB19(IO9_PORT , IO9);
        IO51 PA24(IO0_PORT , IO0);
        //万万不可以把输入口当做左值 输出口当做右值
        bool t = PB19;
        PA24 = t;
        delay_ms(100); */

    }
}

extern "C" void SProbe_INST_IRQHandler(void){
    switch (DL_UART_getPendingInterrupt(SProbe_INST)) {
        case DL_UART_MAIN_IIDX_RX:
            gEchoData = DL_UART_Main_receiveData(SProbe_INST);
            DL_UART_Main_transmitData(SProbe_INST, gEchoData);
            break;
        default:
            break;
    }
}

extern "C" void Serial_Screen_INST_IRQHandler(void){
    switch (DL_UART_getPendingInterrupt(Serial_Screen_INST)) {
        case DL_UART_MAIN_IIDX_RX:
            gEchoData = DL_UART_Main_receiveData(Serial_Screen_INST);
            DL_UART_Main_transmitData(Serial_Screen_INST, gEchoData);
            break;
        default:
            break;
    }
}

extern "C" void ADC12_0_INST_IRQHandler(void){
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_DMA_DONE:{
            DL_TimerG_stopCounter(TIMER_ADC0_INST);
            //processing
            //__BKPT();
            if (flagADC0Working)
            DL_TimerG_startCounter(TIMER_ADC0_INST);
            break;
            }
        default:
            break;
    }
}

extern "C" void ADC12_1_INST_IRQHandler(void){
    switch (DL_ADC12_getPendingInterrupt(ADC12_1_INST)) {
        case DL_ADC12_IIDX_DMA_DONE:{
            DL_TimerG_stopCounter(TIMER_ADC1_INST);
            //processing
            //__BKPT();
            if (flagADC1Working)
            DL_TimerG_startCounter(TIMER_ADC1_INST);
            break;
            }
        default:
            break;
    }
}

extern "C" void SysTick_Handler(void) {
    if(KUP == 0&&(clicky.isClickEventPending == false)){
        clicky.isUpClicked = true;
        clicky.isClickEventPending = true;
    }
    if(KDN == 0&&(clicky.isClickEventPending == false)){
        clicky.isDownClicked = true;
        clicky.isClickEventPending = true;
    }
    if(KLT == 0&&(clicky.isClickEventPending == false)){
        clicky.isLeftClicked = true;
        clicky.isClickEventPending = true;
    }
    if(KRT == 0&&(clicky.isClickEventPending == false)){
        clicky.isRightClicked = true;
        clicky.isClickEventPending = true;
    }


    current_time++;

    for (uint32_t i = 0; i < event_count; i++) {
        if (event_list[i].trigger_time <= current_time) {
            // 调用事件的回调函数
            event_list[i].callback(event_list[i].param);

            // 移除已触发的事件
            for (uint32_t j = i; j < event_count - 1; j++) {
                event_list[j] = event_list[j + 1];
            }
            event_count--;
            i--;  // 调整索引以检查下一个事件
        }
    }
}