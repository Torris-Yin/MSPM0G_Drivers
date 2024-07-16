/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 立创论坛：https://oshwhub.com/forum
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 * Change Logs:
 * Date           Author       Notes
 * 2024-03-29     LCKFB-LP    first version
 */
#include "bsp_vl53l0x.h"


VL53L0X_Dev_t vl53l0x_dev;//设备I2C数据参数
VL53L0X_DeviceInfo_t vl53l0x_dev_info;//设备ID版本信息
uint8_t AjustOK=0;//校准标志位

//VL53L0X各测量模式参数
//0：默认;1:高精度;2:长距离;3:高速
mode_data Mode_data[]=
{
    {(FixPoint1616_t)(0.25*65536), 
	 (FixPoint1616_t)(18*65536),
	 33000,
	 14,
	 10},//默认
		
	{(FixPoint1616_t)(0.25*65536) ,
	 (FixPoint1616_t)(18*65536),
	 200000, 
	 14,
	 10},//高精度
		
    {(FixPoint1616_t)(0.1*65536) ,
	 (FixPoint1616_t)(60*65536),
	 33000,
	 18,
	 14},//长距离
	
    {(FixPoint1616_t)(0.25*65536) ,
	 (FixPoint1616_t)(32*65536),
	 20000,
	 14,
	 10},//高速
		
};

//API错误信息打印
//Status：详情看VL53L0X_Error参数的定义
void print_pal_error(VL53L0X_Error Status)
{
	
	char buf[VL53L0X_MAX_STRING_LENGTH];
	
	VL53L0X_GetPalErrorString(Status,buf);//根据Status状态获取错误信息字符串
	
    printf("API Status: %i : %s\r\n",Status, buf);//打印状态和错误信息
}


//配置VL53L0X设备I2C地址
//dev:设备I2C参数结构体
//newaddr:设备新I2C地址
VL53L0X_Error vl53l0x_Addr_set(VL53L0X_Dev_t *dev,uint8_t newaddr)
{
	uint16_t Id;
	uint8_t FinalAddress;
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	u8 sta=0x00;
	
	FinalAddress = newaddr;
	
	if(FinalAddress==dev->I2cDevAddr)//新设备I2C地址与旧地址一致,直接退出
		return VL53L0X_ERROR_NONE;
	//在进行第一个寄存器访问之前设置I2C标准模式(400Khz)
	Status = VL53L0X_WrByte(dev,0x88,0x00);
	if(Status!=VL53L0X_ERROR_NONE) 
	{
		sta=0x01;//设置I2C标准模式出错
		goto set_error;
	}
	//尝试使用默认的0x52地址读取一个寄存器
	Status = VL53L0X_RdWord(dev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, &Id);
	if(Status!=VL53L0X_ERROR_NONE) 
	{
		sta=0x02;//读取寄存器出错
		goto set_error;
	}
	if(Id == 0xEEAA)
	{
		//设置设备新的I2C地址
		Status = VL53L0X_SetDeviceAddress(dev,FinalAddress);
		if(Status!=VL53L0X_ERROR_NONE) 
		{
			sta=0x03;//设置I2C地址出错
			goto set_error;
		}
		//修改参数结构体的I2C地址
		dev->I2cDevAddr = FinalAddress;
		//检查新的I2C地址读写是否正常
		Status = VL53L0X_RdWord(dev, VL53L0X_REG_IDENTIFICATION_MODEL_ID, &Id);
		if(Status!=VL53L0X_ERROR_NONE) 
		{
			sta=0x04;//新I2C地址读写出错
			goto set_error;
		}	
	}
	set_error:
	if(Status!=VL53L0X_ERROR_NONE)
	{
		print_pal_error(Status);//打印错误信息
	}
	if(sta!=0)
	  printf("sta:0x%x\r\n",sta);
	return Status;
}

//vl53l0x复位函数
//dev:设备I2C参数结构体
void vl53l0x_reset(VL53L0X_Dev_t *dev)
{
	uint8_t addr;
	addr = dev->I2cDevAddr;//保存设备原I2C地址
    VL53L0X_Xshut(0);//失能VL53L0X
	delay_ms(30);
	VL53L0X_Xshut(1);//使能VL53L0X,让传感器处于工作(I2C地址会恢复默认0X52)
	delay_ms(30);	
	dev->I2cDevAddr=0x52;
	vl53l0x_Addr_set(dev,addr);//设置VL53L0X传感器原来上电前原I2C地址
	VL53L0X_DataInit(dev);	
}

//初始化vl53l0x
//dev:设备I2C参数结构体
VL53L0X_Error vl53l0x_init(VL53L0X_Dev_t *dev)
{
	VL53L0X_Error Status = VL53L0X_ERROR_NONE;
	VL53L0X_Dev_t *pMyDevice = dev;
	
	pMyDevice->I2cDevAddr = VL53L0X_Addr;//I2C地址(上电默认0x52)
	pMyDevice->comms_type = 1;           //I2C通信模式
	pMyDevice->comms_speed_khz = 400;    //I2C通信速率
	
	VL53L0X_i2c_init();//初始化IIC总线
	
	VL53L0X_Xshut(0);//失能VL53L0X
	delay_ms(50);
	VL53L0X_Xshut(1);//使能VL53L0X,让传感器处于工作
	delay_ms(50);
	
    vl53l0x_Addr_set(pMyDevice,0x52);//设置VL53L0X传感器I2C地址
    if(Status!=VL53L0X_ERROR_NONE) goto error;
	Status = VL53L0X_DataInit(pMyDevice);//设备初始化
	if(Status!=VL53L0X_ERROR_NONE) goto error;
	delay_ms(2);
	Status = VL53L0X_GetDeviceInfo(pMyDevice,&vl53l0x_dev_info);//获取设备ID信息
    if(Status!=VL53L0X_ERROR_NONE) goto error;
	
	error:
	if(Status!=VL53L0X_ERROR_NONE)
	{
		print_pal_error(Status);//打印错误信息
		return Status;
	}
  	
	return Status;
}
