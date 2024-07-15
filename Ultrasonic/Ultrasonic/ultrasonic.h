#include "ti_msp_dl_config.h"

#ifndef GPIO_Ultrasonic_PIN_Trig_PORT
#define GPIO_Ultrasonic_PIN_Trig_PORT GPIO_Ultrasonic_PORT
#endif

#ifndef GPIO_Ultrasonic_PIN_Echo_PORT
#define GPIO_Ultrasonic_PIN_Echo_PORT GPIO_Ultrasonic_PORT
#endif

uint32_t getUltrasonicDist(void);