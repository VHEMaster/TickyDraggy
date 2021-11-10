/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LCD_LED_Pin GPIO_PIN_1
#define LCD_LED_GPIO_Port GPIOA
#define TP_INT_Pin GPIO_PIN_4
#define TP_INT_GPIO_Port GPIOC
#define TP_INT_EXTI_IRQn EXTI4_IRQn
#define TP_BUSY_Pin GPIO_PIN_5
#define TP_BUSY_GPIO_Port GPIOC
#define TP_BUSY_EXTI_IRQn EXTI9_5_IRQn
#define SPI2_NSS_Pin GPIO_PIN_12
#define SPI2_NSS_GPIO_Port GPIOB
#define FLASH_CS_Pin GPIO_PIN_12
#define FLASH_CS_GPIO_Port GPIOD
#define LCD_RST_Pin GPIO_PIN_13
#define LCD_RST_GPIO_Port GPIOD

#define BUT_RIGHT_GPIO_Port GPIOE
#define BUT_RIGHT_Pin GPIO_PIN_3
#define BUT_UP_GPIO_Port GPIOE
#define BUT_UP_Pin GPIO_PIN_1
#define BUT_LEFT_GPIO_Port GPIOE
#define BUT_LEFT_Pin GPIO_PIN_5
#define BUT_ENTER_GPIO_Port GPIOA
#define BUT_ENTER_Pin GPIO_PIN_0
#define BUT_DOWN_GPIO_Port GPIOE
#define BUT_DOWN_Pin GPIO_PIN_6
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
