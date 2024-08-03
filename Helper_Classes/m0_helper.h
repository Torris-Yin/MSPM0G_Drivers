#ifndef __M0_HELPER_H
#define __M0_HELPER_H
#include "arm_math.h"
#include "ti/devices/msp/peripherals/hw_adc12.h"
#include "ti/devices/msp/peripherals/hw_gptimer.h"
#include "ti/devices/msp/peripherals/hw_uart.h"
#include "ti/driverlib/dl_dac12.h"
#include "ti/driverlib/dl_dma.h"
#include <cstddef>
#include <machine/_stdint.h>
#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>
#include "ti_msp_dl_config.h"
#include <cstdint>
#define __SYS_DAC_VREF  3.30000 //Calibrate first
#define MSP_M0_CLK_CYCLES 32000000
#define LED_BUILTIN         GPIO_LEDS_PIN_BUILTIN_LED_PIN
#define LED_BUILTIN_PORT    GPIO_LEDS_PORT
#define HIGH 1
#define LOW  0

typedef struct {
    uint32_t trigger_time;       // 事件触发的时间 (单位: 毫秒)
    void (*callback)(void*);     // 需要调用的函数
    void* param;                 // 函数参数
} Event;

typedef struct {
    bool isUpClicked;
    bool isDownClicked;
    bool isLeftClicked;
    bool isRightClicked;
    bool isClickEventPending;
} Clicked;

void delay_ms(int milsecs);
void delay_us(int usec);
void writeIO(bool IO_STATE, GPIO_Regs *gpio, uint32_t pins);
void UART_SendChar(const char ch, UART_Regs* const uart_dev);
void UART_SendString(const char* str, int len, UART_Regs* const uart_dev);

void analogWrite(float voltage);
void analogWrite(double voltage);
void analogWrite(uint16_t step);
void analogWrite(int step);
void clearClickPending(Clicked* c);


//KEY PAD PORTS
#define     KEY_PORT    GPIO_KEY_PAD_PORT
#define     KEY1        GPIO_KEY_PAD_KEY1_PIN
#define     KEY2        GPIO_KEY_PAD_KEY2_PIN
#define     KEY3        GPIO_KEY_PAD_KEY3_PIN
#define     KEY4        GPIO_KEY_PAD_KEY4_PIN

//RELAY PORTS
#define     RELAY_PORT  GPIO_RELAY_PORT
#define     RELAY       GPIO_RELAY_RELAY_PIN

/*  DCZYewen 提供的类似51单片机的IO口设计 
    为了简化设计  不使用模板构造 以接受可变个数的参数
    对于一个端口  请务必确定它是输入还是输出
    此类不进行输入输出类型的检查  错误调用将产生严重后果*/
class IO51 {
public:
    IO51(void);
    IO51(GPIO_Regs* const _GPIO, uint32_t _PIN);
    operator bool() const;
    bool operator=(bool state);
    bool operator=(int state);

private:
    const GPIO_Regs* __GPIO;
    uint32_t __PIN;
};

class ADCM0 {
public:
    ADCM0(void);
    ADCM0(int dma_chn_id , int16_t * buf , uint16_t len,  GPTIMER_Regs* const timer , ADC12_Regs* const adc12);
    void DMA_Init();
    void DMA_Enable();
    void DMA_Disable();
    void DMA_SetSampleSize(uint16_t size);
    void SetADCSampleRateQuarter_us(uint32_t quarter);
    uint16_t GetSampleSize();

private:
    int16_t*   _buf = NULL;
    uint16_t    _len = 0;
    int         DMA_CHANNEL_ID = -1;
    GPTIMER_Regs * _Timer = NULL;
    ADC12_Regs * _ADC12 = NULL;

};

#endif