//////////////////////////////////////////////////////////////////////////////////	 
//  文 件 名   : oled.c
//  版 本 号   : v2.0
//  作    者   : Torris
//  生成日期   : 2024-07-08
//  最近修改   : 
//  功能描述   : 0.96寸OLED 接口演示例程(MSPM0G系列)
//  驱动IC     : SSD1306/SSD1315
//              说明: 
//              ----------------------------------------------------------------
//              GND    电源地
//              VCC    接3.3v电源
//              SCL    PA31（时钟）
//              SDA    PA28（数据）    
//******************************************************************************/
#include "ti_msp_dl_config.h"
#include "oled.h"
#include "bmp.h"

int main( void )
{
    SYSCFG_DL_init();
    
    uint8_t t=' ';
    OLED_Init();		//初始化OLED
    while(1) 
    {		
        OLED_DrawBMP(0,0,128,64,BMP1);
        delay_ms(500);
        OLED_Clear();
        OLED_ShowChinese(0,0,0,16);//中
        OLED_ShowChinese(18,0,1,16);//景
        OLED_ShowChinese(36,0,2,16);//园
        OLED_ShowChinese(54,0,3,16);//电
        OLED_ShowChinese(72,0,4,16);//子
        OLED_ShowChinese(90,0,5,16);//科
        OLED_ShowChinese(108,0,6,16);//技
        OLED_ShowString(8,2,(uint8_t *)"ZHONGJINGYUAN",16);
        OLED_ShowString(20,4,(uint8_t *)"2014/05/01",16);
        OLED_ShowString(0,6,(uint8_t *)"ASCII:",16);  
        OLED_ShowString(63,6,(uint8_t *)"CODE:",16);
        OLED_ShowChar(48,6,t,16);
        t++;
        if(t>'~')t=' ';
        OLED_ShowNum(103,6,t,3,16);
        delay_ms(500);
        OLED_Clear();
    }
}
