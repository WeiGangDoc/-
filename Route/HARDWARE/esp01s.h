#ifndef __ESP01S_H__
#define __ESP01S_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "sys.h"

#define ESP_usart huart2
void NBiot_Init(void);
void NBiot_gpio_init(void);
void Sub_MQTT(char* Sub);
void Pub_MQTT(char* topic, char* message);
uint8_t connected_IP(char* id,char* user,char* pwd);
void Do_Work(char* NB_Type);
int NBiot_SendCmd(char* cmd, char* reply, int wait);
uint8_t NBiot_CheckIOT(char* cmd, int wait);
#endif

