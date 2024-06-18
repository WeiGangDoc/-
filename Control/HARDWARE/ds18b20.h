#ifndef __DS18B20_H
#define __DS18B20_H
#include "sys.h"

//IO��������
#define DS18B20_IO_IN()  {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=0<<(9*2);}	//PB9����ģʽ
#define DS18B20_IO_OUT() {GPIOB->MODER&=~(3<<(9*2));GPIOB->MODER|=1<<(9*2);} 	//PB9���ģʽ

////IO��������											   
#define	DS18B20_DQ_OUT PBout(9) //���ݶ˿�	PB9
#define	DS18B20_DQ_IN  PBin(9)  //���ݶ˿�	PB9 
   	
uint8_t DS18B20_Init(void);			//��ʼ��DS18B20
short DS18B20_Get_Temp(void);	//��ȡ�¶�
void DS18B20_Start(void);		//��ʼ�¶�ת��
void DS18B20_Write_Byte(uint8_t dat);//д��һ���ֽ�
uint8_t DS18B20_Read_Byte(void);		//����һ���ֽ�
uint8_t DS18B20_Read_Bit(void);		//����һ��λ
uint8_t DS18B20_Check(void);			//����Ƿ����DS18B20
void DS18B20_Rst(void);			//��λDS18B20 
#endif
