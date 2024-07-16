#include "vl53l0x_cali.h"


_vl53l0x_adjust Vl53l0x_adjust; //校准数据24c02写缓存区(用于在校准模式校准数据写入24c02)
_vl53l0x_adjust Vl53l0x_data;   //校准数据24c02读缓存区（用于系统初始化时向24C02读取数据）
           
#define adjust_num 5//校准错误次数

//VL53L0X校准函数
//dev:设备I2C参数结构体
VL53L0X_Error vl53l0x_adjust(VL53L0X_Dev_t *dev)
{
	
	VL53L0X_DeviceError Status = VL53L0X_ERROR_NONE;
	uint32_t refSpadCount = 7;
	uint8_t  isApertureSpads = 0;
	uint32_t XTalkCalDistance = 100;
	uint32_t XTalkCompensationRateMegaCps;
	uint32_t CalDistanceMilliMeter = 100<<16;
	int32_t  OffsetMicroMeter = 30000;
	uint8_t VhvSettings = 23;
	uint8_t PhaseCal = 1;
	u8 i=0;

	VL53L0X_StaticInit(dev);//数值恢复默认,传感器处于空闲状态
//    LED1=0;
	//SPADS校准----------------------------
	spads:
	delay_ms(10);
	printf("The SPADS Calibration Start...\r\n");
	Status = VL53L0X_PerformRefSpadManagement(dev,&refSpadCount,&isApertureSpads);//执行参考Spad管理
	if(Status == VL53L0X_ERROR_NONE)
	{
	    printf("refSpadCount = %d\r\n",refSpadCount);
	    Vl53l0x_adjust.refSpadCount = refSpadCount;
	    printf("isApertureSpads = %d\r\n",isApertureSpads);	
	    Vl53l0x_adjust.isApertureSpads = isApertureSpads;
        printf("The SPADS Calibration Finish...\r\n\r\n");		
	    i=0;
	}
	else
	{
	    i++;
	    if(i==adjust_num) return Status;
	    printf("SPADS Calibration Error,Restart this step\r\n");
	    goto spads;
	}
	//设备参考校准---------------------------------------------------
	ref:
	delay_ms(10);
	printf("The Ref Calibration Start...\r\n");
	Status = VL53L0X_PerformRefCalibration(dev,&VhvSettings,&PhaseCal);//Ref参考校准
	if(Status == VL53L0X_ERROR_NONE)
	{
		printf("VhvSettings = %d\r\n",VhvSettings);
		Vl53l0x_adjust.VhvSettings = VhvSettings;
		printf("PhaseCal = %d\r\n",PhaseCal);
		Vl53l0x_adjust.PhaseCal = PhaseCal;
		printf("The Ref Calibration Finish...\r\n\r\n");
		i=0;
	}
	else
	{
		i++;
		if(i==adjust_num) return Status;
		printf("Ref Calibration Error,Restart this step\r\n");
		goto ref;
	}
	//偏移校准------------------------------------------------
	offset:
	delay_ms(10);
	printf("Offset Calibration:need a white target,in black space,and the distance keep 100mm!\r\n");
	printf("The Offset Calibration Start...\r\n");
	
	Status = VL53L0X_PerformOffsetCalibration(dev,CalDistanceMilliMeter,&OffsetMicroMeter);//偏移校准
	if(Status == VL53L0X_ERROR_NONE)
	{
		printf("CalDistanceMilliMeter = %d mm\r\n",CalDistanceMilliMeter);
		Vl53l0x_adjust.CalDistanceMilliMeter = CalDistanceMilliMeter;
		printf("OffsetMicroMeter = %d mm\r\n",OffsetMicroMeter);	
		Vl53l0x_adjust.OffsetMicroMeter = OffsetMicroMeter;
		printf("The Offset Calibration Finish...\r\n\r\n");
		i=0;
	}
	else
	{
		i++;
		if(i==adjust_num) return Status;
		printf("Offset Calibration Error,Restart this step\r\n");
		goto offset;
	}
	//串扰校准-----------------------------------------------------
	xtalk:
	delay_ms(20);
	printf("Cross Talk Calibration:need a grey target\r\n");
	printf("The Cross Talk Calibration Start...\r\n");	
	Status = VL53L0X_PerformXTalkCalibration(dev,XTalkCalDistance,&XTalkCompensationRateMegaCps);//串扰校准
	if(Status == VL53L0X_ERROR_NONE)
	{
		printf("XTalkCalDistance = %d mm\r\n",XTalkCalDistance);
		Vl53l0x_adjust.XTalkCalDistance = XTalkCalDistance;
		printf("XTalkCompensationRateMegaCps = %d\r\n",XTalkCompensationRateMegaCps);	
		Vl53l0x_adjust.XTalkCompensationRateMegaCps = XTalkCompensationRateMegaCps;
		printf("The Cross Talk Calibration Finish...\r\n\r\n");
		i=0;
	}
	else
	{
		i++;
		if(i==adjust_num) return Status;
		printf("Cross Talk Calibration Error,Restart this step\r\n");
		goto xtalk;
	}
//	LED1=1;
	printf("All the Calibration has Finished!\r\n");
	printf("Calibration is successful!!\r\n");

	Vl53l0x_adjust.adjustok = 0xAA;//校准成功
//	AT24CXX_Write(0,(u8*)&Vl53l0x_adjust,sizeof(_vl53l0x_adjust));//将校准数据保存到24c02
	memcpy(&Vl53l0x_data,&Vl53l0x_adjust,sizeof(_vl53l0x_adjust));
	return Status;
}
