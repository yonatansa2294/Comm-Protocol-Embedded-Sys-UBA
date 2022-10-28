/* USER CODE BEGIN Header */
/**
  *******************************************************************************************************************
  * @file   : main.c
  * @brief  : Program to send a text message when the user button is pressed
  * @note		: The sequence for initializing the SIM800 is defined as follows:
  * 					  1. Hardware configuration for SIM management. You can use the function SIM800_Default_ConfigHW()
  * 					  which uses the default UART1 and pins B0 and B1 for powerKey and reset respectively.
  * 					  Otherwise use the SIM800_ConfigHW() function to define the UART and pins.
  * 					  2. Power up the SIM with the SIM800_On() function.
  * 					  3. Initialize the SIM with the SIM800_Init() function.
  * 					  This function configures the SIM in text mode for SMS functions and a baud rate of 9600
  * 					  for communication with the SIM UART.
  * 				  The sequence to send an SMS text message is as follows:
  * 				  	1. Verify if the SIM800 is registered on the network 2. Use the check_Network_Registration() function.
  * 				  	2. If the SIM is registered, use the function send_SMS(cellNumber,message) and pass as parameters
  * 				  	the cell number to which you want to send the message and as a second parameter the text to send.
  * @author	:	Yonatan Aguirre - PCSE CESE18 UBA
  ***********************************************************************************************************************
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "SIM800x.h"
#include "API_Debounce.h"
#include "API_DelayNB.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
delay_t delayNBLeds;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LED_USER_ON         			"LED_USER_ON"
#define LED_USER_OFF        			"LED_USER_OFF"
#define	DELAY_LED						      200LU
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

uint8_t cellNumber[13]="+51123456789";
uint8_t sms[60]="HELLO FROM SIM800: THE BUTTON HAS BEEN PRESSED";
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART1_UART_Init(void);
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
  MX_USART2_UART_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  BSP_LED_Init(LED_USER);
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);

  delayInit(&delayNBLeds,DELAY_LED);	/* initialize structure to implement a delay no blocking */
  debounceFSM_init();					/* initialize FSM for anti debounce*/

  BSP_LED_On(LED_USER);
  /* USER CODE END 2 */
  if(SIM800_Default_ConfigHW() == OK)
  	  HAL_UART_Transmit(&huart2, (const uint8_t*)"HW SIM OK\r\n", strlen("HW SIM OK\r\n"), 1000);
  else
  	  HAL_UART_Transmit(&huart2, (const uint8_t*)"HW SIM NO CONFIG\r\n", strlen("HW SIM NO CONFIG\r\n"), 1000);

  SIM800_On();
  HAL_UART_Transmit(&huart2, (const uint8_t*)"SIM ACTIVATED\r\n", strlen("SIM ACTIVATED\r\n"), 1000);

  if(SIM800_Init() == OK)
	  HAL_UART_Transmit(&huart2, (const uint8_t*)"CONFIGURED\r", strlen("CONFIGURED\r"), 1000);
  else
	  HAL_UART_Transmit(&huart2, (const uint8_t*)"FAIL\r", strlen("NOK\r"), 1000);
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  debounceFSM_update(); //update FSM

	  if (readKey())
	  {
		  HAL_UART_Transmit(&huart2, (const uint8_t*)"BUTTON PRESSED\r", strlen("BUTTON PRESSED\r"), 1000);
		  /*Check if we are registered in the network*/
		  if(check_Network_Registration()== OK)	//
		  {
			  HAL_UART_Transmit(&huart2, (const uint8_t*)"SIM REGISTERED\r", strlen("SIM REGISTERED\r"), 1000);

			  /* Send text SMS to the indicated number */
			  if(send_SMS(&cellNumber,&sms) == OK)
				  HAL_UART_Transmit(&huart2, (const uint8_t*)"SMS SENT\r", strlen("SMS SENT\r"), 1000);
			  else
				  HAL_UART_Transmit(&huart2, (const uint8_t*)"SMS NOT SENT\r", strlen("SMS NOT SENT\r"), 1000);
		  }
	  }

	  /* Check if delay is expired for toggle LED_USER */
	  if(delayRead(&delayNBLeds))	//
		  BSP_LED_Toggle(LED_USER);
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 160;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
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
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
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
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

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
