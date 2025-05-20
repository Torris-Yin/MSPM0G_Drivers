#include "AD9954.h"
#include "m0_helper.h"

/* 
    Ported by DCZYewen Github: https://github.com/DCZYewen
*/

/*  M0 helper IO51 class provided by m0_helper.h*/
IO51 AD9954_CS(IO0_PORT , IO0);
IO51 AD9954_SCLK(IO1_PORT , IO1);
IO51 AD9954_SDIO(IO2_PORT , IO2);
IO51 AD9954_OSK(IO3_PORT , IO3);
IO51 PS1(IO4_PORT , IO4);
IO51 PS0(IO5_PORT , IO5);
IO51 IOUPDATE(IO6_PORT , IO6);
IO51 AD9954_IOSY(IO7_PORT , IO7);
IO51 AD9954_RES(IO8_PORT , IO8);
IO51 AD9954_PWR(IO9_PORT , IO9);

// the only input pin
IO51 AD9954_SDO(IO10_PORT , IO10);

//系统频率fosc（外部晶振频率），系统频率=fs
#define fosc  20                        //晶振频率 20Mhz
#define PLL_MULTIPLIER  20              //PLL倍频数（4--20）
#define fs  (fosc*PLL_MULTIPLIER)       //系统时钟频率

double fH_Num=10.73741824;
//double fH_Num=11.2204;
//double fH_Num=11.3671588397205;//
//double fH_Num = 11.3025455157895;//频率转换系数：2^32/系统时钟频率

extern "C"{

/************************************************************
** 函数名称 ：void AD9954_GPIO_Init(void)  
** 函数功能 ：初始化控制AD9954需要用到的IO口
** 入口参数 ：无
** 出口参数 ：无
** 函数说明 ：无
**************************************************************/
void AD9954_GPIO_Init(void)
{
/* 	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);					 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_2|GPIO_Pin_7|GPIO_Pin_6;			
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_1|GPIO_Pin_0;				
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	    		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 */
	AD9954_RES=0;
	AD9954_CS =0;
	AD9954_SCLK =0;
	AD9954_SDIO=0;
	AD9954_OSK=0;
	PS0=0;
	PS1=0;
	IOUPDATE=0;
	AD9954_IOSY=0;
	AD9954_PWR=0;
	delay_ms(10);

}

/*********************************************************************************************************
** 函数名称 ：void AD9954_RESET(void)
** 函数功能 ：复位AD9954
** 入口参数 ：无
** 出口参数 ：无
** 函数说明 ：不复位也可以
*********************************************************************************************************/
void AD9954_RESET(void)
{
	AD9954_RES = 1;
	delay_ms(10);
	AD9954_RES = 0;
	AD9954_CS = 0;
	AD9954_SCLK = 0;
	AD9954_CS = 1;
}


/*********************************************************************************************************
** 函数名称 ：void UPDATE(void)
** 函数功能 ：产生一个更新信号，更新AD9954内部寄存器，
** 入口参数 ：无
** 出口参数 ：无
** 函数说明 ：可以不加任何延时
*********************************************************************************************************/
void UPDATE(void)
{ 
	IOUPDATE = 1;
	delay_us(10);
	IOUPDATE = 0;
}

/*********************************************************************************************************
** 函数名称 ：void AD9954_Send_Byte(uint8_t dat)
** 函数功能 ：向AD9954发送一个字节的内容
** 入口参数 ：待发送字节
** 出口参数 ：无
** 函数说明 ：AD9954的传输速度最大为25M，所以不加延时也可以
*********************************************************************************************************/
void AD9954_Send_Byte(uint8_t dat)
{
	uint8_t i;
	for (i = 0;i<8;i++)
	{
		AD9954_SCLK = 0;
		delay_us(10);
		if (dat & 0x80)
		{
			AD9954_SDIO = 1;
		}
		else
		{
			AD9954_SDIO = 0;
		}
		AD9954_SCLK = 1;
		delay_us(10);
		dat <<= 1;
	}
}

/*********************************************************************************************************
** 函数名称 ：uint8_t AD9954_Read_Byte(void)
** 函数功能 ：读AD9954一个字节的内容
** 入口参数 ：无
** 出口参数 ：读出的一个字节数据
** 函数说明 ：
*********************************************************************************************************/
uint8_t AD9954_Read_Byte(void)
{
	uint8_t i,dat=0;
	for (i = 0;i<8;i++)
	{
		AD9954_SCLK = 0;
		delay_us(2);
		dat|=AD9954_SDO;
		AD9954_SCLK = 1;
		delay_us(2);
		dat <<= 1;
	}
	return dat;
}

/************************************************************
** 函数名称 ：void AD9954_Write_nByte(uint8_t RegAddr,uint8_t *Data,uint8_t Len)
** 函数功能 ：向AD9954指定的寄存器写数据
** 入口参数 ：RegAddr: 寄存器地址
						*Data: 数据起始地址
						Len: 要写入的字节数
** 出口参数 ：无
** 函数说明 ：无
**************************************************************/
void AD9954_Write_nByte(uint8_t RegAddr,uint8_t *Data,uint8_t Len)
{  	
	uint8_t t=0;
	
	AD9954_Send_Byte(RegAddr);
	for(t=0;t<Len;t++)
	{
		AD9954_Send_Byte(Data[t]);
	}												    
}
/*********************************************************************************************************
** 函数名称 ：uint32_t AD9954_Read_nByte(uint8_t ReadAddr,uint8_t Len)
** 函数功能 ：读AD9954寄存器数据
** 入口参数 ：ReadAddr:要读出的寄存器地址
							Len:要读出数据的长度1-4
** 出口参数 ：读出的数据
** 函数说明 ：
*********************************************************************************************************/
uint32_t AD9954_Read_nByte(uint8_t ReadAddr,uint8_t Len)
{
	uint8_t t=0;
	uint32_t temp=0;
	AD9954_CS=1;
	AD9954_Send_Byte(ReadAddr);
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AD9954_Read_Byte();
	}
	AD9954_CS=1;
	return temp;
}

