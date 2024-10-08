/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_0
#define LED_GPIO_Port GPIOC
#define UP_Pin GPIO_PIN_1
#define UP_GPIO_Port GPIOC
#define DOWN_Pin GPIO_PIN_2
#define DOWN_GPIO_Port GPIOC
#define ON_Pin GPIO_PIN_4
#define ON_GPIO_Port GPIOA
#define T3_Pin GPIO_PIN_5
#define T3_GPIO_Port GPIOA
#define SEL_Pin GPIO_PIN_6
#define SEL_GPIO_Port GPIOA
#define BACKLIGHT_Pin GPIO_PIN_7
#define BACKLIGHT_GPIO_Port GPIOA
#define T2_Pin GPIO_PIN_6
#define T2_GPIO_Port GPIOC
#define T1_Pin GPIO_PIN_7
#define T1_GPIO_Port GPIOC
#define LCD_RD_Pin GPIO_PIN_8
#define LCD_RD_GPIO_Port GPIOC
#define LCD_WR_Pin GPIO_PIN_9
#define LCD_WR_GPIO_Port GPIOC
#define LCD_RS_Pin GPIO_PIN_10
#define LCD_RS_GPIO_Port GPIOC
#define LCD_CS_Pin GPIO_PIN_11
#define LCD_CS_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
