/*
 * stm32f4xx_BlackPill.c
 *
 *  Created on: Oct 16, 2022
 *      Author: Jonatan
 */

#include "stm32f4xx_blackpill.h"

GPIO_TypeDef* GPIO_PORT[LEDn] = {LED_USER_GPIO_PORT};
const uint16_t GPIO_PIN[LEDn] = {LED_USER_PIN};

GPIO_TypeDef* BUTTON_PORT[BUTTONn] = {KEY_BUTTON_GPIO_PORT};
const uint16_t BUTTON_PIN[BUTTONn] = {KEY_BUTTON_PIN};
const uint8_t BUTTON_IRQn[BUTTONn] = {KEY_BUTTON_EXTI_IRQn};

void BSP_LED_Init(Led_TypeDef Led)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	LED_USER_GPIO_CLK_ENABLE();

	/*Configure GPIO pin : USER_LED_Pin */
	GPIO_InitStruct.Pin = GPIO_PIN[Led];
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(GPIO_PORT[Led], &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_SET);
}

void BSP_LED_DeInit(Led_TypeDef Led)
{
	GPIO_InitTypeDef  gpio_init_structure;

	/* GPIO Ports Clock Disable */
	LED_USER_GPIO_CLK_DISABLE();

	/* Turn off LED */
	HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_RESET);
	/* DeInit the GPIO_LED pin */
	gpio_init_structure.Pin = GPIO_PIN[Led];
	HAL_GPIO_DeInit(GPIO_PORT[Led], gpio_init_structure.Pin);
}

void BSP_LED_On(Led_TypeDef Led)
{
	HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_RESET);
}

void BSP_LED_Off(Led_TypeDef Led)
{
	HAL_GPIO_WritePin(GPIO_PORT[Led], GPIO_PIN[Led], GPIO_PIN_SET);
}

void BSP_LED_Toggle(Led_TypeDef Led)
{
	HAL_GPIO_TogglePin(GPIO_PORT[Led], GPIO_PIN[Led]);
}

void BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* Enable the BUTTON Clock */
	KEY_BUTTON_GPIO_CLK_ENABLE();

	if(ButtonMode == BUTTON_MODE_GPIO)
	{
	/* Configure Button pin as input */
	GPIO_InitStruct.Pin = BUTTON_PIN[Button];
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(BUTTON_PORT[Button], &GPIO_InitStruct);
	}

	if(ButtonMode == BUTTON_MODE_EXTI)
	{
	/* Configure Button pin as input with External interrupt */
	GPIO_InitStruct.Pin = BUTTON_PIN[Button];
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	HAL_GPIO_Init(BUTTON_PORT[Button], &GPIO_InitStruct);

	/* Enable and set Button EXTI Interrupt to the lowest priority */
	HAL_NVIC_SetPriority((IRQn_Type)(BUTTON_IRQn[Button]), 0x0F, 0x00);
	HAL_NVIC_EnableIRQ((IRQn_Type)(BUTTON_IRQn[Button]));
	}
}

void BSP_PB_DeInit(Button_TypeDef Button)
{
	GPIO_InitTypeDef gpio_init_structure;

	gpio_init_structure.Pin = BUTTON_PIN[Button];
	HAL_NVIC_DisableIRQ((IRQn_Type)(BUTTON_IRQn[Button]));
	HAL_GPIO_DeInit(BUTTON_PORT[Button], gpio_init_structure.Pin);
}

uint32_t BSP_PB_GetState(Button_TypeDef Button)
{
	return HAL_GPIO_ReadPin(BUTTON_PORT[Button], BUTTON_PIN[Button]);
}

