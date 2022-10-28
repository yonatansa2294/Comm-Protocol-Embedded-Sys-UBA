/**
 * @file 	Sim800x.h
 * @brief	This file uses the port.h file for the SIM initialization routines and in particular,
 * 			the UART handling functions for GSM/GPRS connectivity.
 * @version 1.0
 * @date	24/10/2022
 * @author	Yonatan Aguirre
 */

#ifndef SIM800X_INC_SIM800X_H_
#define SIM800X_INC_SIM800X_H_

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "port.h"

/**
 * @typedef	bool_t
 * @brief	A type definition for bool
 * */
typedef bool bool_t;

/*---- COMMUNICATION AND CONFIG SERIAL PORT SIM ----*/
/**
 * @def		AT_CMD_CHECK_COMM
 * @brief	Defines the prefix "AT" used to check communication with the SIM.
 * */
#define AT_CHECK_COMM				"AT"
#define AT_CMD_CONFIG_BAUD			"IPR"
#define DEFAULT_BAUD_RATE_SIM		"9600"
/*-------------------------------------------------*/

/*---------- STATUS RESPONSES --------*/
#define OK              0
#define ERROR           1
#define NO_RESPONSE     2
/*------------------------------------*/

/*----------------------------------- STATUS REGISTRATION NETWORK RESPONSES -------------------------------*/
#define AT_CMD_NETWORK_REGISTER					"AT+CREG?"
#define NETWORK_NOT_REGISTERED_NO_SEARCHING     ",0"
#define NETWORK_REGISTERED                     	",1"
#define NETWORK_NOT_REGISTERED_BUT_SEARCHING   	",2"
#define REGISTRATION_DENIED                    	",3"
#define UNKNOWN                                	",4"
#define REGISTERED_ROAMING                     	",5"
/*-------------------------------------------------------------------------------------------------------*/

/*-------------------------------- CALL -----------------------------*/
#define AT_CMD_CALL				"ATD"
#define AT_CMD_END_CALL         "ATH"
/*-------------------------------------------------------------------*/

/*------------------------------------------------------------ SMS -----------------------------------------------------*/
/**
 * @def		AT_CMD_LIST_SMS
 * @brief	Defines the the parameter of command used to list received messages according to their status:READ,UNREAD or
 * 			stored messages: SENT,UNSET.
 *
 * @def		AT_CMD_DELETE_SMS
 * @brief	Defines the the parameter of command used to delete received messages according to flag parameter [0-4]
 * 			which indicates which type of message to delete.
 *
 * @def		DELETE_ALL_SMS
 * @brief	Delete all messages from preferred message storage including unread messages
 *
 * @def		DELETE_ALL_READ_SENT_UNSET_SMS
 * @brief	Delete all read messages from preferred message storage, sent and unsent SIM originated messages
 *  		leaving unread messages untouched
 *
 * @def		DELETE_ALL_READ_SENT_SMS
 * @brief	Delete all read messages from preferred message storage and sent SIM originated messages,
 * 			leaving unread messages and unsent SIM originated messages untouched
 *
 * @def		DELETE_ALL_READ_SMS
 * @brief	Delete all read messages from preferred message storage, leaving unread messages and stored SIM
 * 			originated messages (whether sent or not) untouched
 *
 * @def		DELETE_SPECIFIC_SMS
 * @brief	Delete the message specified in <index>
 *
 * @def		INPUT_SMS
 * @brief	Defines the character returned by the AT+CMGS command, indicating the entry of the text to be sent.
 *
 * @def		CTRL_Z
 * @brief	Defines the character ASCII with which the text entry in the message is finalized and the message is sent.
 * */
#define AT_CMD_TEXT_MODE							"CMGF"
#define TEXT_MODE									"1"
#define AT_CMD_SEND_SMS        						"CMGS"
#define INPUT_DATA									'>'
#define CTRL_Z          							0x1A
#define AT_CMD_READ_SMS            					"CMGR"
#define AT_CMD_LIST_SMS            					"CMGL"
#define RECEIVED_UNREAD     						"\"REC UNREAD\""
#define RECEIVED_READ       						"\"REC READ\""
#define STORED_UNSENT       						"\"STO UNSENT\""
#define STORED_SENT         						"\"STO SENT\""
#define ALL                 						"\"ALL\""
#define AT_CMD_DELETE_SMS							"CMGD"
#define DELETE_ALL_SMS								4
#define DELETE_ALL_READ_SENT_UNSET_SMS				3
#define DELETE_ALL_READ_SENT_SMS					2
#define DELETE_ALL_READ_SMS							1
#define DELETE_SPECIFIC_SMS							0
#define OK_RESPONSE     							"OK"

