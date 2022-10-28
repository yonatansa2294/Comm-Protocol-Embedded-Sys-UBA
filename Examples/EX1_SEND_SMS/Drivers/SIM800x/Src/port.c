/**
 * @file 	port.c
 * @brief	This file presents the source code for the implementation of each function prototype
 * 			described in the port.h file.
 * @version 1.0
 * @date	24/10/2022
 * @author	Yonatan Aguirre
 */

#include "port.h"

static UART_HandleTypeDef	UARTHandlerPort;
static UART_HandleTypeDef	*pUARTHandlerPort;
static GPIO_TypeDef			*resetHandlerPort;
static GPIO_TypeDef			*powerKeyHandlerPort;
static uint16_t				_powerKeyPin;
static uint16_t				_resetPin;
static const uint16_t 		gpioPin[PINn] = {GPIO_PIN_0,GPIO_PIN_1,GPIO_PIN_2,GPIO_PIN_3,
											 GPIO_PIN_4,GPIO_PIN_5,GPIO_PIN_6,GPIO_PIN_7,
											 GPIO_PIN_8,GPIO_PIN_9,GPIO_PIN_10,GPIO_PIN_11,
											 GPIO_PIN_12,GPIO_PIN_13,GPIO_PIN_14,GPIO_PIN_15,
											};
static uint8_t				serialUARTBuffer[MAX_BUFFER_SIZE];


/**
 * @brief	Initializes the UART peripheral passed as a parameter for communication with the SIM800x.
 * @note	This function only works with USARTx other than USART1, because USART1 is used by default.
 * @param	UartHandle pointer to the UART_Handle structure.
 * @retval 	Initialization status. If the initialization was successful it returns true, otherwise it returns false.
 */
bool_t config_UART_SIM(UART_HandleTypeDef *uartHandler)
{
	bool_t initStatusUART;

	if(uartHandler != NULL)
	{
		pUARTHandlerPort = uartHandler;

		/* If the maximum baud rate is exceeded for USARTx, the default is 9600 baud */
		if(pUARTHandlerPort->Init.BaudRate > MAX_BAUD_RATE)
			pUARTHandlerPort->Init.BaudRate = DEFAULT_BAUD_RATE;

		/* Initializes USARTx asynchronous mode */
		if (HAL_UART_Init(pUARTHandlerPort) == HAL_OK)
			initStatusUART = SUCCESSFUL;
		else
			initStatusUART = UNSUCCESSFUL;
	}
	else
		initStatusUART = UNSUCCESSFUL;

	return initStatusUART;
}

/**
 * @brief	Set the default initialization parameters for the USART as well as for the reset and powerkey pins.
 * @note	The default parameters are as follows:
				USART		:	USART1, Baud rate:9600, Bits:8, Parity: None, Stop bits: 1
				PowerKey Pin: 	PIN_B0,	initial state: OFF
				Reset Pin	: 	PIN_B1, initial state: ON
			In addition, the SIM remains turn off and reset until the power_On_SIM function is used.
			This is because some modules do not have the powerKey pin, so we reset the SIM.
 * @param	None.
 * @retval 	Initialization status. If the initialization was successful it returns true, otherwise it returns false.
 */
