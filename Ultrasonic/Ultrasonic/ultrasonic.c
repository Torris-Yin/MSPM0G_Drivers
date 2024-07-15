#include "ultrasonic.h"

uint8_t overflowFlag;

void delay_us(uint32_t us)
{
    while(us--)
    {
        delay_cycles(CPUCLK_FREQ/1000000);
    }
}

uint32_t getUltrasonicDist(void)
{
    uint32_t cnt, dist;
    DL_GPIO_setPins(GPIO_Ultrasonic_PORT, GPIO_Ultrasonic_PIN_Trig_PIN);
    delay_us(20);
    DL_GPIO_clearPins(GPIO_Ultrasonic_PORT, GPIO_Ultrasonic_PIN_Trig_PIN);

    while(!DL_GPIO_readPins(GPIO_Ultrasonic_PORT, GPIO_Ultrasonic_PIN_Echo_PIN));

    overflowFlag = 0;
    DL_TimerG_setTimerCount(TIMER_Ultrasonic_INST, 0);
    DL_TimerG_startCounter(TIMER_Ultrasonic_INST);

    while(DL_GPIO_readPins(GPIO_Ultrasonic_PORT, GPIO_Ultrasonic_PIN_Echo_PIN) && !overflowFlag);

    DL_TimerG_stopCounter(TIMER_Ultrasonic_INST);

    if(overflowFlag)
    {
        dist = 0;
    }
    else 
    {
        cnt = DL_TimerG_getTimerCount(TIMER_Ultrasonic_INST);
        dist = cnt*0.017;
    }
    return dist;
}

void TIMER_Ultrasonic_INST_IRQHandler(void)
{
    switch (DL_TimerG_getPendingInterrupt(TIMER_Ultrasonic_INST)) {
        case DL_TIMER_IIDX_LOAD:
            overflowFlag = 1;
            break;
        default:
            break;
    }
}