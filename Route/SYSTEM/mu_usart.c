/*************************************************************************************************************
 * 文件名:		mu_usart.h
 * 功能:		自定义双串口中断+接受不定长数据
 * 作者:		mu
 * 详细:PA9--TX1	PA10--RX1	PA2--TX2	PA3--RX2		PA11--TX6	PA12--RX6
使用方法:	
CUBEMX:配置串口1与串口2，开启串口DMA循环接收与串口中断
KEIL:在魔术棒里面勾选Target里面勾选Use MiscroLIB
在main.h中添加代码
#include "mu_usart.h"
MU_USART_init();

在stm32f4xx_it.h中添加代码
#include "mu_usart.h"
在stm32f4xx_itc.中添加代码
void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart1);
	USER_UART_IRQHandler(&huart1);
}
void USART2_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart2);
	USER_UART_IRQHandler(&huart2);
}

*************************************************************************************************************/

#include "mu_usart.h"
#include "string.h"

uint8_t usart1_rx_flag = 0;          //接收中断标志位
uint8_t usart2_rx_flag = 0;          //接收中断标志位
uint8_t usart6_rx_flag = 0;          //接收中断标志位

uint8_t tx2_buff[255];
uint8_t tx6_buff[255];

uint8_t rx1_buff[255];
uint8_t rx2_buff[255];
uint8_t rx6_buff[255];

uint8_t data_length;
	
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;

extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;

extern UART_HandleTypeDef huart6;
extern DMA_HandleTypeDef hdma_usart6_rx;

extern uint8_t peopleval,tempval,humival,windval,waterval;
void MU_USART_init()
{
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);							//使能USART1中断
	HAL_UART_Receive_DMA(&huart1, (uint8_t*)rx1_buff, 255);		//使能USART1的DMA数据接受
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);							//使能USART2中断
	HAL_UART_Receive_DMA(&huart2, (uint8_t*)rx2_buff, 255);		//使能USART2的DMA数据接受
	__HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);							//使能USART2中断
	HAL_UART_Receive_DMA(&huart6, (uint8_t*)rx2_buff, 255);		//使能USART2的DMA数据接受

	printf("USART init ok\r\n");	//开机打印hello
}

void uart1_init(uint32_t bound)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = bound;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}

void uart2_init(uint32_t bound)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = bound;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}



//串口中断处理函数
void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{	
	if(USART1 == huart1.Instance)																	//判断是否为USART1
	{
		if(RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE))   		//判断是否是空闲中断
		{
			__HAL_UART_CLEAR_IDLEFLAG(&huart1);                     //清除空闲中断标志（否则会一直不断进入中断）
			HAL_UART_DMAStop(&huart1);
			if(usart1_rx_flag != 1)usart1_rx_flag = 1;
			//停止本次DMA传输
			if(data_length != 0 )data_length=0;
			data_length  = BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);   //计算接收到的数据长度
			
			memset(rx1_buff,0,data_length);                                            //清零接收缓冲区
			HAL_UART_Receive_DMA(&huart1, (uint8_t*)rx1_buff, 255);                    //重启开始DMA传输 每次255字节数据
		}
	}
	if(USART2 == huart2.Instance)                                   //判断是否是串口1（！此处应写(huart->Instance == USART1)
	{
		if(RESET != __HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE))   //判断是否是空闲中断
		{
			__HAL_UART_CLEAR_IDLEFLAG(&huart2);                     //清楚空闲中断标志（否则会一直不断进入中断）
			HAL_UART_DMAStop(&huart2);                                                     //停止本次DMA传输
			if(usart2_rx_flag != 1)usart2_rx_flag = 1;
			if(data_length != 0 )data_length=0;
			data_length  = BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);   //计算接收到的数据长度
			HAL_UART_Transmit(&huart1,rx2_buff,data_length,0x200); 
			//清零接收缓冲区，当程序需要用此处判断则需注释掉，在其他地方进行清零
//			memset(rx2_buff,0,data_length);
			HAL_UART_Receive_DMA(&huart2, (uint8_t*)rx2_buff, 255);                    //重启开始DMA传输 每次255字节数据
		}
	}
	if(USART6 == huart6.Instance)                                   //判断是否是串口1（！此处应写(huart->Instance == USART1)
	{
		if(RESET != __HAL_UART_GET_FLAG(&huart6, UART_FLAG_IDLE))   //判断是否是空闲中断
		{
			__HAL_UART_CLEAR_IDLEFLAG(&huart6);                     //清楚空闲中断标志（否则会一直不断进入中断）
			HAL_UART_DMAStop(&huart6);                                                     //停止本次DMA传输
			if(usart6_rx_flag != 1)usart6_rx_flag = 1;
			if(data_length != 0 )data_length=0;
			data_length  = BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart6_rx);   //计算接收到的数据长度
//			HAL_UART_Transmit(&huart1,rx6_buff,data_length,0x200); 
			//清零接收缓冲区，当程序需要用此处判断则需注释掉，在其他地方进行清零
//			memset(rx2_buff,0,data_length);
			HAL_UART_Receive_DMA(&huart6, (uint8_t*)rx6_buff, 255);                    //重启开始DMA传输 每次255字节数据
		}
	}

}

//串口重定向
#if 1
#include <stdio.h>

int fputc(int ch, FILE *stream)
{
    /* 堵塞判断串口是否发送完成 */
    while((USART1->SR & 0X40) == 0);

    /* 串口发送完成，将该字符发送 */
    USART1->DR = (uint8_t) ch;

    return ch;
}
#endif