bool_t config_Default_SIM()
{
	GPIO_InitTypeDef GPIO_InitStruct={0};
	bool_t initStatusHwSIM;

	/* Enable port B clock for powerKey and reset pins */
	DEFAULT_POWERKEY_GPIO_CLK_ENABLE();
	DEFAULT_RST_GPIO_CLK_ENABLE();

	/* Default pin assignments for reset and powerKey pins */
	_powerKeyPin = DEFAULT_POWERKEY_PIN;
	powerKeyHandlerPort = DEFAULT_POWERKEY_GPIO_PORT;

	_resetPin = DEFAULT_RST_PIN;
	resetHandlerPort = DEFAULT_RST_GPIO_PORT;

	/* Configure GPIO pin : Power Key */
	GPIO_InitStruct.Pin = _powerKeyPin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(powerKeyHandlerPort, &GPIO_InitStruct);
	HAL_GPIO_WritePin(powerKeyHandlerPort, _powerKeyPin, PWRKEY_OFF);

	/*Configure GPIO pin : Reset */
	GPIO_InitStruct.Pin = _resetPin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(resetHandlerPort, &GPIO_InitStruct);
	HAL_GPIO_WritePin(resetHandlerPort, _resetPin, RST_ON);

	/* Configuration parameters for USART1 peripherals */
	UARTHandlerPort.Instance = DEFAULT_USART;
	UARTHandlerPort.Init.BaudRate = DEFAULT_BAUD_RATE;
	UARTHandlerPort.Init.WordLength = UART_WORDLENGTH_8B;
	UARTHandlerPort.Init.StopBits = UART_STOPBITS_1;
	UARTHandlerPort.Init.Parity = UART_PARITY_NONE;
	UARTHandlerPort.Init.Mode = UART_MODE_TX_RX;
	UARTHandlerPort.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	UARTHandlerPort.Init.OverSampling = UART_OVERSAMPLING_16;

	pUARTHandlerPort = &UARTHandlerPort;

	/* USART1 initialization with default parameters */
	if (HAL_UART_Init(pUARTHandlerPort) == HAL_OK)
		initStatusHwSIM = SUCCESSFUL;
	else
		initStatusHwSIM = UNSUCCESSFUL;

	return initStatusHwSIM;
}

/**
 * @brief	Set the GPIO port and pin assigned to powerKey.
 * @param	GPIOx specified by an enumeration type Port_t, where x can be A|B|C
 * @param	GPIO_Pin specifies the port bit to be written, where Pin can be [0..15]
 * @retval 	None.
 */
void initPowerKeyPin(Port_t portX, uint16_t powerKeyPin)
{
	GPIO_InitTypeDef GPIO_InitStruct={0};

	/* Enable port clock for powerKey pin */
	switch(portX)
	{
		case PORTA:
			__HAL_RCC_GPIOA_CLK_ENABLE();
			powerKeyHandlerPort = GPIOA;
			break;

		case PORTB:
			__HAL_RCC_GPIOB_CLK_ENABLE();
			powerKeyHandlerPort = GPIOB;
			break;

		case PORTC:
			__HAL_RCC_GPIOC_CLK_ENABLE();
			powerKeyHandlerPort = GPIOC;
			break;
		default:
			DEFAULT_POWERKEY_GPIO_CLK_ENABLE();
			powerKeyHandlerPort = DEFAULT_POWERKEY_GPIO_PORT;
			break;
	}

	/* GPIO pin selection for powerKey from the gpioPin array  */
	_powerKeyPin =  gpioPin[powerKeyPin];

	/*Configure GPIO pin : Power Key_Pin */
	GPIO_InitStruct.Pin = _powerKeyPin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(powerKeyHandlerPort, &GPIO_InitStruct);
	/* SIM: Turn off*/
	HAL_GPIO_WritePin(powerKeyHandlerPort, _powerKeyPin, PWRKEY_OFF);
}

/**
 * @brief	Set the GPIO port and pin assigned to reset.
 * @param	GPIOx specified by an enumeration type Port_t, where x can be A|B|C
 * @param	GPIO_Pin specifies the port bit to be written, where Pin can be [0..15]
 * @retval 	None.
 */
void initResetPin(Port_t portX, uint16_t resetPin)
{
	GPIO_InitTypeDef GPIO_InitStruct={0};

	switch(portX)
	{
		case PORTA:
			__HAL_RCC_GPIOA_CLK_ENABLE();
			resetHandlerPort = GPIOA;
			break;

		case PORTB:
			__HAL_RCC_GPIOB_CLK_ENABLE();
			resetHandlerPort = GPIOB;
			break;

		case PORTC:
			__HAL_RCC_GPIOC_CLK_ENABLE();
			resetHandlerPort = GPIOC;
			break;
		default:
			DEFAULT_POWERKEY_GPIO_CLK_ENABLE();
			resetHandlerPort = DEFAULT_POWERKEY_GPIO_PORT;
			break;
	}
	/* GPIO pin selection for Reset from the gpioPin array  */
	_resetPin =  gpioPin[resetPin];

	/*Configure GPIO pin : Power Reset Pin */
	GPIO_InitStruct.Pin = _resetPin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(resetHandlerPort, &GPIO_InitStruct);
	/* SIM: Reset status*/
	HAL_GPIO_WritePin(resetHandlerPort, _resetPin, RST_ON);
}

