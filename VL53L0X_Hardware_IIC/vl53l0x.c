/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "board.h"
#include <stdio.h>
#include "bsp_vl53l0x.h"

extern VL53L0X_Dev_t vl53l0x_dev;//设备I2C数据参数

int main(void)
{
	//开发板初始化
	board_init();
	
    uint8_t mode = 0;//0：默认;1:高精度;2:长距离;3:高速
    VL53L0X_Error Status=VL53L0X_ERROR_NONE;//工作状态
    
    printf("Start\r\n");
    
    while(vl53l0x_init(&vl53l0x_dev))//vl53l0x初始化
    {
        printf("VL53L0X Error!!!\n\r");
        delay_ms(500);
    }
    printf("VL53L0X OK\r\n");
    
    while(vl53l0x_set_mode(&vl53l0x_dev,mode))//配置测量模式
    {
        printf("Mode Set Error\r\n");
        delay_ms(500);
    }
    
    while(1) 
    {
         if(Status==VL53L0X_ERROR_NONE)
         {
                //执行单次测距并获取测距测量数据
                Status = VL53L0X_PerformSingleRangingMeasurement(&vl53l0x_dev, &vl53l0x_data);
                printf("d: %4imm\r\n",vl53l0x_data.RangeMilliMeter);//打印测量距离
        }
        else
        {
           printf("error\r\n");
        }
        delay_ms(500);
    }
}

