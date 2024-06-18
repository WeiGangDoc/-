/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define KEY_Pin GPIO_PIN_0
#define KEY_GPIO_Port GPIOA
#define DIR1_Pin GPIO_PIN_4
#define DIR1_GPIO_Port GPIOA
#define DIR2_Pin GPIO_PIN_6
#define DIR2_GPIO_Port GPIOA
#define DIR3_Pin GPIO_PIN_0
#define DIR3_GPIO_Port GPIOB
#define DIR4_Pin GPIO_PIN_2
#define DIR4_GPIO_Port GPIOB
#define NC1_Pin GPIO_PIN_12
#define NC1_GPIO_Port GPIOB
#define NC2_Pin GPIO_PIN_13
#define NC2_GPIO_Port GPIOB
#define NC3_Pin GPIO_PIN_14
#define NC3_GPIO_Port GPIOB
#define NC4_Pin GPIO_PIN_15
#define NC4_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define KEY_A0		HAL_GPIO_ReadPin(KEY_GPIO_Port,KEY_Pin)
#define LED		  HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin)
#define LED1 		HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_RESET)	//低电平亮
#define LED0 		HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_SET)		//高电平灭

#define NC1_0 		HAL_GPIO_WritePin(NC1_GPIO_Port,NC1_Pin,GPIO_PIN_RESET)
#define NC1_1 		HAL_GPIO_WritePin(NC1_GPIO_Port,NC1_Pin,GPIO_PIN_SET)
#define NC2_0 		HAL_GPIO_WritePin(NC2_GPIO_Port,NC2_Pin,GPIO_PIN_RESET)
#define NC2_1 		HAL_GPIO_WritePin(NC2_GPIO_Port,NC2_Pin,GPIO_PIN_SET)
#define NC3_0 		HAL_GPIO_WritePin(NC3_GPIO_Port,NC3_Pin,GPIO_PIN_RESET)
#define NC3_1 		HAL_GPIO_WritePin(NC3_GPIO_Port,NC3_Pin,GPIO_PIN_SET)
#define NC4_0 		HAL_GPIO_WritePin(NC4_GPIO_Port,NC4_Pin,GPIO_PIN_RESET)
#define NC4_1 		HAL_GPIO_WritePin(NC4_GPIO_Port,NC4_Pin,GPIO_PIN_SET)

#define DIR1_0		HAL_GPIO_WritePin(DIR1_GPIO_Port,DIR1_Pin,GPIO_PIN_RESET)
#define DIR1_1		HAL_GPIO_WritePin(DIR1_GPIO_Port,DIR1_Pin,GPIO_PIN_SET)
#define DIR2_0		HAL_GPIO_WritePin(DIR2_GPIO_Port,DIR2_Pin,GPIO_PIN_RESET)
#define DIR2_1		HAL_GPIO_WritePin(DIR2_GPIO_Port,DIR2_Pin,GPIO_PIN_SET)
#define DIR3_0		HAL_GPIO_WritePin(DIR3_GPIO_Port,DIR3_Pin,GPIO_PIN_RESET)
#define DIR3_1		HAL_GPIO_WritePin(DIR3_GPIO_Port,DIR3_Pin,GPIO_PIN_SET)
#define DIR4_0		HAL_GPIO_WritePin(DIR4_GPIO_Port,DIR4_Pin,GPIO_PIN_RESET)
#define DIR4_1		HAL_GPIO_WritePin(DIR4_GPIO_Port,DIR4_Pin,GPIO_PIN_SET)


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
