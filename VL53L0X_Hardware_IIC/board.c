/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 立创论坛：https://oshwhub.com/forum
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 * Change Logs:
 * Date           Author       Notes
 * 2024-05-23     LCKFB     first version
 */
#include "board.h"
#include "stdio.h"
#include "string.h"

volatile unsigned int delay_times = 0;
volatile unsigned char uart_data = 0;

void board_init(void)
{
	// SYSCFG初始化
	SYSCFG_DL_init();
	//清除串口中断标志
	NVIC_ClearPendingIRQ(UART_0_INST_INT_IRQN);
	//使能串口中断
	NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
    NVIC_EnableIRQ(I2C_VL53L0X_INST_INT_IRQN);
}

//搭配滴答定时器实现的精确us延时
void delay_us(unsigned long us) 
{
    while(us--)
        delay_cycles(CPUCLK_FREQ/1000000);
}
//搭配滴答定时器实现的精确ms延时
void delay_ms(unsigned long ms) 
{
    while(ms--)
	    delay_cycles(CPUCLK_FREQ/1000);
}

void delay_1us(unsigned long __us){ delay_us(__us); }
void delay_1ms(unsigned long ms){ delay_ms(ms); }

//串口发送单个字符
void uart0_send_char(char ch)
{
	//当串口0忙的时候等待，不忙的时候再发送传进来的字符
	while( DL_UART_isBusy(UART_0_INST) == true );
	//发送单个字符
	DL_UART_Main_transmitData(UART_0_INST, ch);

}
//串口发送字符串
void uart0_send_string(char* str)
{
	//当前字符串地址不在结尾 并且 字符串首地址不为空
	while(*str!=0&&str!=0)
	{
		//发送字符串首地址中的字符，并且在发送完成之后首地址自增
		uart0_send_char(*str++);
	}
}

int fputc(int c, FILE* stream)
{
	DL_UART_Main_transmitDataBlocking(UART_0_INST, c);
    return c;
}

int fputs(const char* restrict s, FILE* restrict stream)
{
    uint16_t i, len;
    len = strlen(s);
    for(i=0; i<len; i++)
    {
        DL_UART_Main_transmitDataBlocking(UART_0_INST, s[i]);
    }
    return len;
}

int puts(const char *_ptr)
{
    int count = fputs(_ptr, stdout);
    count += fputs("\n", stdout);
    return count;
}

//串口的中断服务函数
void UART_0_INST_IRQHandler(void)
{
	//如果产生了串口中断
	switch( DL_UART_getPendingInterrupt(UART_0_INST) )
	{
		case DL_UART_IIDX_RX://如果是接收中断
			//接发送过来的数据保存在变量中
			uart_data = DL_UART_Main_receiveData(UART_0_INST);
			//将保存的数据再发送出去
			uart0_send_char(uart_data);
			break;
		
		default://其他的串口中断
			break;
	}
}
