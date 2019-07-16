/**
 ******************************************************************************
 * @file    stm32f1xx_it.c
 * @author  Ac6
 * @version V1.0
 * @date    02-Feb-2015
 * @brief   Default Interrupt Service Routines.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "main.h"
#include "tm1637.h"
#ifdef USE_RTOS_SYSTICK
#include <cmsis_os.h>
#endif
#include "stm32f1xx_it.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

extern volatile uint16_t Seconds;
extern TIM_HandleTypeDef s_TimerEncoder;
extern TIM_HandleTypeDef s_Timer;
extern volatile uint8_t f_States;
extern volatile uint32_t tEncoderButtonPressedTicks;
extern volatile SettingsTypeDef Settings[16];

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            	  	    Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief  This function handles SysTick Handler, but only if no RTOS defines it.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void) {
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
#ifdef USE_RTOS_SYSTICK
	osSystickHandler();
#endif
}

void StopExposer() {

	HAL_GPIO_WritePin(GPIOB, UPPER_DRIVER_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, LOWER_DRIVER_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, BUZZER_PIN, GPIO_PIN_RESET);
	HAL_TIM_Base_Stop_IT(&s_Timer);
	f_States &= ~(1UL << 0); //Timer is not running, so set it's bit to zero in global variable

}

void StartExposer() {
	if (HAL_GPIO_ReadPin(GPIOA, BUTTON_1_PIN) == GPIO_PIN_RESET) { // We will start everything only if the case is closed
		if (!Seconds)
			Seconds = (s_TimerEncoder.Instance->CNT / 4) * 15;
		if (!Seconds)
			return; // Because nothing to start
		HAL_GPIO_WritePin(GPIOB, UPPER_DRIVER_PIN, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, LOWER_DRIVER_PIN, GPIO_PIN_SET);
		HAL_TIM_Base_Start_IT(&s_Timer);
		f_States |= 1UL << 0; //Timer is started, so set it's bit to one in global variable

	}

}

void HAL_SYSTICK_Callback(void) {

	if (!HAL_GPIO_ReadPin(GPIOA, ENCODER_BUTTON)) {

		if (tEncoderButtonPressedTicks++ > LONG_PRESS_TIME) { //Long press, so stop everything
			StopExposer();
			Seconds = 0;
			f_States |= 1UL << 1; //Display should be updated
			tEncoderButtonPressedTicks = 0;
		}

	} else {

		if (tEncoderButtonPressedTicks > DEBOUNCE_TIME && Seconds) { // We have at least DEBOUNCE_TIME ms button pressed, and it was not holdoff time during reset
			if (f_States & 0x1) {
				StopExposer();
			} else {
				if ( f_States & 0x8) {
					WriteSettings();
					f_States &= ~(1UL << 3); //Clear save settings bit
				}
				StartExposer();
			}
		}

		tEncoderButtonPressedTicks = 0;
	}

}

void TIM1_CC_IRQHandler(void) {
	HAL_TIM_IRQHandler(&s_TimerEncoder);
}

void TIM2_IRQHandler(void) {
	HAL_TIM_IRQHandler(&s_Timer);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM2) {
		if (--Seconds) {
			if (Seconds < 8) {
				HAL_GPIO_TogglePin(GPIOA, BUZZER_PIN);
			}
		} else {

			StopExposer();
		};

		f_States |= 1UL << 1; //Display should be updated

	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {

	if (htim->Instance == ENCODER_TIM && !(f_States & 0x1)) { //Update seconds only if timer is not running
		Seconds = (htim->Instance->CNT / 4) * 15;
		Settings[0].Seconds = Seconds; // Last settings should be autosaved
		f_States |= 0xA; //Display should be updated and settings saved
	};
}

void EXTI15_10_IRQHandler(void) {

	HAL_GPIO_EXTI_IRQHandler(EXTI->PR);

}

void EXTI1_IRQHandler(void) {

	HAL_GPIO_EXTI_IRQHandler(EXTI->PR);

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	/*if (GPIO_Pin == ENCODER_BUTTON) {

	 if (HAL_GPIO_ReadPin(GPIOA, GPIO_Pin) == GPIO_PIN_RESET) {

	 tEncoderButtonPressedTicks = HAL_GetTick();

	 } else {

	 TicksDiff = HAL_GetTick() - tEncoderButtonPressedTicks;

	 if ( TicksDiff > 50 && TicksDiff < 2000 ) {
	 if (f_States & 0x1) {
	 StopExposer();
	 } else {
	 StartExposer();
	 }
	 } else
	 tEncoderButtonPressedTicks = HAL_GetTick();
	 }

	 } else*/if (GPIO_Pin == BUTTON_1_PIN) {

		if (HAL_GPIO_ReadPin(GPIOA, BUTTON_1_PIN)) {

			f_States |= 1UL << 2;

		} else {

			f_States &= ~(1UL << 2);

		}

		if (f_States & 0x5) { // If case was opened, and timer was running we must stop everything

			StopExposer();

		} else if (!(f_States & 0x5) && Seconds) {
			//If case was closed, when timer was stopped, but we have some seconds left, we just
			// start everything back. It seems like someone peeked up into case earlier
			StartExposer();

		}
	}
}
;

