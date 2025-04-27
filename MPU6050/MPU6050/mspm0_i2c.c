#include "ti_msp_dl_config.h"
#include "mspm0_clock.h"
#include "mspm0_i2c.h"

int mspm0_i2c_write(unsigned char slave_addr,
                     unsigned char reg_addr,
                     unsigned char length,
                     unsigned char const *data)
{
    unsigned int cnt = length;
    unsigned char const *ptr = data;

    if (!length)
        return 0;

    DL_I2C_transmitControllerData(I2C_MPU6050_INST, reg_addr);
    DL_I2C_clearInterruptStatus(I2C_MPU6050_INST, DL_I2C_INTERRUPT_CONTROLLER_TX_DONE);
    DL_I2C_startControllerTransfer(I2C_MPU6050_INST, slave_addr, DL_I2C_CONTROLLER_DIRECTION_TX, length+1);

    do {
        unsigned fillcnt;
        fillcnt = DL_I2C_fillControllerTXFIFO(I2C_MPU6050_INST, ptr, cnt);
        cnt -= fillcnt;
        ptr += fillcnt;
    } while (!DL_I2C_getRawInterruptStatus(I2C_MPU6050_INST, DL_I2C_INTERRUPT_CONTROLLER_TX_DONE));

    while (DL_I2C_getControllerStatus(I2C_MPU6050_INST) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS);
    DL_I2C_flushControllerTXFIFO(I2C_MPU6050_INST);

    return 0;
}

int mspm0_i2c_read(unsigned char slave_addr,
                    unsigned char reg_addr,
                    unsigned char length,
                    unsigned char *data)
{
    unsigned i;
    if (!length)
        return 0;

    DL_I2C_transmitControllerData(I2C_MPU6050_INST, reg_addr);
    I2C_MPU6050_INST->MASTER.MCTR = I2C_MCTR_RD_ON_TXEMPTY_ENABLE;
    DL_I2C_clearInterruptStatus(I2C_MPU6050_INST, DL_I2C_INTERRUPT_CONTROLLER_RX_DONE);
    DL_I2C_startControllerTransfer(I2C_MPU6050_INST, slave_addr, DL_I2C_CONTROLLER_DIRECTION_RX, length);

    i = 0;
    do {
        if (!DL_I2C_isControllerRXFIFOEmpty(I2C_MPU6050_INST))
        {
            uint8_t c;
            c = DL_I2C_receiveControllerData(I2C_MPU6050_INST);
            if (i < length)
            {
                data[i] = c;
                ++i;
            }
        }
    } while(!DL_I2C_getRawInterruptStatus(I2C_MPU6050_INST, DL_I2C_INTERRUPT_CONTROLLER_RX_DONE));

    I2C_MPU6050_INST->MASTER.MCTR = 0*I2C_MCTR_RD_ON_TXEMPTY_ENABLE;
    DL_I2C_flushControllerTXFIFO(I2C_MPU6050_INST);
    
    return 0;
}