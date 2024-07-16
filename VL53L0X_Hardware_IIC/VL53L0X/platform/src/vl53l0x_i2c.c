#include "vl53l0x_i2c.h"
#include "board.h"

enum I2cControllerStatus {
    I2C_STATUS_IDLE = 0,
    I2C_STATUS_TX_STARTED,
    I2C_STATUS_TX_INPROGRESS,
    I2C_STATUS_TX_COMPLETE,
    I2C_STATUS_RX_STARTED,
    I2C_STATUS_RX_INPROGRESS,
    I2C_STATUS_RX_COMPLETE,
    I2C_STATUS_ERROR,
} gI2cControllerStatus;

uint32_t gTxLen, gTxCount, gRxCount, gRxLen;
uint8_t gTxPacket[128];
uint8_t gRxPacket[128];

//VL53L0X I2C初始化
void VL53L0X_i2c_init(void)
{

}

//IIC写n字节数据
u8 VL_IIC_Write_nByte(u8 addr, u8 regaddr,u16 num, u8 *regdata)
{
    uint16_t i;

    gI2cControllerStatus = I2C_STATUS_IDLE;
    gTxLen = num+1;

    gTxPacket[0] = regaddr;
    for(i=1; i<=num; i++)
    {
        gTxPacket[i] = (uint8_t)regdata[i-1];
    }

    gTxCount = DL_I2C_fillControllerTXFIFO(I2C_VL53L0X_INST, &gTxPacket[0], gTxLen);

    if (gTxCount < gTxLen) 
    {
        DL_I2C_enableInterrupt(I2C_VL53L0X_INST, DL_I2C_INTERRUPT_CONTROLLER_TXFIFO_TRIGGER);
    } 
    else 
    {
        DL_I2C_disableInterrupt(I2C_VL53L0X_INST, DL_I2C_INTERRUPT_CONTROLLER_TXFIFO_TRIGGER);
    }

    gI2cControllerStatus = I2C_STATUS_TX_STARTED;
    while (!(DL_I2C_getControllerStatus(I2C_VL53L0X_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));
    DL_I2C_startControllerTransfer(I2C_VL53L0X_INST, addr>>1, DL_I2C_CONTROLLER_DIRECTION_TX, gTxLen);

    while ((gI2cControllerStatus != I2C_STATUS_TX_COMPLETE) && (gI2cControllerStatus != I2C_STATUS_ERROR));

    while (DL_I2C_getControllerStatus(I2C_VL53L0X_INST) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS);

    while (!(DL_I2C_getControllerStatus(I2C_VL53L0X_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));
    delay_cycles(1000);

    return 0;
}

//IIC读n字节数据
u8 VL_IIC_Read_nByte(u8 addr, u8 regaddr,u16 num,u8 *Read)
{
    uint8_t data[2], i;
    data[0] = regaddr;

    gI2cControllerStatus = I2C_STATUS_IDLE;
    DL_I2C_fillControllerTXFIFO(I2C_VL53L0X_INST, &data[0], 1);
    DL_I2C_disableInterrupt(I2C_VL53L0X_INST, DL_I2C_INTERRUPT_CONTROLLER_TXFIFO_TRIGGER);
    gI2cControllerStatus = I2C_STATUS_TX_STARTED;
    while (!(DL_I2C_getControllerStatus(I2C_VL53L0X_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));
    DL_I2C_startControllerTransfer(I2C_VL53L0X_INST, addr>>1, DL_I2C_CONTROLLER_DIRECTION_TX, 1);
    while ((gI2cControllerStatus != I2C_STATUS_TX_COMPLETE) && (gI2cControllerStatus != I2C_STATUS_ERROR));
    while (DL_I2C_getControllerStatus(I2C_VL53L0X_INST) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS);

    while (!(DL_I2C_getControllerStatus(I2C_VL53L0X_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));
    // delay_cycles(1000);

    gRxLen = num;
    gRxCount = 0;
    gI2cControllerStatus = I2C_STATUS_RX_STARTED;
    DL_I2C_startControllerTransfer(I2C_VL53L0X_INST, addr>>1, DL_I2C_CONTROLLER_DIRECTION_RX, gRxLen);
    while (gI2cControllerStatus != I2C_STATUS_RX_COMPLETE);
    while (DL_I2C_getControllerStatus(I2C_VL53L0X_INST) &DL_I2C_CONTROLLER_STATUS_BUSY_BUS);

    for(i=0; i<num; i++)
    {
        Read[i] = gRxPacket[i];
    }

    return 0;
}

//VL53L0X 写多个数据
//address:地址
//index:偏移地址
//pdata:数据指针
//count:长度 最大65535
u8 VL53L0X_write_multi(u8 address, u8 index,u8 *pdata,u16 count)
{
	u8 status = STATUS_OK;

	if(VL_IIC_Write_nByte(address,index,count,pdata))
	{
	   status  = STATUS_FAIL;
	}
	return status;
}


//VL53L0X 读多个数据
//address:地址
//index:偏移地址
//pdata:数据指针
//count:长度 最大65535
u8 VL53L0X_read_multi(u8 address,u8 index,u8 *pdata,u16 count)
{
	u8 status = STATUS_OK;

	if(VL_IIC_Read_nByte(address,index,count,pdata))
	{
	  status  = STATUS_FAIL;
	}

	return status;
}

//VL53L0X 写1个数据(单字节)
//address:地址
//index:偏移地址
//data:数据(8位)
u8 VL53L0X_write_byte(u8 address,u8 index,u8 data)
{
	u8 status = STATUS_OK;

	status = VL53L0X_write_multi(address,index,&data,1);

	return status;
}

//VL53L0X 写1个数据(双字节)
//address:地址
//index:偏移地址
//data:数据(16位)
u8 VL53L0X_write_word(u8 address,u8 index,u16 data)
{
	u8 status = STATUS_OK;
	
	u8 buffer[2];
	
	//将16位数据拆分成8位
	buffer[0] = (u8)(data>>8);//高八位
	buffer[1] = (u8)(data&0xff);//低八位
	
	if(index%2==1)
	{  
		//串行通信不能处理对非2字节对齐寄存器的字节
		status = VL53L0X_write_multi(address,index,&buffer[0],1);
		status = VL53L0X_write_multi(address,index,&buffer[0],1);
	}else
	{
		status = VL53L0X_write_multi(address,index,buffer,2);
	}
	
	return status;
}

//VL53L0X 写1个数据(四字节)
//address:地址
//index:偏移地址
//data:数据(32位)
u8 VL53L0X_write_dword(u8 address,u8 index,u32 data)
{
	
    u8 status = STATUS_OK;

    u8 buffer[4];	
	
	//将32位数据拆分成8位
	buffer[0] = (u8)(data>>24);
	buffer[1] = (u8)((data&0xff0000)>>16);
	buffer[2] = (u8)((data&0xff00)>>8);
	buffer[3] = (u8)(data&0xff);
	
	status = VL53L0X_write_multi(address,index,buffer,4);
	
	return status;
	
}


//VL53L0X 读1个数据(单字节)
//address:地址
//index:偏移地址
//data:数据(8位)
u8 VL53L0X_read_byte(u8 address,u8 index,u8 *pdata)
{
	u8 status = STATUS_OK;
	 
	status = VL53L0X_read_multi(address,index,pdata,1);
	
	return status;
	 
}

//VL53L0X 读个数据(双字节)
//address:地址
//index:偏移地址
//data:数据(16位)
u8 VL53L0X_read_word(u8 address,u8 index,u16 *pdata)
{
	u8 status = STATUS_OK;
	
	u8 buffer[2];
	
	status = VL53L0X_read_multi(address,index,buffer,2);
	
	*pdata = ((u16)buffer[0]<<8)+(u16)buffer[1];
	
	return status;
	
}

//VL53L0X 读1个数据(四字节)
//address:地址
//index:偏移地址
//data:数据(32位)
u8 VL53L0X_read_dword(u8 address,u8 index,u32 *pdata)
{
	u8 status = STATUS_OK;
	
	u8 buffer[4];
	
	status = VL53L0X_read_multi(address,index,buffer,4);
	
	*pdata = ((u32)buffer[0]<<24)+((u32)buffer[1]<<16)+((u32)buffer[2]<<8)+((u32)buffer[3]);
	
	return status;
	
}

void I2C_VL53L0X_INST_IRQHandler(void)
{
    switch (DL_I2C_getPendingInterrupt(I2C_VL53L0X_INST)) {
        case DL_I2C_IIDX_CONTROLLER_RX_DONE:
            gI2cControllerStatus = I2C_STATUS_RX_COMPLETE;
            break;
        case DL_I2C_IIDX_CONTROLLER_TX_DONE:
            DL_I2C_disableInterrupt(
                I2C_VL53L0X_INST, DL_I2C_INTERRUPT_CONTROLLER_TXFIFO_TRIGGER);
            gI2cControllerStatus = I2C_STATUS_TX_COMPLETE;
            break;
        case DL_I2C_IIDX_CONTROLLER_RXFIFO_TRIGGER:
            gI2cControllerStatus = I2C_STATUS_RX_INPROGRESS;
            /* Receive all bytes from target */
            while (DL_I2C_isControllerRXFIFOEmpty(I2C_VL53L0X_INST) != true) {
                if (gRxCount < gRxLen) {
                    gRxPacket[gRxCount++] =
                        DL_I2C_receiveControllerData(I2C_VL53L0X_INST);
                } else {
                    /* Ignore and remove from FIFO if the buffer is full */
                    DL_I2C_receiveControllerData(I2C_VL53L0X_INST);
                }
            }
            break;
        case DL_I2C_IIDX_CONTROLLER_TXFIFO_TRIGGER:
            gI2cControllerStatus = I2C_STATUS_TX_INPROGRESS;
            /* Fill TX FIFO with next bytes to send */
            if (gTxCount < gTxLen) {
                gTxCount += DL_I2C_fillControllerTXFIFO(
                    I2C_VL53L0X_INST, &gTxPacket[gTxCount], gTxLen - gTxCount);
            }
            break;
            /* Not used for this example */
        case DL_I2C_IIDX_CONTROLLER_ARBITRATION_LOST:
        case DL_I2C_IIDX_CONTROLLER_NACK:
            if ((gI2cControllerStatus == I2C_STATUS_RX_STARTED) ||
                (gI2cControllerStatus == I2C_STATUS_TX_STARTED)) {
                /* NACK interrupt if I2C Target is disconnected */
                gI2cControllerStatus = I2C_STATUS_ERROR;
            }
        case DL_I2C_IIDX_CONTROLLER_RXFIFO_FULL:
        case DL_I2C_IIDX_CONTROLLER_TXFIFO_EMPTY:
        case DL_I2C_IIDX_CONTROLLER_START:
        case DL_I2C_IIDX_CONTROLLER_STOP:
        case DL_I2C_IIDX_CONTROLLER_EVENT1_DMA_DONE:
        case DL_I2C_IIDX_CONTROLLER_EVENT2_DMA_DONE:
        default:
            break;
    }
}