/************************************************************
** 函数名称 ：uint32_t Get_FTW(double Real_fH)
** 函数功能 ：频率数据转换
** 入口参数 ：Freq，需要转换的频率，0-140000000hz
** 出口参数 ：频率数据值
** 函数说明 ：
**************************************************************/
uint32_t Get_FTW(double Real_fH)
{
		return (uint32_t)(fH_Num*Real_fH);
}

/*********************************************************************************************************
** 函数名称 ：void AD9954_Init(void))
** 函数功能 ：初始化AD9954的管脚和最简单的内部寄存器的配置，
** 入口参数 ：无
** 出口参数 ：无
** 函数说明 ：板上的晶振为20MHz，最大采用了20倍频，为400M
*********************************************************************************************************/
void AD9954_Init(void)
{
	uint8_t CFR1_data[4]={0,0,0,0};
	uint8_t CFR2_data[3]={0,0,0};

	AD9954_GPIO_Init();
	AD9954_RESET();
	delay_ms(300);
	AD9954_CS = 0;
	
	CFR1_data[0]=0X02;//此处：0x02->OSK使能；0X00->OSK关闭。在OSK模式使能的前提下，幅度寄存器(0X02)生效
										//见英文数据手册，page 31 ：Amplitude Scale Factor (ASF)部分
										//In manual OSK
										//mode, ASF<15:14> has no effect. ASF<13:0> provide the output
										//scale factor directly. If the OSK is disabled using CFR1<25>,
										//this register has no effect on device operation.
	CFR1_data[1]=0X00;
	CFR1_data[2]=0X00;
	CFR1_data[3]=0x00;//比较器启用,方波输出;0x40,比较器禁用方波无输出
	AD9954_Write_nByte(CFR1,CFR1_data,4);//数据写入控制功能寄存器1

	CFR2_data[0]=0X00;
	CFR2_data[1]=0X00;
	if(fs>400)
		;//系统频率超过芯片最大值
	else if(fs>=250)
		CFR2_data[2]=PLL_MULTIPLIER<<3|0x04|0X03;
	else CFR2_data[2]=PLL_MULTIPLIER<<3;
	AD9954_Write_nByte(CFR2,CFR2_data,3);//数据写入控制功能寄存器2
	
	AD9954_CS=1;
}


