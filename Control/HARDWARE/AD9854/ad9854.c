#include "AD9854.h"    
#include "delay.h"
/*
接口说明


*/




uchar FreqWord[6];				 //6个字节频率控制字

#define CLK_Set 	10
const ulong Freq_mult_ulong = 281474976710656/(CLK_Set*30*1000000);
const double Freq_mult_doulle = 281474976710656/(CLK_Set*30*1000000);

void AD9854_IO_Init(void)
{
	
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  __HAL_RCC_GPIOF_CLK_ENABLE();
	
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_8, GPIO_PIN_RESET);
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);
	
  /*Configure GPIO pins : PF2 PF3 PF4 PF5
                           PF6 PF8 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  /*Configure GPIO pins : PC0 PC1 PC2 PC3
                           PC4 PC5 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PE2 PE3 PE4 PE5
                           PE6 PE7 PE8 PE9
                           PE10 PE11 PE12 PE13
                           PE0 PE1 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}
//====================================================================================
//函数名称:void AD9854_WR_Byte(uchar addr,uchar dat)
//函数功能:AD9854并行口写入数据
//入口参数:addr     6位地址
//         dat      写入的数据
//出口参数:无
//====================================================================================

void AD9854_WR_Byte(u32 addr,u32 dat)
{
	AD9854_AdrBus=addr;
	AD9854_DataBus=dat;
	
	AD9854_WR      = 0;
	AD9854_WR      = 1;	
}

//====================================================================================
//函数名称:void AD9854_Init(void)
//函数功能:AD9854初始化
//入口参数:无
//出口参数:无
//====================================================================================
void AD9854_Init(void)
{
		
		AD9854_IO_Init();
		AD9854_SP = 1;
	
		AD9854_WR=1;//将读、写控制端口设为无效
    AD9854_RD=1;
    AD9854_UDCLK=0;
    AD9854_RST=1;                 //复位AD9854
    AD9854_RST=0;

		AD9854_WR_Byte(0x1d,0x00);	           //关闭比较器
		AD9854_WR_Byte(0x1e,CLK_Set);	   //设置系统时钟倍频            
		AD9854_WR_Byte(0x1f,0x00);	   //设置系统为模式0，由外部更新
		AD9854_WR_Byte(0x20,0x60);	   //设置为可调节幅度，取消插值补偿

		AD9854_UDCLK=1;               //更新AD9854输出
    AD9854_UDCLK=0;
}

//====================================================================================
//函数名称:void Freq_convert(long Freq)
//函数功能:正弦信号频率数据转换
//入口参数:Freq   需要转换的频率，取值从0~SYSCLK/2
//出口参数:无   但是影响全局变量FreqWord[6]的值
//说明：   该算法位多字节相乘算法，有公式FTW = (Desired Output Frequency × 2N)/SYSCLK
//         得到该算法，其中N=48，Desired Output Frequency 为所需要的频率，即Freq，SYSCLK
//         为可编程的系统时钟，FTW为48Bit的频率控制字，即FreqWord[6]
//====================================================================================

void Freq_convert(long Freq)   
{
  ulong FreqBuf;
  ulong Temp=Freq_mult_ulong;   	       

	uchar Array_Freq[4];			     //将输入频率因子分为四个字节
	Array_Freq[0]=(uchar)Freq;
	Array_Freq[1]=(uchar)(Freq>>8);
	Array_Freq[2]=(uchar)(Freq>>16);
	Array_Freq[3]=(uchar)(Freq>>24);

	FreqBuf=Temp*Array_Freq[0];                  
  FreqWord[0]=FreqBuf;    
  FreqBuf>>=8;

  FreqBuf+=(Temp*Array_Freq[1]);
  FreqWord[1]=FreqBuf;
  FreqBuf>>=8;

  FreqBuf+=(Temp*Array_Freq[2]);
  FreqWord[2]=FreqBuf;
	FreqBuf>>=8;

	FreqBuf+=(Temp*Array_Freq[3]);
  FreqWord[3]=FreqBuf;
  FreqBuf>>=8;

  FreqWord[4]=FreqBuf;
  FreqWord[5]=FreqBuf>>8;			
}  

//====================================================================================
//函数名称:void AD9854_SetSine(ulong Freq,uint Shape)
//函数功能:AD9854正弦波产生程序
//入口参数:Freq   频率设置，取值范围为0~(1/2)*SYSCLK
//         Shape  幅度设置. 为12 Bit,取值范围为(0~4095) ,取值越大,幅度越大 
//出口参数:无
//====================================================================================

void AD9854_SetSine(ulong Freq,uint Shape)
{
	uchar count;
	uchar Adress;

	Adress = 0x04;                        //选择频率控制字地址的初值

	Freq_convert(Freq);		           //频率转换

	for(count=6;count>0;)	          //写入6字节的频率控制字  
    {
		AD9854_WR_Byte(Adress++,FreqWord[--count]);
    }
	
	AD9854_WR_Byte(0x21,Shape>>8);	  //设置I通道幅度
	AD9854_WR_Byte(0x22,(uchar)(Shape&0xff));
	
	AD9854_WR_Byte(0x23,Shape>>8);	  //设置Q通道幅度
	AD9854_WR_Byte(0x24,(uchar)(Shape&0xff));

	AD9854_UDCLK=1;                    //更新AD9854输出
  AD9854_UDCLK=0;
}

//====================================================================================
//函数名称:void Freq_doublt_convert(double Freq)
//函数功能:正弦信号频率数据转换
//入口参数:Freq   需要转换的频率，取值从0~SYSCLK/2
//出口参数:无   但是影响全局变量FreqWord[6]的值
//说明：   有公式FTW = (Desired Output Frequency × 2N)/SYSCLK得到该函数，
//         其中N=48，Desired Output Frequency 为所需要的频率，即Freq，SYSCLK
//         为可编程的系统时钟，FTW为48Bit的频率控制字，即FreqWord[6]
//注意：   该函数与上面函数的区别为该函数的入口参数为double，可使信号的频率更精确
//         建议在100HZ以下用本函数，在高于100HZ的情况下用函数void Freq_convert(long Freq)
//====================================================================================
void Freq_double_convert(double Freq)   
{
	ulong Low32;
	uint  High16;
    double Temp=Freq_mult_doulle;   	            //23ca99为2的48次方除以120M
	Freq*=(double)(Temp);
//	1 0000 0000 0000 0000 0000 0000 0000 0000 = 4294967295
	High16 =(int)(Freq/4294967295);                  //2^32 = 4294967295
	Freq -= (double)High16*4294967295;
	Low32 = (ulong)Freq;

    FreqWord[0]=Low32;	     
    FreqWord[1]=Low32>>8;
    FreqWord[2]=Low32>>16;
    FreqWord[3]=Low32>>24;
    FreqWord[4]=High16;
    FreqWord[5]=High16>>8;			
} 
//====================================================================================
//函数名称:void AD9854_SetSine_double(double Freq,uint Shape)
//函数功能:AD9854正弦波产生程序
//入口参数:Freq   频率设置，取值范围为0~1/2*SYSCLK
//         Shape  幅度设置. 为12 Bit,取值范围为(0~4095) 
//出口参数:无
//====================================================================================
void AD9854_SetSine_double(double Freq,uint Shape)
{
	uchar count=0;
	uchar Adress;

	Adress=0x04;						     //选择频率控制字1地址的初值

	Freq_double_convert(Freq);		           //频率转换
	 
	for(count=6;count>0;)	                    //写入6字节的频率控制字  
    {
		AD9854_WR_Byte(Adress++,FreqWord[--count]);
    }
	
	AD9854_WR_Byte(0x21,Shape>>8);	  //设置I通道幅度
	AD9854_WR_Byte(0x22,(uchar)(Shape&0xff));
	
	AD9854_WR_Byte(0x23,Shape>>8);	  //设置Q通道幅度
	AD9854_WR_Byte(0x24,(uchar)(Shape&0xff));

	AD9854_UDCLK=1;                    //更新AD9854输出
  AD9854_UDCLK=0;
}

//====================================================================================
//函数名称:void AD9854_InitFSK(void)
//函数功能:AD9854的FSK初始化
//入口参数:无
//出口参数:无
//====================================================================================
void AD9854_InitFSK(void)
{
		AD9854_IO_Init();
		AD9854_WR=1;                        //将读、写控制端口设为无效
    AD9854_RD=1;
    AD9854_UDCLK=0;
    AD9854_RST=1;                        //复位AD9854
    AD9854_RST=0;	

		AD9854_WR_Byte(0x1d,0x00);	       //关闭比较器
		AD9854_WR_Byte(0x1e,CLK_Set);	   //设置系统时钟倍频
		AD9854_WR_Byte(0x1f,0x02);	       //设置系统为模式1，由外部更新
		AD9854_WR_Byte(0x20,0x60);	      //设置为可调节幅度，取消插值补偿

		AD9854_UDCLK=1;                  //更新AD9854输出
    AD9854_UDCLK=0;
}

//====================================================================================
//函数名称:void AD9854_SetFSK(ulong Freq1,ulong Freq2)
//函数功能:AD9854的FSK设置
//入口参数:Freq1   FSK频率1
//         Freq2   FSK频率2
//出口参数:无
//====================================================================================
void AD9854_SetFSK(ulong Freq1,ulong Freq2)
{
  uchar count=6;
	uchar Adress1,Adress2;

	const uint Shape=4000;	      //幅度设置. 为12 Bit,取值范围为(0~4095)
	
	Adress1=0x04;				 //选择频率控制字1地址的初值
	Adress2=0x0a;				 //选择频率控制字2地址的初值
	
	Freq_convert(Freq1);               //频率转换1
	
	for(count=6;count>0;)	          //写入6字节的频率控制字  
    {
		AD9854_WR_Byte(Adress1++,FreqWord[--count]);
    }
	
	Freq_convert(Freq2);               //频率转换2

	for(count=6;count>0;)	AD9854_WR_Byte(Adress2++,FreqWord[--count]);          //写入6字节的频率控制字  

	AD9854_WR_Byte(0x21,Shape>>8);	      //设置I通道幅度
	AD9854_WR_Byte(0x22,(uchar)(Shape&0xff));
	
	AD9854_WR_Byte(0x23,Shape>>8);	     //设置Q通道幅度
	AD9854_WR_Byte(0x24,(uchar)(Shape&0xff));

	AD9854_UDCLK=1;                    //更新AD9854输出
  AD9854_UDCLK=0;		
}

//====================================================================================
//函数名称:void AD9854_InitBPSK(void)
//函数功能:AD9854的BPSK初始化
//入口参数:无
//出口参数:无
//====================================================================================
void AD9854_InitBPSK(void)
{
	AD9854_IO_Init();
	AD9854_WR=1;                    //将读、写控制端口设为无效
  AD9854_RD=1;
  AD9854_UDCLK=0;
  AD9854_RST=1;                   //复位AD9854
  AD9854_RST=0;

	AD9854_WR_Byte(0x1d,0x00);	       //关闭比较器
	AD9854_WR_Byte(0x1e,CLK_Set);	   //设置系统时钟倍频
	AD9854_WR_Byte(0x1f,0x08);	      //设置系统为模式4，由外部更新
	AD9854_WR_Byte(0x20,0x60);	      //设置为可调节幅度，取消插值补偿

	AD9854_UDCLK=1;                //更新AD9854输出
  AD9854_UDCLK=0;
}

//====================================================================================
//函数名称:void AD9854_SetBPSK(uint Phase1,uint Phase2)
//函数功能:AD9854的BPSK设置
//入口参数:Phase1   调制相位1
//         Phase2	调制相位2
//出口参数:无
//说明：   相位为14Bit，取值从0~16383，建议在用本函数的时候将Phase1设置为0，
//         将Phase1设置为8192，180°相位
//====================================================================================
void AD9854_SetBPSK(uint Phase1,uint Phase2)
{
	uchar count;

	const ulong Freq=60000;
  const uint Shape=4000;

	uchar Adress;
	Adress=0x04;                           //选择频率控制字1地址的初值

	AD9854_WR_Byte(0x00,Phase1>>8);	           //设置相位1
	AD9854_WR_Byte(0x01,(uchar)(Phase1&0xff));
	
	AD9854_WR_Byte(0x02,Phase2>>8);	          //设置相位2
	AD9854_WR_Byte(0x03,(uchar)(Phase2&0xff));

	Freq_convert(Freq);                            //频率转换

	for(count=6;count>0;)	                         //写入6字节的频率控制字  
    {
		AD9854_WR_Byte(Adress++,FreqWord[--count]);
    }

	AD9854_WR_Byte(0x21,Shape>>8);	                  //设置I通道幅度
	AD9854_WR_Byte(0x22,(uchar)(Shape&0xff));
	
	AD9854_WR_Byte(0x23,Shape>>8);	               //设置Q通道幅度
	AD9854_WR_Byte(0x24,(uchar)(Shape&0xff));

	AD9854_UDCLK=1;                                //更新AD9854输出
  AD9854_UDCLK=0;	
}

//====================================================================================
//函数名称:void AD9854_InitOSK(void)
//函数功能:AD9854的OSK初始化
//入口参数:无
//出口参数:无
//====================================================================================
void AD9854_InitOSK(void)
{
	AD9854_IO_Init();
	AD9854_WR=1;                           //将读、写控制端口设为无效
	AD9854_RD=1;
	AD9854_UDCLK=0;
	AD9854_RST=1;                          //复位AD9854
	AD9854_RST=0;

  AD9854_WR_Byte(0x1d,0x00);	           //关闭比较器
	AD9854_WR_Byte(0x1e,CLK_Set);	       //设置系统时钟倍频
	AD9854_WR_Byte(0x1f,0x00);	           //设置系统为模式0，由外部更新
	AD9854_WR_Byte(0x20,0x70);	           //设置为可调节幅度，取消插值补偿,通断整形内部控制

	AD9854_UDCLK=1;                        //更新AD9854输出
	AD9854_UDCLK=0;
}

//====================================================================================
//函数名称:void AD9854_SetOSK(uchar RateShape)
//函数功能:AD9854的OSK设置
//入口参数: RateShape    OSK斜率,取值为4~255，小于4则无效
//出口参数:无
//====================================================================================
void AD9854_SetOSK(uchar RateShape)
{
	uchar count;

	const ulong Freq=60000;			 //设置载频
  const uint  Shape=4000;			//幅度设置. 为12 Bit,取值范围为(0~4095)

	uchar Adress;
	Adress=0x04;               //选择频率控制字地址的初值

	Freq_convert(Freq);                       //频率转换

	for(count=6;count>0;)	 AD9854_WR_Byte(Adress++,FreqWord[--count]);            //写入6字节的频率控制字  

	AD9854_WR_Byte(0x21,Shape>>8);	                  //设置I通道幅度
	AD9854_WR_Byte(0x22,(uchar)(Shape&0xff));
	
	AD9854_WR_Byte(0x23,Shape>>8);	                  //设置Q通道幅度
	AD9854_WR_Byte(0x24,(uchar)(Shape&0xff));  	 


  AD9854_WR_Byte(0x25,RateShape);				       //设置OSK斜率

	AD9854_UDCLK=1;                                //更新AD9854输出
  AD9854_UDCLK=0;	
}

//====================================================================================
//函数名称:void AD9854_InitAM(void)
//函数功能:AD9854的AM初始化
//入口参数:无
//出口参数:无
//====================================================================================
void AD9854_InitAM(void)
{
	uchar count;

	const ulong Freq=60000;			 //设置载频

	uchar  Adress;
	Adress=0x04;      //选择频率控制字地址的初值
	
	AD9854_IO_Init();
  AD9854_WR=1;    //将读、写控制端口设为无效
  AD9854_RD=1;
  AD9854_UDCLK=0;
  AD9854_RST=1;     //复位AD9854
  AD9854_RST=0;

	AD9854_WR_Byte(0x1d,0x00);	                  //关闭比较器
	AD9854_WR_Byte(0x1e,CLK_Set);	             //设置系统时钟倍频
	AD9854_WR_Byte(0x1f,0x00);	                 //设置系统为模式0，由外部更新
	AD9854_WR_Byte(0x20,0x60);	                  //设置为可调节幅度，取消插值补偿

	Freq_convert(Freq);                            //频率转换

	for(count=6;count>0;)	 AD9854_WR_Byte(Adress++,FreqWord[--count]);          //写入6字节的频率控制字  

	AD9854_UDCLK=1;                             //更新AD9854输出
  AD9854_UDCLK=0;
}

//====================================================================================
//函数名称:void AD9854_SetAM(uchar Shape)
//函数功能:AD9854的AM设置
//入口参数:Shape   12Bit幅度,取值从0~4095   
//出口参数:无
//====================================================================================
void AD9854_SetAM(uint Shape)
{
	AD9854_WR_Byte(0x21,Shape>>8);	                  //设置I通道幅度
	AD9854_WR_Byte(0x22,(uchar)(Shape&0xff));
	
	AD9854_WR_Byte(0x23,Shape>>8);	                  //设置Q通道幅度
	AD9854_WR_Byte(0x24,(uchar)(Shape&0xff));

	AD9854_UDCLK=1;                                   //更新AD9854输出
  AD9854_UDCLK=0;			
}

//====================================================================================
//函数名称:void AD9854_InitRFSK(void)
//函数功能:AD9854的RFSK初始化
//入口参数:无
//出口参数:无
//====================================================================================
void AD9854_InitRFSK(void)
{
	AD9854_IO_Init();
	AD9854_WR=1;                      //将读、写控制端口设为无效
  AD9854_RD=1;
  AD9854_UDCLK=0;
  AD9854_RST=1;                     //复位AD9854
  AD9854_RST=0;
	
	AD9854_WR_Byte(0x1d,0x00);	       //关闭比较器
	AD9854_WR_Byte(0x1e,CLK_Set);	   //设置系统时钟倍频
	AD9854_WR_Byte(0x1f,0x24);	        //设置系统为模式2，由外部更新,使能三角波扫频功能
	AD9854_WR_Byte(0x20,0x60);	        //设置为可调节幅度，取消插值补偿	

	AD9854_UDCLK=1;                   //更新AD9854输出
  AD9854_UDCLK=0;
}

//====================================================================================
//函数名称:void AD9854_SetRFSK(void)
//函数功能:AD9854的RFSK设置
//入口参数:Freq_Low          RFSK低频率	   48Bit
//         Freq_High         RFSK高频率	   48Bit
//         Freq_Up_Down		 步进频率	   48Bit
//		   FreRate           斜率时钟控制  20Bit
//出口参数:无
//注：     每两个脉冲之间的时间周期用下式表示（FreRate +1）*（System Clock ），一个脉冲,
//         频率 上升或者下降 一个步进频率
//====================================================================================
void AD9854_SetRFSK(ulong Freq_Low,ulong Freq_High,ulong Freq_Up_Down,ulong FreRate)
{
	uchar count=6;
	uchar Adress1,Adress2,Adress3;
    const uint  Shape=4000;			   //幅度设置. 为12 Bit,取值范围为(0~4095)

	Adress1=0x04;		     //选择频率控制字地址的初值 
	Adress2=0x0a;
	Adress3=0x10;

	Freq_convert(Freq_Low);                             //频率1转换

	for(count=6;count>0;)	                         //写入6字节的频率控制字  
    {
		AD9854_WR_Byte(Adress1++,FreqWord[--count]);
    }

	Freq_convert(Freq_High);                             //频率2转换

	for(count=6;count>0;)	                         //写入6字节的频率控制字  
    {
		AD9854_WR_Byte(Adress2++,FreqWord[--count]);
    }

	Freq_convert(Freq_Up_Down);                             //步进频率转换

	for(count=6;count>0;)	                               //写入6字节的频率控制字  
    {
		AD9854_WR_Byte(Adress3++,FreqWord[--count]);
    }

	AD9854_WR_Byte(0x1a,(uchar)((FreRate>>16)&0x0f));				   //设置斜升速率
	AD9854_WR_Byte(0x1b,(uchar)(FreRate>>8));
	AD9854_WR_Byte(0x1c,(uchar)FreRate);				    

	AD9854_WR_Byte(0x21,Shape>>8);	                  //设置I通道幅度
	AD9854_WR_Byte(0x22,(uchar)(Shape&0xff));
	
	AD9854_WR_Byte(0x23,Shape>>8);	                  //设置Q通道幅度
	AD9854_WR_Byte(0x24,(uchar)(Shape&0xff));

	AD9854_UDCLK=1;                                //更新AD9854输出
  AD9854_UDCLK=0;	
}


//以下为各功能测试主函数【STC51】
//int main()
//{
//	AD9854_Init();
//	AD9854_SetSine(80000000,4095);
//	while(1);			
//}

 
//测试正弦波，采用120MHZ SYSCLK时,出来87.697HZ波形，波形很好，测试成功
//
//int main()
//{
//	AD9854_Init();
//	AD9854_SetSine_double(87.697,4000);
//	while(1);			
//} 


//测试FSK，采用120MHZ SYSCLK,1K和6K,测试成功,结果对应"FSK波形.bmp"

//int main()
//{
//	AD9854_InitFSK();
//	AD9854_SetFSK(1000,6000);
//	while(1)
//	{
//		AD9854_FDATA = 1;
//		delay_us(30000);	      //延时时间长，便于观察
//		AD9854_FDATA = 0;
//		delay_us(30000);
//	}			
//} 


//测试BPSK，采用120MHZ SYSCLK,测试成功

//int main()
//{
//	AD9854_InitBPSK();
//	AD9854_SetBPSK(0,8192);
//	while(1)
//	{
//		AD9854_FDATA = 1;
//		delay_us(10);	      
//		AD9854_FDATA = 0;
//		delay_us(10);
//	}			
//} 



//测试OSK，采用120MHZ SYSCLK,测试成功

//int main()
//{
//	AD9854_InitOSK();
//	AD9854_SetOSK(10);
//	while(1)
//	{
//		AD9854_OSK=1;	
//		delay_us(30); 
//		AD9854_OSK=0;	
//		delay_us(30);
//	}			
//} 


//测试AM，采用120MHZ SYSCLK,测试成功

//int main()
//{
//	AD9854_InitAM();
//	while(1)
//	{
//		AD9854_SetAM(2000);
//		delay_us(10);	      
//		AD9854_SetAM(4000);
//		delay_us(10);
//	}
//}
// 

//测试RFSK，采用120MHZ SYSCLK,测试成功

//int main()
//{
//	AD9854_InitRFSK();
//	AD9854_SetRFSK(1000,60000,100,30);
//	while(1)
//	{
//		AD9854_FDATA = 1;
//		delay_us(30000);	      //延时时间长，便于观察
//		AD9854_FDATA = 0;
//		delay_us(30000);
//	} 
//}			