/**
 * @brief	Turn on the SIM.
 * @note	The start-up sequence is as follows:
 * 				1. Hold the powerKey pin high for a time of at least 1s and then set it low.
 * 				This is specified in the Hardware Design Guide v1.9, page 22.
 * 				2. Wait a time of at least 15s for the SIM UART to be released.
 * 				On power-up, the SIM sends some initial status messages over the UART,
 * 				which are not relevant, so we wait for this time.
 * @param	None.
 * @retval 	None.
 */
void power_On_SIM()
{
	HAL_GPIO_WritePin(powerKeyHandlerPort, _powerKeyPin, PWRKEY_ON);
	HAL_Delay(DELAY_POWER_ON);
	HAL_GPIO_WritePin(powerKeyHandlerPort, _powerKeyPin, PWRKEY_OFF);
	HAL_Delay(DELAY_ENTRY_ACTIVE);

	/* Initializes the serial receive buffer */
	memset(&serialUARTBuffer,0,sizeof(serialUARTBuffer));
}

/**
 * @brief	Turn off the SIM.
 * @note	The start-up sequence is as follows:
 * 				1. Hold the powerKey pin high for at least 1.5s and then set it low.
 * 				This is specified in the Hardware Design Guide v1.9, page 23.
 * 				2. Wait a time of at least 3s for the SIM UART to to wait for the UART SIM goes to the idle state.
 * @param	None.
 * @retval 	None.
 */
void power_Down_SIM()
{
	HAL_GPIO_WritePin(powerKeyHandlerPort, _powerKeyPin, PWRKEY_ON);
	HAL_Delay(DELAY_POWER_DOWN);
	HAL_GPIO_WritePin(powerKeyHandlerPort, _powerKeyPin, PWRKEY_OFF);
	HAL_Delay(DELAY_ENTRY_IDLE);
}

/**
 * @brief	Reset the SIM.
 * @note	Resetting consists of holding the reset pin high for at least 105ms and then set it low.
 * 			This is specified in the Hardware Design Guide v1.9, page 25 - Table 6.
 * @param	None.
 * @retval 	None.
 */
void restart_SIM()
{
	memset(&serialUARTBuffer,0,sizeof(serialUARTBuffer));

	HAL_GPIO_WritePin(resetHandlerPort, _resetPin, RST_ON);
	HAL_Delay(DELAY_RESET);
	HAL_GPIO_WritePin(resetHandlerPort, _resetPin, RST_OFF);
}

/**
 * @brief	Keeps the SIM in a reset state
 * @param	None.
 * @retval 	None.
 */
void keep_Reset_SIM()
{
	HAL_GPIO_WritePin(resetHandlerPort, _resetPin, RST_ON);
}

/**
 * @brief	Releases the SIM from the permanent reset state
 * @param	None.
 * @retval 	None.
 */
void exit_Reset_SIM()
{
	HAL_GPIO_WritePin(resetHandlerPort, _resetPin, RST_OFF);
}

/**
 * @brief	Sends a string of data over the specified UART peripheral.
 * @param 	Pointer to the data string to be sent.
 * @retval 	None.
 */
void write_Data_UART(uint8_t *pDataTx)
{
	if(pDataTx!= NULL)	 /**< verifies that the data pointer is valid */
	{
		/* get the number of characters to send */
		uint16_t dataLength = strlen((const char*)pDataTx);
		HAL_UART_Transmit(pUARTHandlerPort,pDataTx, dataLength, TIMEOUT);
	}
}

/**
 * @brief  	Reads a byte over the UART and stores it in the serialUARTBuffer array.
 * @param 	None.
 * @retval 	The first element of the array serialUARTBuffer, containing the data byte read.
 */
uint8_t read_Data_UART()
{
	HAL_UART_Receive(pUARTHandlerPort,&serialUARTBuffer,1,TIMEOUT);
	return serialUARTBuffer[0];
}
