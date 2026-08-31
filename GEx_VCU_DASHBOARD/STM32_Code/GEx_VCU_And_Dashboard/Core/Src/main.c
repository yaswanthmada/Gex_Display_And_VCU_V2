/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include"RCC.h"
#include"SPI.h"
#include"MCP2515.h"
#include"PROCESS.h"
#include"DECODE_BATTERY_MANAGEMENT_SYSTEM.h"
#include"DECODE_MOTOR_CONTROLLER.h"
#include<string.h>
#include <stdio.h>
#include"UART.h"
#include"DWIN_DISPLAY.h"
#include"CAN.h"
#include"SYSTICK.h"
#include"GPIO.h"
#include"ADC.h"
#include"DWIN_DEFINES.h"
#include"SYSTICK.h"
#include"IWDT.h"
#include"FLASH.h"
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

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
     Stm32f103_System_Clock_Init();

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
     if(!Universal_Asyn_Rx_Tx_1_Init())
     {
     	//uart1 Inititialization Fails
 	    Error_Handler();
     }
     Uart_Printf("[INFO] UART1 Initialized...\r\n");
     if(!Universal_Asyn_Rx_Tx_3_Init())
     {
     	//uart3 Inititialization Fails
		Uart_Printf("[ERROR] Failed to initialize GPIO\r\n");
		Error_Handler();
     }
     Uart_Printf("[INFO] UART3 Initialized...\r\n");
   if(!Serial_Peripheral_Interface_Init())
     {
     	//SPI Inititialization Fails
 	    Uart_Printf("[ERROR] Failed to initialize SPI\r\n");
 	    Error_Handler();
     }
    Uart_Printf("[INFO] SPI Initialized...\r\n");
     if (!Micro_Chip_Product_2515_Init())
     {
     	//MCP2515 Inititialization Fails
  	    Uart_Printf("[ERROR] Failed to initialize MCP2515\r\n");
  	    Error_Handler();
      }
     Uart_Printf("[INFO] MCP2515 Initialized...\r\n");
     if(!Configure_Mcp2515_Can_Ids())
     {
   	    Uart_Printf("[ERROR] Failed to  Configure_Mcp2515_Can_Ids\r\n");
     }
     else
     {
   	  Uart_Printf("[INFO] Configure_Mcp2515_Can_Ids OK\r\n");
     }
     if(!Core_Timer_Init())
     {
     	//systick Inititialization Fails
   	    Uart_Printf("[ERROR] Failed to initialize systick\r\n");
   	    Error_Handler();
       }
      Uart_Printf("[INFO] systick Initialized...\r\n");
     if(!Controller_Area_Network_Init())
     {
     	//CAN Inititialization Fails
    	 Uart_Printf("[ERROR] Failed to initialize CAN\r\n");
    	 Error_Handler();
     }
     Uart_Printf("[INFO] CAN Started at 500kbps\r\n");
     Configure_Can_filter_Ids();
     Uart_Printf("[INFO] Initializing GPIOs...\r\n");
     if(!General_Purpose_Input_Output_Init())
     {
 	    Uart_Printf("[ERROR] Failed to initialize GPIO\r\n");
 	    Error_Handler();
     }
     Uart_Printf("[INFO] GPIOs Initialized...\r\n");
     if(!Analog_To_Digital_Converter_Init())
     {
     	//ADC Inititialization  Fails
		Uart_Printf("[ERROR] Failed to initialize ADCs\r\n");
		Error_Handler();
     }
     Uart_Printf("[INFO] ADCs Initialized...\r\n");
     if(!Enable_Task_Timer_Init())
     {
     	//Task Inititialization Fails
    	  Uart_Printf("[ERROR]Failed to initialize Task timer\r\n");
     }
     else
     {
    	  Uart_Printf("[INFO] Task timer initialized\r\n");
     }
     if(!Enable_Gpio_Print_Task())
     {
         Uart_Printf("[ERROR] Failed to create MCU Control task\r\n");
     }
     else
     {
         Uart_Printf("[INFO] Enable_Gpio_Print_Task enabled \r\n");
     }
     if(!Enable_Adc_Print_Task())
     {
         Uart_Printf("[ERROR] Failed to create Enable_Adc_Print_Task\r\n");
     }
     else
     {
         Uart_Printf("[INFO] Enable_Adc_Print_Task enabled \r\n");
     }
     if(!Enable_Mcu_Print_Task())
     {
         Uart_Printf("[ERROR] Failed to create Enable_Mcu_Print_Task\r\n");
     }
     else
     {
         Uart_Printf("[INFO] Enable_Mcu_Print_Task enabled\r\n");
     }
     if(!Enable_Bms_Print_Task())
     {
         Uart_Printf("[ERROR] Failed to create Enable_Bms_Print_Task\r\n");
     }
     else
     {
         Uart_Printf("[INFO] Enable_Bms_Print_Task enabled \r\n");
     }
     if(!Enable_Display_Print_Task())
     {
         Uart_Printf("[ERROR] Failed to create Enable_Display_Print_Task\r\n");
     }
     else
     {
         Uart_Printf("[INFO] Enable_Display_Print_Task enabled \r\n");
     }
     Wdt_Log_Check_And_Process();
     IWDG_Init(2000);
     Uart_Printf("[INFO] Entering in to Super Loop \r\n");
    /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  uint32_t Start_time=Get_Tick_Ms();
	  IWDG_Refresh();
	  Request_Bms_Messages();
	  Process_Can_Messages();
	  Update_Gpio();
	  Update_Adc();
	  Display_Update_All();
	  Task_Timer_Run_All();
	  Uart_Printf("Time Taken for While loop Iteration :%d in ms\n",Get_Tick_Ms()-Start_time);

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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void LED_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    GPIOB->CRL &= ~(0xFU << (2U * 4U));
    GPIOB->CRL |=  (0x2U << (2U * 4U));
    GPIOB->BSRR = GPIO_BSRR_BR2;
}
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
  LED_Init();
  GPIOB->BSRR = GPIO_BSRR_BS2;
  while (1)
  {
	  GPIOB->BSRR ^= GPIO_BSRR_BS2;
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
