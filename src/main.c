/**
 ******************************************************************************
 * @file    main.c
 * @author  Ac6
 * @version V1.0
 * @date    01-December-2013
 * @brief   Default main function.
 ******************************************************************************
 */

#include "stm32f1xx.h"
#include "main.h"
#include "tm1637.h"

/*
 * Ongoing Issues
 * 1. Display initialization fucked in current tm1637 lib if gcc optimizations enabled.
 * SOLUTION: Use different library
 * */

volatile uint16_t Seconds = 0;
volatile uint32_t tEncoderButtonPressedTicks = 0;
/**
 * f_States are the global flags
 * bit 0 - s_Timer status - counting or not.
 * bit 1 - flag indicating that display needs to be updated
 * bit 2 - flag indicating that the case is open
 * bit 3 - flag indicating that settings save is needed
 */
volatile uint8_t f_States = 0;
TIM_HandleTypeDef s_TimerEncoder = { .Instance = ENCODER_TIM };
TIM_HandleTypeDef s_Timer = { .Instance = TIM2 };

volatile SettingsTypeDef Settings[16];

int CleanFlashPage(uint32_t AddressToErase) {

	uint32_t  PageError = 0;

	FLASH_EraseInitTypeDef EraseInitStruct = { .TypeErase =
	FLASH_TYPEERASE_PAGES, .NbPages = 1 };

	EraseInitStruct.PageAddress = AddressToErase;

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) {
		return ERROR;
	}

	return SUCCESS;
}

void ReadSettings() {
	//Read settings

	uint64_t *source_addr = (uint64_t *) SETTINGS_PAGE_ADDR;
	uint64_t *dest_addr = (void *) &Settings;

	for (uint8_t i = 0; i < sizeof(Settings) / sizeof (SettingsTypeDef); i++) {
		*dest_addr = *(__IO uint64_t*) source_addr;
		source_addr++;
		dest_addr++;
	}
}