/*********************************************************************************************************
** 函数名称 ：void AD9954_Set_Fre(double fre)
** 函数功能 ：设置AD9954当前的频率输出，采用的是单一频率输出
** 入口参数 ：fre:欲设置的频率值 0-140000000hz
** 出口参数 ：无
** 函数说明 ：因为采用的浮点数进行计算，转换过程中会出现误差，通过调整可以精确到0.1Hz以内
*********************************************************************************************************/
void AD9954_Set_Fre(double fre)//single tone
{
	uint8_t date[4] ={0x00,0x00,0x00,0x00};	//中间变量
	uint32_t Temp=0;   
	
	Temp=Get_FTW(fre);
	date[0] =(uint8_t)(Temp >> 24);
	date[1] =(uint8_t)(Temp >> 16);
	date[2] =(uint8_t)(Temp >> 8);
	date[3] =(uint8_t)Temp;
	
	AD9954_CS = 0;
	AD9954_Write_nByte(FTW0,date,4);//写频率控制字
	AD9954_CS=1;
	UPDATE();
}

/*********************************************************************************************************
** 函数名称 ：void AD9954_Set_Amp(uint16_t Ampli)
** 函数功能 ：设置AD9954输出幅度
** 入口参数 ：Ampli：0-16383，最大峰峰值约500mv
** 出口参数 ：无
** 函数说明 ：
*********************************************************************************************************/
void AD9954_Set_Amp(uint16_t Ampli)
{
	uint8_t date[2] ={0x00,0x00};	
	
	AD9954_CS = 0;
	date[0]=(uint8_t)(Ampli >> 8);
	date[1]=(uint8_t)Ampli;
	AD9954_Write_nByte(ASF,date,2);
	AD9954_CS = 1;
	UPDATE();
}

/************************************************************
** 函数名称 ：void AD9954_Set_Phase(uint8_t Channel,uint16_t Phase)
** 函数功能 ：设置通道的输出相位
** 入口参数 ：Phase:	输出相位,范围：0~16383(对应角度：0°~360°)
** 出口参数 ：无
** 函数说明 ：无
**************************************************************/
void AD9954_Set_Phase(uint16_t Phase)//写相位
{
	uint8_t date[2] ={0x00,0x00};	
	
	AD9954_CS = 0;
	date[0]=(uint8_t)(Phase >> 8);
	date[1]=(uint8_t)Phase;
	AD9954_Write_nByte(POW0,date,2);
	AD9954_CS = 1;
	UPDATE();
}



