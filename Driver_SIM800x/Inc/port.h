/**
 * @file 	port.h
 * @brief	This file uses the HAL functions, for the configuration of the hardware required
 * 			for SIM800 handling: UART and GPIO.
 * @version 1.0
 * @date	24/10/2022
 * @author	Yonatan Aguirre
 */

#ifndef SIM800X_INC_PORT_H_
#define SIM800X_INC_PORT_H_

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "stm32f4xx_hal.h"

/**
 * @typedef	bool_t
 * @brief	A type definition for bool
 * */
typedef bool bool_t;

/**
 * @enum	Port_t
 * @brief	Type of enumeration for BlackPill ports
 * */
typedef enum
{
	PORTA = 0,
	PORTB = 1,
	PORTC = 2
}Port_t;

/**
 * @def		DELAY_POWER_ON
 * @brief	Defines the minimum time the powerKey pin must be high to turn on the SIM.
 *
 * @def		DELAY_POWER_DOWN
 * @brief	Defines the minimum time the powerKey pin must be high to turn off the SIM.
 *
 * @def		DELAY_ENTRY_ACTIVE
 * @brief	Defines the time to wait until the SIM stops sending startup messages over the UART.
 *
 * @def		DELAY_ENTRY_IDLE
 * @brief	Defines the time to wait until the UART goes to the idle state.
 *
 * @def		DELAY_RESET
 * @brief	Defines the minimum time the reset pin must be high to restart the SIM
 *
 * @def		PINn
 * @brief	Defines the number of pins available per port
 * */
#define DEFAULT_POWERKEY_PIN						GPIO_PIN_0
#define DEFAULT_POWERKEY_GPIO_PORT					GPIOB
#define DEFAULT_POWERKEY_GPIO_CLK_ENABLE()			__HAL_RCC_GPIOB_CLK_ENABLE();

#define DEFAULT_RST_PIN								GPIO_PIN_1
#define DEFAULT_RST_GPIO_PORT						GPIOB
#define DEFAULT_RST_GPIO_CLK_ENABLE()				__HAL_RCC_GPIOB_CLK_ENABLE();

#define PINn										16U
#define PWRKEY_ON									GPIO_PIN_SET
#define PWRKEY_OFF									GPIO_PIN_RESET
#define RST_ON										GPIO_PIN_RESET
#define RST_OFF										GPIO_PIN_SET
#define DELAY_POWER_ON								1500L
#define DELAY_ENTRY_ACTIVE							15000L
#define DELAY_POWER_DOWN							2000L
#define DELAY_ENTRY_IDLE							3000L
#define DELAY_RESET									200L

#define DEFAULT_USART								USART1
#define DEFAULT_BAUD_RATE							9600LU
#define MAX_BAUD_RATE								115200LU
#define TIMEOUT										1000L
#define MAX_BUFFER_SIZE								50U

#define SUCCESSFUL 									true
#define UNSUCCESSFUL 								false

bool_t 			config_UART_SIM(UART_HandleTypeDef *uartHandler);
bool_t 			config_Default_SIM();
void 			initPowerKeyPin(Port_t portX, uint16_t powerKeyPin);
void 			initResetPin(Port_t portX, uint16_t resetPin);
void			power_On_SIM();
void			power_Down_SIM();
void			restart_SIM();
void 			keep_Reset_SIM();
void 			exit_Reset_SIM();
void 			write_Data_UART(uint8_t *pDataTx);
uint8_t			read_Data_UART();

#endif /* SIM800X_INC_PORT_H_ */