void WriteSettings() {

	HAL_FLASH_Unlock();

	if (CleanFlashPage(SETTINGS_PAGE_ADDR)) {

		// Write settings
		uint64_t *source_addr = (void *) &Settings;
		uint64_t *dest_addr = (uint64_t *) SETTINGS_PAGE_ADDR;

		for (uint8_t i = 0; i < sizeof(Settings) / sizeof (SettingsTypeDef); i++) {

			HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t)dest_addr,
					(uint64_t) * source_addr);
			source_addr++;
			dest_addr++;

		}

	}

	HAL_FLASH_Lock();
}
void InitializeButtons() {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	GPIO_InitStruct.Pin = BUTTON_1_PIN | BUTTON_2_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
void InitializeBuzzer() {

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* Set GPIOA Pin#3 Parameters */

	GPIO_InitStruct.Pin = BUZZER_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	/* Init GPIOA Pin#3 for Buzzer*/
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

void InitializeDrivers() {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* Set GPIOB Driver Pins Parameters */

	GPIO_InitStruct.Pin = UPPER_DRIVER_PIN | LOWER_DRIVER_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	/* Init GPIOB Driver Pins */
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}
void InitializeTimerEncoder() {

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	TIM_Encoder_InitTypeDef Encoder_InitStruct = { 0 };

	/* Set GPIOA Pin#6/7 Parameters */

	GPIO_InitStruct.Pin = ENCODER_UP | ENCODER_DOWN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	/* Init GPIOC Pin#6/7 for Encoder*/
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* Set GPIOA Pin#3 Parameters */

	GPIO_InitStruct.Pin = ENCODER_BUTTON;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	/* Init GPIOC Pin#3 for Encoder Button*/
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	s_TimerEncoder.Init.Period = 1600;
	s_TimerEncoder.Init.CounterMode = TIM_COUNTERMODE_UP;

	Encoder_InitStruct.EncoderMode = TIM_ENCODERMODE_TI12;

	Encoder_InitStruct.IC1Filter = 0x0f;
	Encoder_InitStruct.IC1Polarity = TIM_INPUTCHANNELPOLARITY_FALLING;
	Encoder_InitStruct.IC1Prescaler = TIM_ETRPRESCALER_DIV1;
	Encoder_InitStruct.IC1Selection = TIM_ICSELECTION_DIRECTTI;

	Encoder_InitStruct.IC2Filter = 0x0f;
	Encoder_InitStruct.IC2Polarity = TIM_INPUTCHANNELPOLARITY_FALLING;
	Encoder_InitStruct.IC2Prescaler = TIM_ETRPRESCALER_DIV1;
	Encoder_InitStruct.IC2Selection = TIM_ICSELECTION_DIRECTTI;

	if (HAL_TIM_Encoder_Init(&s_TimerEncoder, &Encoder_InitStruct) != HAL_OK) {
		while (1)
			;

	}

	if (HAL_TIM_Encoder_Start_IT(&s_TimerEncoder, TIM_CHANNEL_ALL) != HAL_OK) {
		while (1)
			;
	}
}


void InitializeTimer() {
	s_Timer.Init.Prescaler = 36000 - 1;
	s_Timer.Init.Period = 2000;
	s_Timer.Init.CounterMode = TIM_COUNTERMODE_DOWN;

	if (HAL_TIM_Base_Init(&s_Timer) != HAL_OK) {
		while (1)
			;

	}
}
void InitializeDisplay() {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* Set Display Pins Parameters */
	GPIO_InitStruct.Pin = CLK_Pin | DATA_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	/* Init GPIOC Pins for Display*/
	HAL_GPIO_Init(DISPLAY_GPIO_Port, &GPIO_InitStruct);

	TM1637_brightness(BRIGHT_TYPICAL);
	TM1637_displayTime(0, 0);
}

void InitializeInterrupts() {

	//HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 1);
	//HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	HAL_NVIC_SetPriority(EXTI1_IRQn, 1, 2);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);
	HAL_NVIC_SetPriority(TIM1_CC_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);
	HAL_NVIC_SetPriority(TIM2_IRQn, 2, 1);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

void SystemClock_Config(void) {

	RCC_ClkInitTypeDef clkinitstruct = { 0 };
	RCC_OscInitTypeDef oscinitstruct = { 0 };
	/* Configure PLLs------------------------------------------------------*/
	/* Enable HSE Oscillator and activate PLL with HSE as source */
	oscinitstruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	oscinitstruct.HSEState = RCC_HSE_ON;
	oscinitstruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	oscinitstruct.PLL.PLLState = RCC_PLL_ON;
	oscinitstruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	oscinitstruct.PLL.PLLMUL = RCC_PLL_MUL9;

	if (HAL_RCC_OscConfig(&oscinitstruct) != HAL_OK) { /* Initialization Error */
		while (1)
			;
	}
	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks
	 dividers */
	clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
	clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2) != HAL_OK) { /* Initialization Error */
		while (1)
			;
	}

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

	__HAL_RCC_GPIOA_CLK_ENABLE()
	;
	__HAL_RCC_GPIOB_CLK_ENABLE()
	;
	__HAL_RCC_TIM1_CLK_ENABLE()
	;
	__HAL_RCC_TIM2_CLK_ENABLE()
	;

}

int main(void) {

	HAL_Init();
	SystemClock_Config();
	InitializeTimerEncoder();
	InitializeTimer();
	InitializeDisplay();
	InitializeBuzzer();
	InitializeDrivers();
	InitializeButtons();
	InitializeInterrupts();

	ReadSettings();

	Seconds = Settings[0].Seconds;

	s_TimerEncoder.Instance->CNT = Seconds / 15 * 4;

	f_States |= 1UL << 1; //Display should be updated

	for (;;) {

 		if (f_States & 0x2) { // Display should be updated
			TM1637_displayTime(Seconds / 60, Seconds % 60);
			f_States &= ~(1UL << 1); // Clean update display flag as it already updated
		}
	};
}