/*-------------------------- GPRS ----------------------*/
/**
 * @def		CSTT
 * @brief	Defines the parameter of AT command used to set APN.
 *
 * @def		AT_CMD_DELETE_SMS
 * @brief	Defines the AT command used to delete received messages according to flag parameter [0-4]
 * 			which indicates which type of message to delete.
 *
 * @def		SHUT_OK
 * @brief	Defines the the response obtained by successfully disabling the GPRS PDP context.
 * */
#define AT_CMD_GPRS_SERVICE				"AT+CGATT?"
#define AT_CMD_DESACT_GPRS_CONTEXT		"AT+CIPSHUT"
#define CSTT            				"CSTT"
#define AT_CMD_BRING_UP_GPRS_CONEXION  	"AT+CIICR"
#define AT_CMD_GET_LOCAL_IP				"AT+CIFSR"
#define AT_CMD_START_TCPUDP_CONEXION	"CIPSTART"
#define AT_CMD_CLOSE_TCPUDP_CONEXION	"AT+CIPCLOSE"
#define AT_CMD_SEND_DATA_TCPUDP			"AT+CIPSEND"
#define CIPMODE         				"CIPMODE"
#define CONNECT_OK      				"CONNECT"
#define TCP             				"TCP"
#define UDP             				"UDP"
#define SEND_OK         				"SEND OK"
#define CLOSE_OK        				"CLOSE OK"
#define SHUT_OK         				"SHUT OK"
#define TRANSPARENT_MODE    			1
#define COMMAND_MODE        			0
/*------------------------------------------------------*/

/*--------------- State of GPRS Attachment ------------*/
#define GPRS_DETACHED   "CGATT: 0"
#define GPRS_ATTACHED   "CGATT: 1"
/*-----------------------------------------------------*/

/**
 * @def		LEN_AT_CMD_CONFIG
 * @brief	Defines the length of the array that stores the AT commands for SIM configuration: AT+IPR and AT+CMGF.
 *
 * @def		LEN_BUFFER_AUX_CALL
 * @brief	Defines the length of the array to form the AT command for voice call
 *
 * @def		LEN_FORMAT_CELL_NUMBER
 * @brief	Defines the length of the array to store the phone number including quotes and country code.
 * */
#define SERIAL_RESPONSE_BUFFER_SIZE  	100
#define LEN_BUFFER_AUX_CALL				18
#define LEN_AT_CMD_CONFIG				15
#define LEN_FORMAT_CELL_NUMBER			16
#define LEN_FORMAT_APN					20

/*--------------------------------------------- SIM initialization functions ----------------------------------------------------------*/
uint8_t SIM800_ConfigHW(UART_HandleTypeDef *uartHandler,Port_t powerKeyPort, uint16_t powerKeyPin,Port_t resetPort, uint16_t resetPin);
uint8_t SIM800_Default_ConfigHW(void);
uint8_t SIM800_Init(void);
void	SIM800_On(void);
void	SIM800_Off(void);
void	SIM800_restart(void);

/*------------------------- Functions for SMS ----------------------------------*/
uint8_t check_Network_Registration(void);
uint8_t send_SMS(uint8_t *cellNumber, uint8_t *message);
uint8_t list_Received_SMS_(const uint8_t *smsToSearch, const uint8_t *status);
uint8_t delete_SMS(uint8_t index,uint8_t flag);

/*----------- Functions for voice calls ---------*/
uint8_t call(uint8_t *cellNumber);
uint8_t endCall(void);

/*------------------------------------- Functions for GPRS ----------------------------------*/
uint8_t check_GPRS_Connection(void);
uint8_t disable_GPRS_PDP_Context(void);
uint8_t set_Application_Mode_TCPUDP(uint8_t tcpip_appMode);
uint8_t setAPN(uint8_t *apn);
uint8_t bring_Up_Wireless_Connection(void);
uint8_t start_Up_TCPUDP_Connection(uint8_t *connection, uint8_t *ip_address, uint8_t *port);
uint8_t close_TCPUDP_Connection(void);
uint8_t send_Data_TCPUDP ( uint8_t *data, uint8_t tcpip_appMode);
void 	enable_AT_CMD_In_Transparent_Mode(void);


#endif /* SIM800X_INC_SIM800X_H_ */
