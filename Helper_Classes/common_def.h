#include "m0_helper.h"
#include <cstdint>
volatile uint8_t gEchoData = 0;
#define SSCMD   SerialScreenCommand
#define DAC_DATA_SEGMENT_LENGTH 64
#define ADC_DATA_SEGMENT_LENGTH 1024
uint16_t    DAC_DATA_SEGMENT[DAC_DATA_SEGMENT_LENGTH];
uint16_t    ADC0_DATA_SEGMENT[ADC_DATA_SEGMENT_LENGTH];
uint16_t    ADC1_DATA_SEGMENT[ADC_DATA_SEGMENT_LENGTH];
bool    flagADC0Working = false;
bool    flagADC1Working = false;

IO51 LED(LED_BUILTIN_PORT , LED_BUILTIN);

IO51 KDN(KEY_PORT , KEY1);
IO51 KLT(KEY_PORT , KEY2);
IO51 KRT(KEY_PORT , KEY3);
IO51 KUP(KEY_PORT , KEY4);

Clicked clicky = {0};

char SerialScreenCommand[32];

#define MAX_EVENTS 8
static Event event_list[MAX_EVENTS];
static uint32_t event_count = 0;
// 当前时间 (单位: 毫秒)
static uint32_t current_time = 0;