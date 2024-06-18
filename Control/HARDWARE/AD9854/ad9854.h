#ifndef __AD9854_H
#define __AD9854_H	 
#include "main.h"

#define AD9854_RST    PFout(6)   //AD9854复位端口
#define AD9854_UDCLK  PFout(4)   //AD9854更新时钟
#define AD9854_WR     PFout(5)  //AD9854写使能，低有效
#define AD9854_RD     PFout(8)   //AD9854读使能，低有效
#define AD9854_OSK    PFout(2)   //AD9854 OSK控制端
#define AD9854_SP     PFout(7)  //串行和并行控制，默认拉高为并行控制数据

#define AD9854_FDATA  PFout(3)   //AD9854 FSK,PSK控制

#define AD9854_DataBus GPIOE->ODR
#define AD9854_AdrBus  GPIOC->ODR

#define uint  unsigned int
#define uchar unsigned char
#define ulong unsigned long

 void AD9854_Init(void);						  
 void Freq_convert(long Freq);	         	  
 void AD9854_SetSine(ulong Freq,uint Shape);	  
 void Freq_double_convert(double Freq);		  
 void AD9854_SetSine_double(double Freq,uint Shape);
 void AD9854_InitFSK(void);				
 void AD9854_SetFSK(ulong Freq1,ulong Freq2);					  
 void AD9854_InitBPSK(void);					  
 void AD9854_SetBPSK(uint Phase1,uint Phase2);					
 void AD9854_InitOSK(void);					 
 void AD9854_SetOSK(uchar RateShape);					  
 void AD9854_InitAM(void);					 
 void AD9854_SetAM(uint Shape);					
 void AD9854_InitRFSK(void);					 
 void AD9854_SetRFSK(ulong Freq_Low,ulong Freq_High,ulong Freq_Up_Down,ulong FreRate);
 void AD9854_WR_Byte(uint32_t addr,uint32_t dat);
 void AD9854_IO_Init(void);

#endif

