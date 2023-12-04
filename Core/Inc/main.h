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
#include "stm32l4xx_hal.h"

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
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define SM_Pin GPIO_PIN_2
#define SM_GPIO_Port GPIOC
#define MB_Pin GPIO_PIN_3
#define MB_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define BTN2_Pin GPIO_PIN_4
#define BTN2_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define D2_Pin GPIO_PIN_6
#define D2_GPIO_Port GPIOA
#define D3_Pin GPIO_PIN_7
#define D3_GPIO_Port GPIOA
#define BTN1_Pin GPIO_PIN_5
#define BTN1_GPIO_Port GPIOC
#define C2_Pin GPIO_PIN_1
#define C2_GPIO_Port GPIOB
#define C1_Pin GPIO_PIN_2
#define C1_GPIO_Port GPIOB
#define F4_Pin GPIO_PIN_11
#define F4_GPIO_Port GPIOB
#define F3_Pin GPIO_PIN_12
#define F3_GPIO_Port GPIOB
#define C4_Pin GPIO_PIN_14
#define C4_GPIO_Port GPIOB
#define C3_Pin GPIO_PIN_15
#define C3_GPIO_Port GPIOB
#define SCLK_Pin GPIO_PIN_8
#define SCLK_GPIO_Port GPIOA
#define SDI_Pin GPIO_PIN_9
#define SDI_GPIO_Port GPIOA
#define F2_Pin GPIO_PIN_11
#define F2_GPIO_Port GPIOA
#define F1_Pin GPIO_PIN_12
#define F1_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define LCLK_Pin GPIO_PIN_5
#define LCLK_GPIO_Port GPIOB
#define D4_Pin GPIO_PIN_6
#define D4_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
