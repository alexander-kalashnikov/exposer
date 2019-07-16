/**
 ******************************************************************************
 * File Name          : main.h
 * Description        : This file contains the common defines of the application
 ******************************************************************************
 ** This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether 
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * COPYRIGHT(c) 2018 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H
/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

typedef struct
{
	uint16_t Seconds;
	uint16_t reservedFooter16;
	uint32_t reserverFooter32;

} SettingsTypeDef;

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/

#define CLK_Pin GPIO_PIN_11
#define DISPLAY_GPIO_Port GPIOA
#define DATA_Pin GPIO_PIN_12

#define ENCODER_TIM TIM1
#define ENCODER_BUTTON GPIO_PIN_10
#define ENCODER_UP GPIO_PIN_8
#define ENCODER_DOWN GPIO_PIN_9

#define BUZZER_PIN  GPIO_PIN_3

#define LOWER_DRIVER_PIN  GPIO_PIN_14
#define UPPER_DRIVER_PIN  GPIO_PIN_15

#define BUTTON_1_PIN GPIO_PIN_1
#define BUTTON_2_PIN GPIO_PIN_2

#define TICKS_DIV 128000

#define LONG_PRESS_TIME 2000
#define DEBOUNCE_TIME 50


#define SETTINGS_PAGE_ADDR 0x0800FC00

/* USER CODE BEGIN Private defines */
#define Set_CLK_Pin() HAL_GPIO_WritePin(DISPLAY_GPIO_Port,CLK_Pin,GPIO_PIN_SET)
#define Reset_CLK_Pin() HAL_GPIO_WritePin(DISPLAY_GPIO_Port,CLK_Pin,GPIO_PIN_RESET)
#define Set_DATA_Pin() HAL_GPIO_WritePin(DISPLAY_GPIO_Port,DATA_Pin,GPIO_PIN_SET)
#define Reset_DATA_Pin() HAL_GPIO_WritePin(DISPLAY_GPIO_Port,DATA_Pin,GPIO_PIN_RESET)
#define Read_DATA_Pin() HAL_GPIO_ReadPin(DISPLAY_GPIO_Port,DATA_Pin)
/* USER CODE END Private defines */

void _Error_Handler(char *, int);
void WriteSettings();

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)

/**
 * @}
 */

/**
 * @}
 */

#endif /* __MAIN_H */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
