#ifndef __OLED_H
#define __OLED_H			  	 
#include "ti_msp_dl_config.h"
#define  u8 unsigned char 
#define  u16 unsigned int
#define  u32 unsigned int
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

#ifndef GPIO_OLED_PIN_SCL_PORT
#define GPIO_OLED_PIN_SCL_PORT GPIO_OLED_PORT 
#endif

#ifndef GPIO_OLED_PIN_SDA_PORT
#define GPIO_OLED_PIN_SDA_PORT GPIO_OLED_PORT 
#endif

//----------------------------------------------------------------------------------
//OLED SSD1306 I2C 时钟SCL
#define		OLED_SCL_Set()			    (DL_GPIO_setPins(GPIO_OLED_PIN_SCL_PORT, GPIO_OLED_PIN_SCL_PIN))
#define		OLED_SCL_Clr()				(DL_GPIO_clearPins(GPIO_OLED_PIN_SCL_PORT, GPIO_OLED_PIN_SCL_PIN))

//----------------------------------------------------------------------------------
//OLED SSD1306 I2C 数据SDA
#define		OLED_SDA_Set()				(DL_GPIO_setPins(GPIO_OLED_PIN_SDA_PORT, GPIO_OLED_PIN_SDA_PIN))
#define		OLED_SDA_Clr()			    (DL_GPIO_clearPins(GPIO_OLED_PIN_SDA_PORT, GPIO_OLED_PIN_SDA_PIN))
				   

//OLED控制用函数
void delay_ms(unsigned int ms);
void OLED_ColorTurn(u8 i);
void OLED_DisplayTurn(u8 i);
void OLED_WR_Byte(u8 dat,u8 cmd);
void OLED_Set_Pos(u8 x, u8 y);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Clear(void);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 sizey);
u32 oled_pow(u8 m,u8 n);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 sizey);
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 sizey);
void OLED_ShowChinese(u8 x,u8 y,u8 no,u8 sizey);
void OLED_DrawBMP(u8 x,u8 y,u8 sizex, u8 sizey,u8 BMP[]);
void OLED_Init(void);
#endif  
	 



