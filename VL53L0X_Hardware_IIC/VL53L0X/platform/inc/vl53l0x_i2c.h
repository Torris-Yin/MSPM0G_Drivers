#ifndef __VL53L0_I2C_H
#define __VL53L0_I2C_H

#include "board.h"

#ifndef u8
#define u8 uint8_t
#endif
#ifndef u16
#define u16 uint16_t
#endif
#ifndef u32
#define u32 uint32_t
#endif


//设置SDA输出模式
#define SDA_OUT()   {                                                  \
                        DL_GPIO_initDigitalOutput(VL53L0X_SDA_IOMUX);     \
                        DL_GPIO_setPins(VL53L0X_PORT, VL53L0X_SDA_PIN);      \
                        DL_GPIO_enableOutput(VL53L0X_PORT, VL53L0X_SDA_PIN); \
                    }
//设置SDA输入模式
#define SDA_IN()    { DL_GPIO_initDigitalInput(VL53L0X_SDA_IOMUX); }
//获取SDA引脚的电平变化
#define SDA_GET()   ( ( ( DL_GPIO_readPins(VL53L0X_PORT,VL53L0X_SDA_PIN) & VL53L0X_SDA_PIN ) > 0 ) ? 1 : 0 )
//SDA与SCL输出
#define SDA(x)      ( (x) ? (DL_GPIO_setPins(VL53L0X_PORT,VL53L0X_SDA_PIN)) : (DL_GPIO_clearPins(VL53L0X_PORT,VL53L0X_SDA_PIN)) )
#define SCL(x)      ( (x) ? (DL_GPIO_setPins(VL53L0X_PORT,VL53L0X_SCL_PIN)) : (DL_GPIO_clearPins(VL53L0X_PORT,VL53L0X_SCL_PIN)) )

//状态
#define STATUS_OK       0x00
#define STATUS_FAIL     0x01

//IIC操作函数
void VL53L0X_i2c_init(void);//初始化IIC的IO口

u8 VL53L0X_write_byte(u8 address,u8 index,u8 data);              //IIC写一个8位数据
u8 VL53L0X_write_word(u8 address,u8 index,u16 data);             //IIC写一个16位数据
u8 VL53L0X_write_dword(u8 address,u8 index,u32 data);            //IIC写一个32位数据
u8 VL53L0X_write_multi(u8 address, u8 index,u8 *pdata,u16 count);//IIC连续写

u8 VL53L0X_read_byte(u8 address,u8 index,u8 *pdata);             //IIC读一个8位数据
u8 VL53L0X_read_word(u8 address,u8 index,u16 *pdata);            //IIC读一个16位数据
u8 VL53L0X_read_dword(u8 address,u8 index,u32 *pdata);           //IIC读一个32位数据
u8 VL53L0X_read_multi(u8 address,u8 index,u8 *pdata,u16 count);  //IIC连续读


#endif 