/*********************************************************************************************************
** 函数名称 ：void AD9954_SetFSK(double f1,double f2,double f3,double f4,uint16_t Ampli)   
** 函数功能 ：四相FSK信号输出参数设置
** 入口参数 ：f1:频率1  0-140000000hz
** 	      		f2:频率2
** 	      		f3:频率3
** 	      		f4:频率4
**					 Ampli幅度：0-16383，最大峰峰值约500mv
** 出口参数 ：无
** 隐含控制   			PS0: 0   1	0	 1
** 管脚参数： 			PS1: 0   0	1	 1
** 对应控制 RAM段：		 0   1	2	 3
** 函数说明 ：在四个RAM区各设置了一个频率值，通过改变PS0和PS1的电平选择对应的RAM端输出相应的频率值来实现FSK，也可以实现二项的FSK；
**            通过设置控制PS0,PS1管脚的电平就可以将二进制的编码转化为FSK信号输出
*********************************************************************************************************/
void AD9954_SetFSK(double f1,double f2,double f3,double f4,uint16_t Ampli)   
{
	uint32_t FTW_Vau=0;
	uint8_t fdata[4]={0,0,0,0};	
	uint8_t CFR1_data[4]={0,0,0,0};
	uint8_t data[5]={0,0,0,0,0};
	
	AD9954_CS = 0;

	data[0]=0x00;	//低8位 斜率=0x0000  
	data[1]=0x00;	//高8位 斜率=0x0000
	data[2]=0x00;	//终止地址:0x0000   低8位
	data[3]=0x00;	//0:1终止地址:高2位，2:7起始地址低6位:0x000;   
	data[4]=0x00;	//0:3起始地址高4位  5:7，RAM0工作于模式0;  4：不停留位没有激活  
	AD9954_Write_nByte(RSCW0,data,5);
		
	data[0]=0x00;	//低8位 斜率=0x0000  
	data[1]=0x00;	//高8位 斜率=0x0000
	data[2]=0x01;	//终止地址:0x0001   低8位
	data[3]=0x04;	//0:1终止地址:高2位，2:7起始地址低6位:0x0001;   
	data[4]=0x00;	//0:3起始地址高4位  5:7，RAM1工作于模式0;  4：不停留位没有激活  
	AD9954_Write_nByte(RSCW1,data,5);
	
	data[0]=0x00;	//低8位 斜率=0x0000  
	data[1]=0x00;	//高8位 斜率=0x0000
	data[2]=0x02;	//终止地址:0x0002   低8位
	data[3]=0x08;	//0:1终止地址:高2位，2:7起始地址低6位:0x0002;   
	data[4]=0x00;	//0:3起始地址高4位  5:7，RAM2工作于模式0;  4：不停留位没有激活  
	AD9954_Write_nByte(RSCW2,data,5);
	
	data[0]=0x00;	//低8位 斜率=0x0000  
	data[1]=0x00;	//高8位 斜率=0x0000
	data[2]=0x03;	//终止地址:0x0003   低8位
	data[3]=0x0c;	//0:1终止地址:高2位，2:7起始地址低6位:0x0003;   
	data[4]=0x00;	//0:3起始地址高4位  5:7，RAM3工作于模式0;  4：不停留位没有激活  
	AD9954_Write_nByte(RSCW3,data,5);    	
	
	AD9954_CS = 1;
	UPDATE();
	
	AD9954_CS = 0;
	PS1=0;PS0=0;
	FTW_Vau=Get_FTW(f1); 
  fdata[0]=FTW_Vau>>24;
  fdata[1]=FTW_Vau>>16;
  fdata[2]=FTW_Vau>>8;
  fdata[3]=FTW_Vau;
	AD9954_Write_nByte(RAM,fdata,4);    	

	
	PS1=0;PS0=1;
	FTW_Vau=Get_FTW(f2); 
  fdata[0]=FTW_Vau>>24;
  fdata[1]=FTW_Vau>>16;
  fdata[2]=FTW_Vau>>8;
  fdata[3]=FTW_Vau;
	AD9954_Write_nByte(RAM,fdata,4);  
	
	PS1=1;PS0=0;
	FTW_Vau=Get_FTW(f3); 
  fdata[0]=FTW_Vau>>24;
  fdata[1]=FTW_Vau>>16;
  fdata[2]=FTW_Vau>>8;
  fdata[3]=FTW_Vau;
	AD9954_Write_nByte(RAM,fdata,4);  
	
	PS1=1;PS0=1;
	FTW_Vau=Get_FTW(f4); 
  fdata[0]=FTW_Vau>>24;
  fdata[1]=FTW_Vau>>16;
  fdata[2]=FTW_Vau>>8;
  fdata[3]=FTW_Vau;
	AD9954_Write_nByte(RAM,fdata,4);  
	
	AD9954_CS = 1;
	UPDATE();

	AD9954_CS = 0;
	CFR1_data[0]=0X82;//打开RAM控制位驱动FTW
	CFR1_data[1]=0X00;
	CFR1_data[2]=0X00;
	CFR1_data[3]=0x00;//比较器启用,方波输出;0x40,比较器禁用方波无输出
	AD9954_Write_nByte(CFR1,CFR1_data,4);//数据写入控制功能寄存器1
	AD9954_CS = 1;
	UPDATE();

	AD9954_Set_Amp(Ampli);//设置幅度

}   

