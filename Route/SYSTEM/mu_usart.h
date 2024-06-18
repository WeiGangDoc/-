#ifndef __MU_USART_H__
#define __MU_USART_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "main.h"
#include "stdio.h"
#include "string.h"
#include "usart.h"

#define  BUFFER_SIZE  (255)
#define u2_printf(...)	HAL_UART_Transmit(&huart2,(uint8_t *)tx2_buff,sprintf((char*)tx2_buff,__VA_ARGS__),0xffff)
#define u6_printf(...)	HAL_UART_Transmit(&huart6,(uint8_t *)tx6_buff,sprintf((char*)tx6_buff,__VA_ARGS__),0xffff)

void MU_USART_init(void);

void uart1_init(uint32_t bound);
void uart2_init(uint32_t bound);


void USER_UART_IRQHandler(UART_HandleTypeDef *huart);
void USAR_UART_IDLECallback(UART_HandleTypeDef *huart);


#endif
