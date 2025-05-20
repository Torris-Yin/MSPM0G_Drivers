#include "m0_helper.h"
#include "ti/driverlib/dl_dma.h"
#include "ti/driverlib/dl_timerg.h"
#include "ti_msp_dl_config.h"
#include <cstdint>
#include <stdint.h>

void delay_ms(int milsecs){
    int cycles = MSP_M0_CLK_CYCLES / 1000  * milsecs;
    delay_cycles(cycles);
    return;
};

void delay_us(int usec){
    int cycles = MSP_M0_CLK_CYCLES / 1000000 * usec;
    delay_cycles(cycles);
    return;
};

void writeIO( bool IO_STATE , GPIO_Regs *gpio, uint32_t pins){
    if (IO_STATE){
        DL_GPIO_setPins(gpio, pins);
    }else {
        DL_GPIO_clearPins(gpio, pins);
    }
}


void UART_SendChar(const char ch, UART_Regs* const uart_dev){
    while( DL_UART_isBusy(uart_dev) == true );
    DL_UART_Main_transmitData(uart_dev, ch);
}
void UART_SendString(const char* str , int len,  UART_Regs* const uart_dev){
    if (str == NULL)return;

    for (int i = 0 ; i <len; i ++){
        UART_SendChar(str[i], uart_dev);
    }
}


/* 
//--------------------HELPER_ADC_DMA_FUNCS------------------------//
void helperADC1DMAInit(uint16_t* buf , uint16_t len){
    DL_DMA_setSrcAddr(DMA, DMA_ADC_CHAN_ID, 
    (uint32_t) DL_ADC12_getMemResultAddress(ADC12_0_INST , DL_ADC12_MEM_IDX_0));
    DL_DMA_setDestAddr(DMA, DMA_ADC_CHAN_ID, (uint32_t) buf);
    DL_DMA_setTransferSize(DMA, DMA_ADC_CHAN_ID, len);
}

void helperADC1DMAenable(){
    DL_DMA_enableChannel(DMA, DMA_ADC_CHAN_ID);
}

void helperADC1DMAdisable(){
    DL_DMA_disableChannel(DMA, DMA_ADC_CHAN_ID);
}

void helperSetADC1DMASampleQuarter_us(uint32_t quarter){
    // TIMER_ADC_INST_LOAD_VALUE = (1us * 32000000 Hz) - 1
    static const DL_TimerG_TimerConfig adctimer_cfg = {
        .timerMode  = DL_TIMER_TIMER_MODE_PERIODIC,
        .period     = 8*quarter - 1,
        .startTimer = DL_TIMER_START,
    };
    DL_TimerG_initTimerMode(TIMER_ADC_INST,
    (DL_TimerG_TimerConfig *) &adctimer_cfg);
}
 */


void analogWrite(float voltage){
    if (voltage > __SYS_DAC_VREF){
        __BKPT();
        return;
    }
    DL_DAC12_output12(DAC0 , voltage / __SYS_DAC_VREF *4095);
}
void analogWrite(double voltage){
    if (voltage > __SYS_DAC_VREF){
        __BKPT();
        return;
    }
    DL_DAC12_output12(DAC0 , ((float)voltage) / __SYS_DAC_VREF *4095);
}
void analogWrite(uint16_t step){
    if (step > 4095){
        __BKPT();
    }
    DL_DAC12_output12(DAC0 , step);
}
void analogWrite(int step){
    //passing arguments
    analogWrite((uint16_t) step);
}
void clearClickPending(Clicked* c){
    c->isDownClicked = false;
    c->isUpClicked = false;
    c->isRightClicked = false;
    c->isLeftClicked = false;
    c->isClickEventPending = false;
}


/*  DCZYewen 提供的类似51单片机的IO口设计 
    为了简化设计  不使用模板构造 以接受可变个数的参数
    对于一个端口  请务必确定它是输入还是输出
    此类不进行输入输出类型的检查  错误调用将产生严重后果*/

IO51::IO51(void){
//default constructor does nothing but start a breakpoint
    __BKPT();
}
IO51::IO51(GPIO_Regs* const _GPIO , uint32_t _PIN){
    this->__PIN = _PIN;
    this->__GPIO = _GPIO;
}
bool IO51::operator=(bool state){
    writeIO(state, (GPIO_Regs *) __GPIO, __PIN);
    return state;
}
bool IO51::operator=(int state){
    writeIO((bool) state, (GPIO_Regs *) __GPIO, __PIN);
    return state;
}
/*  注意  你希望它悬空时是什么电平  请在sysconfig中配置电阻
    默认状态下没有电阻 悬空是高电平 但是不保证每次都出来是高
    请在需要悬空高电平是配置上拉电阻*/
IO51::operator bool() const {
    bool t  =  __PIN & DL_GPIO_readPins((GPIO_Regs *) __GPIO , __PIN);
    //我不知道为什么直接return不行  但是这样就行了  it just works
    return t;
}

ADCM0::ADCM0(void){
    __BKPT();
}

ADCM0::ADCM0(int dma_chn_id , int16_t * buf , 
uint16_t len, GPTIMER_Regs* const timer, ADC12_Regs* const adc12){
    this->_buf = buf;
    this->DMA_CHANNEL_ID = dma_chn_id;
    this->_len = len;
    this->_Timer = timer;
    this->_ADC12 = adc12;
}
void ADCM0::DMA_Init(){
    if (_len == 0 | DMA_CHANNEL_ID == -1 | _Timer == NULL| _buf == NULL){
        __BKPT();
    }
    DL_DMA_setSrcAddr(DMA, DMA_CHANNEL_ID, 
    (uint32_t) DL_ADC12_getMemResultAddress(_ADC12  , DL_ADC12_MEM_IDX_0));
    DL_DMA_setDestAddr(DMA, DMA_CHANNEL_ID, (uint32_t) _buf);
    DL_DMA_setTransferSize(DMA, DMA_CHANNEL_ID, _len);
}
void ADCM0::DMA_Enable(){
    DL_DMA_enableChannel(DMA, DMA_CHANNEL_ID);
}
void ADCM0::DMA_SetSampleSize(uint16_t size){
    this->_len = size;
    DL_DMA_setTransferSize(DMA, DMA_CHANNEL_ID, _len);
}
void ADCM0::DMA_Disable(){
    DL_DMA_disableChannel(DMA, DMA_CHANNEL_ID);
}
void ADCM0::SetADCSampleRateQuarter_us(uint32_t quarter){
    // TIMER_ADC_INST_LOAD_VALUE = (1us * 32000000 Hz) - 1
    static const DL_TimerG_TimerConfig adctimer_cfg = {
        .timerMode  = DL_TIMER_TIMER_MODE_PERIODIC,
        .period     = 8*quarter - 1,
        .startTimer = DL_TIMER_START,
    };
    DL_TimerG_initTimerMode(_Timer,
    (DL_TimerG_TimerConfig *) &adctimer_cfg);
}
uint16_t ADCM0::GetSampleSize(){
    return _len;
}