/*********************************************************************************************************
** 函数名称 ：void AD9954_SetPSK(uint16_t Phase1,uint16_t Phase2,uint16_t Phase3,uint16_t Phase4,double fre,uint16_t Ampli)
** 函数功能 ：四相PSK信号输出参数设置
** 入口参数 ：Phase1:相位1 范围：0-16383 对应0-360度
** 	      		Phase2:相位2
** 	      		Phase3:相位3
** 	      		Phase4:相位4
** 						fre：频率    0-140000000hz
**					 Ampli幅度：0-16383，最大峰峰值约500mv
** 出口参数 ：无
** 隐含控制   			PS0: 0   1	0	 1
** 管脚参数： 			PS1: 0   0	1	 1
** 对应控制 RAM段：		 0   1	2	 3
** 函数说明 ：在四个RAM区各设置了一个频率值，通过改变PS0和PS1的电平选择对应的RAM端输出相应的频率值来实现PSK，也可以实现二项的PSK；
**            通过设置控制PS0,PS1管脚的电平就可以将二进制的编码转化为PSK信号输出
*********************************************************************************************************/
void AD9954_SetPSK(uint16_t Phase1,uint16_t Phase2,uint16_t Phase3,uint16_t Phase4,double fre,uint16_t Ampli)
{
	uint8_t pdata[4]={0,0,0,0};	
	uint8_t CFR1_data[4]={0,0,0,0};

	uint8_t data[5]={0,0,0,0,0};
	
	AD9954_CS = 0;

	data[0]=0x00;	//低8位 斜率=0x0000  
	data[1]=0x00;	//高8位 斜率=0x0000
	data[2]=0x00;	//终止地址:0x0000   低8位
	data[3]=0x00;	//0:1终止地址:高2位，2:7起始地址低6位:0x000;   
	data[4]=0x00;	//0:3起始地址高4位  5:7，RAM0工作于模式0;  4：不停留位没有激活  
	AD9954_Write_nByte(RSCW0,data,5);
		
	data[0]=0x00;	//低8位 斜率=0x0000  
	data[1]=0x00;	//高8位 斜率=0x0000
	data[2]=0x01;	//终止地址:0x0001   低8位
	data[3]=0x04;	//0:1终止地址:高2位，2:7起始地址低6位:0x0001;   
	data[4]=0x00;	//0:3起始地址高4位  5:7，RAM1工作于模式0;  4：不停留位没有激活  
	AD9954_Write_nByte(RSCW1,data,5);
	
	data[0]=0x00;	//低8位 斜率=0x0000  
	data[1]=0x00;	//高8位 斜率=0x0000
	data[2]=0x02;	//终止地址:0x0002   低8位
	data[3]=0x08;	//0:1终止地址:高2位，2:7起始地址低6位:0x0002;   
	data[4]=0x00;	//0:3起始地址高4位  5:7，RAM2工作于模式0;  4：不停留位没有激活  
	AD9954_Write_nByte(RSCW2,data,5);
	
	data[0]=0x00;	//低8位 斜率=0x0000  
	data[1]=0x00;	//高8位 斜率=0x0000
	data[2]=0x03;	//终止地址:0x0003   低8位
	data[3]=0x0c;	//0:1终止地址:高2位，2:7起始地址低6位:0x0003;   
	data[4]=0x00;	//0:3起始地址高4位  5:7，RAM3工作于模式0;  4：不停留位没有激活  
	AD9954_Write_nByte(RSCW3,data,5);    	
	
	AD9954_CS = 1;
	UPDATE();
	
	Phase1<<=2;
	Phase2<<=2;
	Phase3<<=2;
	Phase4<<=2;
	
	AD9954_CS = 0;
	PS1=0;PS0=0;
  pdata[0]=(uint8_t)(Phase1>>8);
  pdata[1]=(uint8_t)Phase1;
	pdata[2]=0x00;
  pdata[3]=0x00;
	AD9954_Write_nByte(RAM,pdata,4);    	

	PS1=0;PS0=1;
  pdata[0]=(uint8_t)(Phase2>>8);
  pdata[1]=(uint8_t)Phase2;
	pdata[2]=0x00;
  pdata[3]=0x00;
	AD9954_Write_nByte(RAM,pdata,4);  
	
	PS1=1;PS0=0;
  pdata[0]=(uint8_t)(Phase3>>8);
  pdata[1]=(uint8_t)Phase3;
	pdata[2]=0x00;
  pdata[3]=0x00;
	AD9954_Write_nByte(RAM,pdata,4);  
	
	PS1=1;PS0=1;
  pdata[0]=(uint8_t)(Phase4>>8);
  pdata[1]=(uint8_t)Phase4;
	pdata[2]=0x00;
  pdata[3]=0x00;
	AD9954_Write_nByte(RAM,pdata,4);  
	
	AD9954_CS = 1;
	UPDATE();

	AD9954_CS = 0;
	CFR1_data[0]=0Xc2;//打开RAM控制位驱动POW相位
	CFR1_data[1]=0X00;
	CFR1_data[2]=0X00;
	CFR1_data[3]=0x00;//比较器启用,方波输出;0x40,比较器禁用方波无输出
	AD9954_Write_nByte(CFR1,CFR1_data,4);//数据写入控制功能寄存器1
	AD9954_CS = 1;
	UPDATE();
	AD9954_Set_Amp(Ampli);//设置幅度
	AD9954_Set_Fre(fre);
}

