/*************************************************************************************************************
 * �ļ���:		motor_42.h
 * ����:		����42�������
ʹ�÷���:	
		PA5		--> TIM2_CH1 -->���X
		PA7		--> TIM3_CH2 -->���Y
    PB1		--> TIM3_CH4 -->���A
		PB10	--> TIM2_CH3 -->���A
��main.h����Ӵ���
#include "motor_42.h"

*************************************************************************************************************/
#include "motor_42.h"
#include "sys.h"
#include "tim.h"
#include "main.h"
void Motor_init(void)
{
	//ͨ��PWM���ڵ�ѹ�������ڵ�λ
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
 * @brief	�������ѡ�����
 * @param motor - ѡ������dir - ѡ����state - ѡ�񿪱�
 * @note	��ֲʱ����ʹ���Լ��ĵײ�APIʵ�� 
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
