/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sys.h"
#include "oled.h"
#include "esp01s.h"
#include "jansson.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern uint8_t usart1_rx_flag;          //接收中断标志位
extern uint8_t usart2_rx_flag;          //接收中断标志位
extern uint8_t usart6_rx_flag;          //接收中断标志位

extern uint8_t tx2_buff[255];
extern uint8_t tx6_buff[255];

extern uint8_t rx1_buff[255];
extern uint8_t rx2_buff[255];
extern uint8_t rx6_buff[255];

//json解析与数据
json_error_t error;
json_t *root;
char *result;
uint16_t temp[2] = {0,0},humi[2] = {0,0},lux[2] = {0,0};
uint16_t tempv = 60,humiv = 60,luxv = 100;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint16_t index = 0;
	char data[128];
	uint8_t mode = 0,nc1 = 0,nc2 = 0,nc3 = 0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
	delay_init(100);
	MU_USART_init();
	OLED_Init();

	LED1,OLED_ShowString(0, 0, "MQTT Disconnect", 16);
	NBiot_Init();      //NBIOT初始化
	LED0,OLED_ShowString(0, 0, "MQTT Connect   ", 16);
	Sub_MQTT("soft");
	delay_ms(1000);
	printf("init ok\r\n");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		delay_ms(10);
		if(usart2_rx_flag == 1)
		{
			usart2_rx_flag = 0;
			if		 (strstr((char*)rx2_buff, "mode0"))mode = 0;
			else if(strstr((char*)rx2_buff, "mode1"))mode = 1;
			else if(strstr((char*)rx2_buff, "nc10"))nc1 = 0;
			else if(strstr((char*)rx2_buff, "nc11"))nc1 = 1;
			else if(strstr((char*)rx2_buff, "nc20"))nc2 = 0;
			else if(strstr((char*)rx2_buff, "nc21"))nc2 = 1;
			else if(strstr((char*)rx2_buff, "nc30"))nc3 = 0;
			else if(strstr((char*)rx2_buff, "nc31"))nc3 = 1;
			else if(strstr((char*)rx2_buff, "envi"))nc3 = 1;
			{
				result = strstr((char*)rx2_buff, "{");
				printf("%s",result);
				root = json_loads(result, 0, &error);
				if(json_is_object(root))
				{
					//解析软件端阈值控制信息
					tempv = json_integer_value(json_object_get(root, "tempv"));
					humiv = json_integer_value(json_object_get(root, "humiv"));
					luxv  = json_integer_value(json_object_get(root, "luxv"));
					printf("id= %d,templ = %d,temph = %d\r\n",tempv,humiv,luxv);
				}else printf("root format error:%d-%s\r\n", error.line, error.text);
				json_delete(root);
			}
			memset(rx2_buff,0,sizeof(rx2_buff));
		}
		
		if(usart6_rx_flag == 1)
		{
			usart6_rx_flag = 0;
			printf("%s\r\n",rx6_buff);
			if(strstr((char*)rx6_buff, "node1"))
			{
				result = strstr((char*)rx6_buff, "{");
				printf("%s",result);
				root = json_loads(result, 0, &error);
				if(json_is_object(root))
				{
					//解析软件端阈值控制信息
					temp[0] = json_integer_value(json_object_get(root, "temp"));
					humi[0] = json_integer_value(json_object_get(root, "humi"));
					lux [0]  = json_integer_value(json_object_get(root, "lux"));
					printf("node1:temp= %d,humi = %d,lux = %d\r\n",temp[0],humi[0],lux[0]);
				}else printf("root format error:%d-%s\r\n", error.line, error.text);
				json_delete(root);
			}else if(strstr((char*)rx6_buff, "node2"))
			{
				result = strstr((char*)rx6_buff, "{");
				printf("%s",result);
				root = json_loads(result, 0, &error);
				if(json_is_object(root))
				{
					//解析软件端阈值控制信息
					temp[1] = json_integer_value(json_object_get(root, "temp"));
					humi[1] = json_integer_value(json_object_get(root, "humi"));
					lux [1] = json_integer_value(json_object_get(root, "lux"));
					printf("node2:temp= %d,humi = %d,lux = %d\r\n",temp[1],humi[1],lux[1]);
				}else printf("root format error:%d-%s\r\n", error.line, error.text);
				json_delete(root);
			}
			memset(rx6_buff,0,sizeof(rx6_buff));
		}
		//1秒更新一次OLED数据与阈值控制信息
		if(index % 100 == 0)
		{
			sprintf(data,"T:%d H:%d L:%d ",temp[0],humi[0],lux[0]);
			OLED_ShowString(0,0,data,16);
			memset(data,0,sizeof(data));
			delay_ms(20);
			sprintf(data,"T:%d H:%d L:%d ",temp[1],humi[1],lux[1]);
			OLED_ShowString(0,2,data,16);
			memset(data,0,sizeof(data));
			sprintf(data,"M:%d  ",mode);
			OLED_ShowString(0,4,data,16);
			memset(data,0,sizeof(data));
			sprintf(data,"N1:%d N2:%d N3:%d ",nc1,nc2,nc3);
			OLED_ShowString(0,6,data,16);
			memset(data,0,sizeof(data));
			if(mode == 1)
			{
				if((temp[0]>tempv)||(temp[1]>tempv))nc1 = 1;
				else nc1 = 0;
				if((humi[0]>humiv)||(humi[1]>humiv))nc2 = 1;
				else nc2 = 0;
				if((lux[0]>luxv)||(lux[1]>luxv))		nc3 = 1;
				else nc3 = 0;
			}
			u6_printf("nc1%d\r\n",nc1);
			delay_ms(100);
			u6_printf("nc2%d\r\n",nc2);
			delay_ms(100);
			u6_printf("nc3%d\r\n",nc3);
			delay_ms(100);
		}
		//4秒上传一次数据
		if(index %400 == 0)
		{
			index = 0;
			//打包JSON数据，通过ESP01S模块发送出去
			root = json_pack("{sisisi}","temp1",temp[0],"humi1",humi[0],"lux1",lux[0]);
			result = json_dumps(root,JSON_PRESERVE_ORDER);
			json_delete(root);
			Pub_MQTT("hard",result);
			delay_ms(100);
			root = json_pack("{sisisi}","temp2",temp[1],"humi2",humi[1],"lux2",lux[1]);
			result = json_dumps(root,JSON_PRESERVE_ORDER);
			json_delete(root);
			Pub_MQTT("hard",result);
			delay_ms(100);
		}index ++;

		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

