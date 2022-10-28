/*
 * stm32f4xx_BlackPill.h
 *
 *  Created on: Oct 16, 2022
 *      Author: Jonatan
 */

#ifndef BSP_STM32F4XX_BLACKPILL_INC_STM32F4XX_BLACKPILL_H_
#define BSP_STM32F4XX_BLACKPILL_INC_STM32F4XX_BLACKPILL_H_

#ifdef __cplusplus
 extern "C" {
#endif

 /* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

 typedef enum
 {
   LED_USER=0,
 }Led_TypeDef;

 typedef enum
 {
   BUTTON_MODE_GPIO=0,
   BUTTON_MODE_EXTI
 }ButtonMode_TypeDef;

 typedef enum
 {
   BUTTON_USER=0
 }Button_TypeDef;

#define LEDn								1

#define LED_USER_PIN 						GPIO_PIN_13
#define LED_USER_GPIO_PORT 					GPIOC

#define LED_USER_GPIO_CLK_ENABLE()			__HAL_RCC_GPIOC_CLK_ENABLE();
#define LED_USER_GPIO_CLK_DISABLE()			__HAL_RCC_GPIOC_CLK_DISABLE();

#define BUTTONn                                 1

#define KEY_BUTTON_PIN          			GPIO_PIN_0
#define KEY_BUTTON_GPIO_PORT    			GPIOA
#define KEY_BUTTON_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOA_CLK_ENABLE()
#define KEY_BUTTON_GPIO_CLK_DISABLE()       __HAL_RCC_GPIOA_CLK_DISABLE()
#define KEY_BUTTON_EXTI_LINE                GPIO_PIN_0
#define KEY_BUTTON_EXTI_IRQn                EXTI0_IRQn

 /** @defgroup STM32F4XX_BLACKPILL_LOW_LEVEL_Exported_Functions STM32F4XX LOW LEVEL Exported Functions
   * @{
   */
void             BSP_LED_Init(Led_TypeDef Led);
void             BSP_LED_DeInit(Led_TypeDef Led);
void             BSP_LED_On(Led_TypeDef Led);
void             BSP_LED_Off(Led_TypeDef Led);
void             BSP_LED_Toggle(Led_TypeDef Led);
void             BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode);
void             BSP_PB_DeInit(Button_TypeDef Button);
uint32_t         BSP_PB_GetState(Button_TypeDef Button);

#endif /* BSP_STM32F4XX_BLACKPILL_INC_STM32F4XX_BLACKPILL_H_ */
