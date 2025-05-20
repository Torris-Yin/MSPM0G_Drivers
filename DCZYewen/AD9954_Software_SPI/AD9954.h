#ifndef __AD9954_H
#define __AD9954_H	 
#include "AD9954.h"
#include <cstdint>

/*
    Ported by DCZYewen Github:https://github.com/DCZYewen
*/


/* #define AD9954_CS 	PAout(3)	
#define AD9954_SCLK PAout(4)	
#define AD9954_SDIO PAout(5)	
#define AD9954_OSK 	PCout(0)
#define PS1 				PAout(2)
#define PS0 				PBout(10)
#define IOUPDATE 		PAout(7)

#define AD9954_SDO	PAin(8)
#define AD9954_IOSY PBout(1)
#define AD9954_RES 	PAout(6)
#define AD9954_PWR 	PBout(0) */


#define CFR1    0X00	//控制功能寄存器1
#define CFR2    0X01	//控制功能寄存器2
#define ASF     0X02	//振幅比例因子寄存器
#define ARR     0X03	//振幅斜坡速率寄存器

#define FTW0    0X04	//频率调谐字寄存器0
#define POW0    0X05	//相位偏移字寄存器
#define FTW1    0X06	//频率调谐字寄存器1

#define NLSCW   0X07	//下降扫描控制字寄存器
#define PLSCW   0X08	//上升扫描控制字寄存器

#define RSCW0   0X07	//
#define RSCW1   0X08	//
#define RSCW2   0X09	//
#define RSCW3   0X0A	//RAM段控制字寄存器
#define RAM     0X0B	//读取指令写入RAM签名寄存器数据

#define No_Dwell	0x04	//No_Dwell不停留，输出扫频到终止频率回到起始频率。
#define Dwell 		0x00	//Dwell停留，输出扫频到终止频率后保持在终止频率。

extern "C" {
    void AD9954_GPIO_Init(void);//初始化控制AD9954需要用到的IO口
    void AD9954_RESET(void);		//复位AD9954
    void UPDATE(void);					//产生一个更新信号，更新AD9954内部寄存器

    void AD9954_Send_Byte(uint8_t dat);//向AD9954发送一个字节的内容
    uint8_t AD9954_Read_Byte(void);			//读AD9954一个字节的内容
    void AD9954_Write_nByte(uint8_t RegAddr,uint8_t *Data,uint8_t Len);//向AD9954指定的寄存器写数据
    uint32_t AD9954_Read_nByte(uint8_t ReadAddr,uint8_t Len);					//读AD9954寄存器数据
    uint32_t Get_FTW(double Real_fH);	//频率数据转换


    void AD9954_Init(void);//初始化AD9954的管脚和内部寄存器的配置，
    void AD9954_Set_Fre(double fre);//设置AD9954输出频率，点频
    void AD9954_Set_Amp(uint16_t Ampli);//写幅度
    void AD9954_Set_Phase(uint16_t Phase);//写相位
    
    void AD9954_SetFSK(double f1,double f2,double f3,double f4,uint16_t Ampli);//AD9954的FSK参数设置
    void AD9954_SetPSK(uint16_t Phase1,uint16_t Phase2,uint16_t Phase3,uint16_t Phase4,double fre,uint16_t Ampli);//AD9954的PSK参数设置
    void AD9954_Set_LinearSweep(double Freq_Low,double Freq_High,double  UpStepFreq, uint8_t UpStepTime,
    double	DownStepFreq, uint8_t DownStepTime,uint8_t mode);//扫频参数设置
}


#endif

