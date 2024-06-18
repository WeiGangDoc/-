/*************************************************************************************************************
 * 文件名:		motor_42.h
 * 功能:		驱动42步进电机
使用方法:	
		PA5		--> TIM2_CH1 -->电机X
		PA7		--> TIM3_CH2 -->电机Y
    PB1		--> TIM3_CH4 -->电机A
		PB10	--> TIM2_CH3 -->电机A
在main.h中添加代码
#include "motor_42.h"

*************************************************************************************************************/
#include "motor_42.h"
#include "sys.h"
#include "tim.h"
#include "main.h"
void Motor_init(void)
{
	//通过PWM调节电压，来调节挡位
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, 0);	//PA5
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, 0);	//PA7
		HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4, 0);	//PB1
		HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3, 0);	//PB10
}
/**
 * @brief	步进电机选择控制
 * @param motor - 选择电机，dir - 选择方向，state - 选择开闭
 * @note	移植时，请使用自己的底层API实现 
*/

void Motor_state(uint8_t motor,uint8_t dir,uint8_t state)
{
	switch(motor)
	{
		case 1:
			if(dir == 0)DIR1_0;
			else if(dir == 1)DIR1_1;
			if(state == 1)
			{
				HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
				__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, 100);
			}else if(state == 0)
			{
				HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
				__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_1, 0);
			}break;
		case 2:
			if(dir == 0)DIR2_0;
			else if(dir == 1)DIR2_1;
			if(state == 1)
			{
				HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, 100);
			}else if(state == 0)
			{
				HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_2, 0);
			}break;
		
		case 3:
			if		 (dir == 0)DIR3_0;
			else if(dir == 1)DIR3_1;
			if(state == 1)
			{
				HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4, 100);
			}else if(state == 0)
			{
				HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
				__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4, 0);
			}break;
		case 4:
			if		 (dir == 0)DIR4_0;
			else if(dir == 1)DIR4_1;
			if(state == 1)
			{
				HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
				__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3, 100);
			}else if(state == 0)
			{
				HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
				__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_3, 0);
			}break;
			
	}
}