/*********************************************************************************************************
** 函数名称 ：void AD9954_Set_LinearSweep(double Freq_Low,double Freq_High,double  UpStepFreq, 
**																			uint8_t UpStepTime,double	DownStepFreq, uint8_t DownStepTime,uint8_t mode)
** 函数功能 ：线性扫描输出模式
** 函数说明 ：使频率按预置的模式线性扫描上去，详细参见官方PDF
** 入口参数 ：Freq_Low:起始频率 0-140000000hz
** 			  		Freq_High:终止频率; 0-140000000hz
**						UpStepFreq：向上扫频步进， 0-140000000hz
**						UpStepTime：向上扫频的步进间隔时间;1-255
**            DownStepFreq：向下扫频步进， 0-140000000hz
**						DownStepTime：向下扫频的步进间隔时间;1-255
**						mode:线性扫描无停留功能,No_Dwell不停留，输出扫频到终止频率回到起始频率；Dwell停留，输出扫频到终止频率后保持在终止频率。
** 出口参数 ：无
** 函数说明 ：需要保证，Freq_Low<Freq_High
**						步进间隔时间 T = StepTime*10 ；例：UpStepTime=100，则向上扫频的步进间隔时间T=1000nS=1us
**						扫频总时间=总扫描频点数*T
**						PS0脚控制扫频方向，PS0=1向上扫频，PS0=0向下扫频
*********************************************************************************************************/
void AD9954_Set_LinearSweep(double Freq_Low,double Freq_High,double  UpStepFreq, uint8_t UpStepTime,double	DownStepFreq, uint8_t DownStepTime,uint8_t mode)//linear sweep mode
{	
	uint8_t date[5] ={0x00,0x00,0x00,0x00,0x00};	//中间变量
	uint8_t CFR1_data[4] ={0x00,0x00,0x00,0x00};
	uint32_t Temp=0; 
	
	PS1=0;PS0=0;
	AD9954_CS=0;
	
	CFR1_data[0]=0x00;//31-24
	CFR1_data[1]=0x20;//23-16
	CFR1_data[2]=0x00;
	CFR1_data[3]=0x00|mode;//比较器启用,方波输出;0x40,比较器禁用方波无输出
	AD9954_Write_nByte(CFR1,CFR1_data,4);//数据写入控制功能寄存器1
	
	Temp=Get_FTW(Freq_Low);
	date[0] =(uint8_t)(Temp >> 24);
	date[1] =(uint8_t)(Temp >> 16);
	date[2] =(uint8_t)(Temp >> 8);
	date[3] =(uint8_t)Temp;
	AD9954_Write_nByte(FTW0,date,4);//写频率控制字
	
	Temp=Get_FTW(Freq_High);
	date[0] =(uint8_t)(Temp >> 24);
	date[1] =(uint8_t)(Temp >> 16);
	date[2] =(uint8_t)(Temp >> 8);
	date[3] =(uint8_t)Temp;
	AD9954_Write_nByte(FTW1,date,4);//写频率控制字
	
	Temp=Get_FTW(DownStepFreq);
	date[0] =DownStepTime;//下降的扫描斜率
	date[1] =(uint8_t)(Temp >> 24);
	date[2] =(uint8_t)(Temp >> 16);
	date[3] =(uint8_t)(Temp >> 8);
	date[4] =(uint8_t)Temp;//下降的频率增量
	AD9954_Write_nByte(NLSCW,date,5);
	
	Temp=Get_FTW(UpStepFreq);
	date[0] =UpStepTime;//上升的扫描斜率
	date[1] =(uint8_t)(Temp >> 24);
	date[2] =(uint8_t)(Temp >> 16);
	date[3] =(uint8_t)(Temp >> 8);
	date[4] =(uint8_t)Temp;//上升的频率增量
	AD9954_Write_nByte(PLSCW,date,5);
	
	AD9954_CS=1;
	UPDATE();
}

}
