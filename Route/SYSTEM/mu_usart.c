/*************************************************************************************************************
 * �ļ���:		mu_usart.h
 * ����:		�Զ���˫�����ж�+���ܲ���������
 * ����:		mu
 * ��ϸ:PA9--TX1	PA10--RX1	PA2--TX2	PA3--RX2		PA11--TX6	PA12--RX6
ʹ�÷���:	
CUBEMX:���ô���1�봮��2����������DMAѭ�������봮���ж�
KEIL:��ħ�������湴ѡTarget���湴ѡUse MiscroLIB
��main.h����Ӵ���
#include "mu_usart.h"
MU_USART_init();

��stm32f4xx_it.h����Ӵ���
#include "mu_usart.h"
��stm32f4xx_itc.����Ӵ���
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

uint8_t usart1_rx_flag = 0;          //�����жϱ�־λ
uint8_t usart2_rx_flag = 0;          //�����жϱ�־λ
uint8_t usart6_rx_flag = 0;          //�����жϱ�־λ

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
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);							//ʹ��USART1�ж�
	HAL_UART_Receive_DMA(&huart1, (uint8_t*)rx1_buff, 255);		//ʹ��USART1��DMA���ݽ���
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);							//ʹ��USART2�ж�
	HAL_UART_Receive_DMA(&huart2, (uint8_t*)rx2_buff, 255);		//ʹ��USART2��DMA���ݽ���
	__HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);							//ʹ��USART2�ж�
	HAL_UART_Receive_DMA(&huart6, (uint8_t*)rx2_buff, 255);		//ʹ��USART2��DMA���ݽ���

	printf("USART init ok\r\n");	//������ӡhello
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



//�����жϴ�����
void USER_UART_IRQHandler(UART_HandleTypeDef *huart)
{	
	if(USART1 == huart1.Instance)																	//�ж��Ƿ�ΪUSART1
	{
		if(RESET != __HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE))   		//�ж��Ƿ��ǿ����ж�
		{
			__HAL_UART_CLEAR_IDLEFLAG(&huart1);                     //��������жϱ�־�������һֱ���Ͻ����жϣ�
			HAL_UART_DMAStop(&huart1);
			if(usart1_rx_flag != 1)usart1_rx_flag = 1;
			//ֹͣ����DMA����
			if(data_length != 0 )data_length=0;
			data_length  = BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);   //������յ������ݳ���
			
			memset(rx1_buff,0,data_length);                                            //������ջ�����
			HAL_UART_Receive_DMA(&huart1, (uint8_t*)rx1_buff, 255);                    //������ʼDMA���� ÿ��255�ֽ�����
		}
	}
	if(USART2 == huart2.Instance)                                   //�ж��Ƿ��Ǵ���1�����˴�Ӧд(huart->Instance == USART1)
	{
		if(RESET != __HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE))   //�ж��Ƿ��ǿ����ж�
		{
			__HAL_UART_CLEAR_IDLEFLAG(&huart2);                     //��������жϱ�־�������һֱ���Ͻ����жϣ�
			HAL_UART_DMAStop(&huart2);                                                     //ֹͣ����DMA����
			if(usart2_rx_flag != 1)usart2_rx_flag = 1;
			if(data_length != 0 )data_length=0;
			data_length  = BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);   //������յ������ݳ���
			HAL_UART_Transmit(&huart1,rx2_buff,data_length,0x200); 
			//������ջ���������������Ҫ�ô˴��ж�����ע�͵����������ط���������
//			memset(rx2_buff,0,data_length);
			HAL_UART_Receive_DMA(&huart2, (uint8_t*)rx2_buff, 255);                    //������ʼDMA���� ÿ��255�ֽ�����
		}
	}
	if(USART6 == huart6.Instance)                                   //�ж��Ƿ��Ǵ���1�����˴�Ӧд(huart->Instance == USART1)
	{
		if(RESET != __HAL_UART_GET_FLAG(&huart6, UART_FLAG_IDLE))   //�ж��Ƿ��ǿ����ж�
		{
			__HAL_UART_CLEAR_IDLEFLAG(&huart6);                     //��������жϱ�־�������һֱ���Ͻ����жϣ�
			HAL_UART_DMAStop(&huart6);                                                     //ֹͣ����DMA����
			if(usart6_rx_flag != 1)usart6_rx_flag = 1;
			if(data_length != 0 )data_length=0;
			data_length  = BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart6_rx);   //������յ������ݳ���
//			HAL_UART_Transmit(&huart1,rx6_buff,data_length,0x200); 
			//������ջ���������������Ҫ�ô˴��ж�����ע�͵����������ط���������
//			memset(rx2_buff,0,data_length);
			HAL_UART_Receive_DMA(&huart6, (uint8_t*)rx6_buff, 255);                    //������ʼDMA���� ÿ��255�ֽ�����
		}
	}

}

//�����ض���
#if 1
#include <stdio.h>

int fputc(int ch, FILE *stream)
{
    /* �����жϴ����Ƿ������ */
    while((USART1->SR & 0X40) == 0);

    /* ���ڷ�����ɣ������ַ����� */
    USART1->DR = (uint8_t) ch;

    return ch;
}
#endif

