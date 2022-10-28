/**
 * @file 	Sim800x.c
 * @brief	This file presents the source code for the implementation of each function prototype
 * 			described in the SIM800x.h file.
 * @version 1.0
 * @date	24/10/2022
 * @author	Yonatan Aguirre
 */

#include "SIM800x.h"

/*---------------------- Private variables -----------------------*/
static uint8_t serialResponseBuffer[SERIAL_RESPONSE_BUFFER_SIZE];

/*--------------------- Prototypes of private functions ----------------------*/
static void send_Test_Read_AT_CMD( uint8_t* command);
static void send_Write_Execution_AT_CMD( uint8_t* command, uint8_t* value);
static void read_Response_SIM(uint8_t *pBuffer);
static void read_Response_AT_Command_CIPSTART(uint8_t *pBuffer);

/**
 * @brief	Configures the UART, powerKey and reset pins all user-defined.
 * @param	Pointer to UART_HandleTypeDef UART_Handle structure.
 * @param	GPIO Port for powerKey
 * @param	GPIO Pin for powerKey
 * @param	GPIO Port for reset
 * @param	GPIO Pin for reset
 * @retval	Integer Value: OK(0) - ERROR(1)
 */
uint8_t SIM800_ConfigHW(UART_HandleTypeDef *uartHandler,Port_t powerKeyPort, uint16_t powerKeyPin,Port_t resetPort, uint16_t resetPin)
{
	uint8_t statusConfigSIM = ERROR;

	if(config_UART_SIM(uartHandler) == SUCCESSFUL)
	{
		initPowerKeyPin(powerKeyPort,powerKeyPin);
		initResetPin(resetPort,resetPin);
		statusConfigSIM = OK;
	}

	return statusConfigSIM;
}

/**
 * @brief	Configures the UART and powerKey and reset pins by default.
 * @note	The UART1 is used by default.
 * @param	Pointer to structure UART_HandleTypeDef
 * @retval	Integer Value: OK(0) - ERROR(1)
 */
uint8_t SIM800_Default_ConfigHW()
{
	uint8_t statusConfigSIM = ERROR;

	if(config_Default_SIM() == SUCCESSFUL)
		statusConfigSIM = OK;

	return statusConfigSIM;
}

/**
 * @brief	Configure the SIM to work in text mode and with a baud rate of 9600 baud.
 * @note	This function implements an auto-bauding routine as this is how the SIM is configured in the factory.
 * 		The serial interface has to be operated at 8 data bits, no parity and 1 stop bit.
 * 		The AT commands that are executed in this function are:
 * 			1. AT+IPR=9600	-	Set baud rate to 9600
 * 			2. AT+CMGF=1	-	Set the message system to text mode
 * @param	None
 * @retval Integer Value: OK(0) - ERROR(1)
 */
uint8_t SIM800_Init()
{
	uint8_t iterTest;
	uint8_t counterOK = 0;
	uint8_t statusInit;
	uint8_t nTimesAT = 5;

	memset(&serialResponseBuffer, 0,SERIAL_RESPONSE_BUFFER_SIZE);

	/* Auto-bauding routine
	 * To allow the baud rate to be synchronized, simply issue an "AT" string.
	 * In this case we send 5 times "AT" and check if the SIM response is OK.
	 * In case we get at least 4 correct answers, we validate the communication successfully.
	 * */
	for(iterTest = 0; iterTest < nTimesAT; iterTest++)
	{
		send_Test_Read_AT_CMD((uint8_t *)AT_CHECK_COMM);
		read_Response_SIM(&serialResponseBuffer);

		if(strstr((char *)serialResponseBuffer,(char *)OK_RESPONSE))
			counterOK++;
		HAL_Delay(100);
	}

	if(counterOK >= nTimesAT-1)
	{
		send_Write_Execution_AT_CMD((uint8_t *)AT_CMD_TEXT_MODE,(uint8_t *)TEXT_MODE);
		read_Response_SIM(&serialResponseBuffer);

		if(strstr((char *)serialResponseBuffer,(char *)OK_RESPONSE))
		{
			send_Write_Execution_AT_CMD((uint8_t *)AT_CMD_CONFIG_BAUD,(uint8_t *)DEFAULT_BAUD_RATE_SIM);
			read_Response_SIM(&serialResponseBuffer);

			if(strstr((char *)serialResponseBuffer,(char *)OK_RESPONSE))
				statusInit = OK;
			else
				statusInit = ERROR;
		}
	}
	else
		statusInit = ERROR;

	return statusInit;
}

/**
 * @brief	Sends via the UART the AT command that arrives as a parameter.
 * @note	This function is used for AT commands that query the current value of the parameter(s)
 * 			or to query the list of parameters and value ranges set with the corresponding write command.
 * 			Command format: AT+<command>=? or AT+<command>?
 * @param	Pointer of type uint8_t containing the AT command
 * @retval 	None
 */
static void send_Test_Read_AT_CMD( uint8_t* command)
{
    write_Data_UART(command);
    write_Data_UART((uint8_t *)"\r");
}

/**
 * @brief	It sends over the UART the AT command together with the configuration parameter.
 * @note 	This function is used for AT commands that sets the user-definable parameter values or when
 * 			execution command reads non-variable parameters.
 * 			Command format: AT+<command>=<value> or AT+<command>
 * @param	Pointer of type uint8_t containing the AT command
 * @param	Pointer of type uint8_t containing the parameter.
 * @retval	None
 */
static void send_Write_Execution_AT_CMD( uint8_t* command, uint8_t* value)
{
	write_Data_UART((uint8_t *)"AT+");
	write_Data_UART((uint8_t *)command);
	write_Data_UART((uint8_t *)"=");
	write_Data_UART((uint8_t *)value);
	write_Data_UART((uint8_t *)"\r");
}

/**
 * @brief	Check if the SIM is registered in the GSM network.
 * @note	AT command used: AT+CREG
 * @param	None.
 * @retval	Integer Value:
 * 			OK(0) - SIM is registered in the network.
 * 			ERROR(1) - SIM is not registered in the network.
 */
uint8_t check_Network_Registration(void)
{
	uint8_t statusReg = ERROR;

	memset(&serialResponseBuffer,0,SERIAL_RESPONSE_BUFFER_SIZE);

	send_Test_Read_AT_CMD((uint8_t*)AT_CMD_NETWORK_REGISTER);
    read_Response_SIM(&serialResponseBuffer);

    if(strstr((char *)serialResponseBuffer,(char *)NETWORK_REGISTERED))
    	statusReg = OK;

    return statusReg;
}

/**
 * @brief	Sends a text message to a user-specified number
 * @note	AT command used: AT+CMGS
 * @parm	Pointer to buffer of type uint8_t containing the cell number to which the message is to be sent.
 * @parm	Pointer to buffer of type uint8_t containing the text to send
 * @retval	Integer Value:
 * 			OK(0) - Message sent
 * 			ERROR(1) - Message not sent
 */
uint8_t send_SMS(uint8_t *cellNumber, uint8_t *message)
{
    uint8_t statusSendSMS;
    uint8_t formatCellNumber[LEN_FORMAT_CELL_NUMBER];
    uint8_t ctrlz[2];

	if((cellNumber != NULL) && (message != NULL))
	{
        memset(&serialResponseBuffer,0,SERIAL_RESPONSE_BUFFER_SIZE);

        /* Format the number to add quotation marks: "++xxxxxxxxxxxx" */
		sprintf((char *)formatCellNumber,"\"%*s\"",sizeof(cellNumber),cellNumber);
		send_Write_Execution_AT_CMD((uint8_t *)AT_CMD_SEND_SMS,formatCellNumber);
        read_Response_SIM(&serialResponseBuffer);

        /* Checks if the sim returned the '>' character to enter the text to be sent */
        if(strstr((char *)serialResponseBuffer,(char *)INPUT_DATA))
        {
        	memset(&serialResponseBuffer,0,SERIAL_RESPONSE_BUFFER_SIZE);

        	/* Enter the text and send with the ASCII character CTRLZ(0x1A)*/
        	write_Data_UART((uint8_t *)message);
        	HAL_Delay(300);
        	sprintf((char *)ctrlz,"%c",CTRL_Z);
        	write_Data_UART(ctrlz);

        	/* Verify the SIM response to validate the sending of the message */
        	read_Response_SIM(&serialResponseBuffer);
        	if(strstr((char *)serialResponseBuffer,(char *)OK_RESPONSE))
        		statusSendSMS = OK;
			else
				statusSendSMS = ERROR;
        }
        else
        	statusSendSMS = ERROR;
    }
	else
		statusSendSMS = ERROR;

	return statusSendSMS;
}

/**
 * @brief	This function searches for the text specified in the message parameter,
 * 		in the message(s) stored in the SIMCARD SMS message list using the storage filter specified
 * 		by the status parameter.
 * @detail	There should only be 1 unread message when using the RECEIVED UNREAD status.
 * 		AT command used: AT+CMGL
 * @parm	Pointer to buffer of type uint8_t containing the text to be searched in the SMS message
 * @parm	Pointer to buffer of type uint8_t containing the status parameter - pag.109 AT Command SIM800
 * @retval	Integer Value:
 * 			OK(0) - Message found
 * 			ERROR(1) - Message not found
 */
uint8_t list_Received_SMS_(const uint8_t *smsToSearch,const uint8_t *status)
{
    uint8_t statusRxSMS = ERROR;
	memset(&serialResponseBuffer, 0,SERIAL_RESPONSE_BUFFER_SIZE);

    send_Write_Execution_AT_CMD((uint8_t *)AT_CMD_LIST_SMS,(uint8_t *)status);
    read_Response_SIM(&serialResponseBuffer);

    if(strstr((char *)serialResponseBuffer,(char *)smsToSearch))
    	statusRxSMS = OK;

    return statusRxSMS;
}

/**
 * @brief	Delete SMS Message from preferred message storage.
 * @note	AT command used: AT+CMGD
 * @parm	Integer value of type uint8_t indicating the position of the message to be deleted.
 * @parm	Integer value of type uint8_t indicating the indicating the criteria for deleting messages.
 * @retval	Integer Value:
 * 			OK(0) - Message deleted
 * 			ERROR(1) - Error deleting message
 */
uint8_t delete_SMS(uint8_t index,uint8_t flag)
{
	uint8_t statusDeleteSMS = ERROR;
	uint8_t value[4];

	memset(&value, 0,sizeof(value));
	memset(&serialResponseBuffer, 0,SERIAL_RESPONSE_BUFFER_SIZE);


	sprintf((char *)value,"%i,%i",index,flag);
	send_Write_Execution_AT_CMD((uint8_t *)AT_CMD_DELETE_SMS,(uint8_t *)value);
	read_Response_SIM(&serialResponseBuffer);

	if(strstr((char *)serialResponseBuffer,(char *)OK_RESPONSE))
		statusDeleteSMS = OK;

	return statusDeleteSMS;
}

/**
 * @brief	Starts a voice call to the number specified by the parameter cellNumber
 * @note	AT command used: ATD
 * @param	Pointer to buffer of type uint8_t containing the cell number
 * @retval	Integer Value:
 * 			OK(0) - Call started
 * 			ERROR(1) - Error starting call
 */
uint8_t call(uint8_t *cellNumber)
{
    uint8_t bufferAuxCall[LEN_BUFFER_AUX_CALL];
    uint8_t statusCall = ERROR;

    /* checks that cellNumber is not null*/
    if(cellNumber != NULL)
    {
    	memset(&serialResponseBuffer,0,SERIAL_RESPONSE_BUFFER_SIZE);

    	/* Form AT command for call: ATD+xxxxxxxxxxxx;*/
    	sprintf((char *)bufferAuxCall,"%*s%*s;",sizeof(AT_CMD_CALL),AT_CMD_CALL,sizeof(cellNumber),cellNumber);
        send_Test_Read_AT_CMD(&bufferAuxCall);
        read_Response_SIM(&serialResponseBuffer);

        if(strstr((char *)serialResponseBuffer,(char *)OK_RESPONSE))
        	statusCall = OK;
    }

    return statusCall;
}

/**
 * @brief	Ends a voice call in progress
 * @note	AT command used: ATH
 * @param	None.
 * @retval	Integer Value:
 * 			OK(0) - Call ended
 * 			ERROR(1) - Error ending call
 */
uint8_t endCall(void)
{

	uint8_t statusEndCall=ERROR;

	memset(&serialResponseBuffer,0,SERIAL_RESPONSE_BUFFER_SIZE);

	send_Test_Read_AT_CMD((uint8_t *)AT_CMD_END_CALL);
    read_Response_SIM(&serialResponseBuffer);
    if(strstr((char *)serialResponseBuffer,(char *)OK_RESPONSE))
    	statusEndCall = OK;

    return statusEndCall;
}

/**
 * @brief	Check if the SIM is connected to the GPRS service.
 * @note	AT command used: AT+CGATT
 * @param	None.
 * @retval	Integer Value:
 * 			OK(0) - Connected to GPRS service.
 * 			ERROR(1) - Disconnected to GPRS service.
 */
uint8_t check_GPRS_Connection(void){

	uint8_t statusGPRS=ERROR;

	memset(&serialResponseBuffer,0,SERIAL_RESPONSE_BUFFER_SIZE);

    send_Test_Read_AT_CMD((uint8_t *)AT_CMD_GPRS_SERVICE);
    read_Response_SIM(&serialResponseBuffer);

    /* Search for text "CGATT: 1" in serialResponseBuffer */
    if(strstr((char *)serialResponseBuffer,(char *)GPRS_ATTACHED))
    	statusGPRS = OK;

    return statusGPRS;
}

/**
 * @brief	Deactivate GPRS PDP(Packet Data Protocol) context.
 * @note	After it is closed, the status is IP INITIAL
 * 			AT command used: AT+CIPSHUT
 * @param	None.
 * @retval	Integer Value:
 * 			OK(0) - IP logging successfully disabled.
 * 			ERROR(1) - Error when trying to disable context.
 */
uint8_t disable_GPRS_PDP_Context(void) {
    uint8_t statusShut = ERROR;

    memset(&serialResponseBuffer,0,SERIAL_RESPONSE_BUFFER_SIZE);

    send_Test_Read_AT_CMD((uint8_t*)AT_CMD_DESACT_GPRS_CONTEXT);
    read_Response_SIM(&serialResponseBuffer);

    /* Search for text "CLOSE OK" in serialResponseBuffer */
    if(strstr((char *)serialResponseBuffer,(char *)SHUT_OK))
    	statusShut = OK;

    return statusShut;
}

/**
 * @brief	Configure the SIM in command mode or transparent mode.
 * @note	When connected to a server, in "transparent" mode all data received by the UART is sent directly to the server,
 * 			so the use of AT commands is not allowed.
 * 			In case you want to enable AT commands use the function enable_AT_CMD_In_Transparent_Mode().
 * 			AT command used: AT+CIPMODE
 * @param	Application mode: COMMAND or TRANSPARENT.
 * @retval	Integer Value:
 * 			OK(0) - Application mode configured correctly.
 * 			ERROR(1) - Error while trying to configure the application mode.
 */
uint8_t set_Application_Mode_TCPUDP(uint8_t tcpip_appMode) {

    uint8_t  statusCmdMode = ERROR;

    if(tcpip_appMode == COMMAND_MODE){
    	send_Write_Execution_AT_CMD((uint8_t*)CIPMODE,(uint8_t*)"0");
        read_Response_SIM(&serialResponseBuffer);
    }
    else if(tcpip_appMode == TRANSPARENT_MODE){
    	send_Write_Execution_AT_CMD((uint8_t*)CIPMODE,(uint8_t*)"1");
        read_Response_SIM(&serialResponseBuffer);
    }
    if(strstr((char *)serialResponseBuffer,(char *)OK_RESPONSE))
    	statusCmdMode = OK;

    return statusCmdMode;
}

/**
 * @brief	Configure the APN (Access Point Name).
 * @note	AT command used: AT+CSTT
 * 			The write command and execution command of this command is valid only at the
 * 			state of IP INITIAL. So it is necessary to execute the function disable_GPRS_PDP_Context()
 * 			before user establishes a TCP/UDP connection with this command when the state is not IP INITIAL or IP STATUS.
 * 			After this command is executed, the state will be changed to IP START.
 * @param	Pointer to buffer of type uint8_t containing the APN of the mobile operator.
 * 			The max length is 50 bytes for APN
 * @retval	Integer Value:
 * 			OK(0) - APN configured correctly.
 * 			ERROR(1) - Error when configuring APN.
 */
uint8_t setAPN(uint8_t *apn) {

    uint8_t formatAPN[LEN_FORMAT_CELL_NUMBER];
    uint8_t statusAPN = ERROR;

    memset(&serialResponseBuffer,0,SERIAL_RESPONSE_BUFFER_SIZE);

    /* Format the APN to add quotation marks: "APN" */
    sprintf((char *)formatAPN,"\"%*s\"",strlen(apn),apn);
    send_Write_Execution_AT_CMD((uint8_t*)CSTT,formatAPN);
    read_Response_SIM(&serialResponseBuffer);

    if(strstr((char *)serialResponseBuffer,(char *)OK_RESPONSE))
    	statusAPN = OK;

    return statusAPN;
}

/**
 * @brief	Bring up wireless connection with GPRS or CSD.
 * @note	AT command used: AT+CIICR
 * 			AT+CIICR will change to IP CONFIG status, only if the status was previously IP START (APN configured).
 * 			After module accepts the activated operation, if it is activated successfully,
 * 			module state will be changed to IP GPRSACT, and it responds OK, otherwise it will respond ERROR.
 * @param	None.
 * @retval	Integer Value:
 * 			OK(0) - wireless connection successful.
 * 			ERROR(1) - Error connecting.
 */
uint8_t bring_Up_Wireless_Connection(void){

    uint8_t  statusGprsConnection =  ERROR;

    memset(&serialResponseBuffer,0,SERIAL_RESPONSE_BUFFER_SIZE);

    send_Test_Read_AT_CMD((uint8_t *)AT_CMD_BRING_UP_GPRS_CONEXION);
    read_Response_SIM(&serialResponseBuffer);

    if(strstr((char *)serialResponseBuffer,(char *)OK_RESPONSE))
    	statusGprsConnection = OK;

    return statusGprsConnection;
}

/**
 * @brief	Get Local IP Address and Start Up TCP/UDP Connection.
 * @note	AT command used: AT+CIFSR
 * 			Only after PDP context is activated, local IP address can be obtained by
			this command, otherwise it will respond ERROR.
 * 			AT command used: AT+CIPSTAR
 * 			This command allows establishment of a TCP/UDP connection only when there is a local IP address.
 * 			To check the status you can use the function send_Test_Read_AT_CMD and send as parameter "CIPSTATUS".
 * @param	Pointer of type uint8_t containing the connection type: TCP  or UDP.
 * @param	Pointer to buffer of type uint8_t containing the remote server IP address
 * @param	Pointer of type uint8_t containing the remote server port
 * @retval	Integer Value:
 * 			OK(0) - TCP/UDP connection successful.
 * 			ERROR(1) - Error connecting.
 */
uint8_t start_Up_TCPUDP_Connection(uint8_t *connection, uint8_t *ip_address, uint8_t *port)
{

    uint8_t stringAux[40];
    uint8_t statusTcpUdpConnection=ERROR;

    memset(&serialResponseBuffer,0,SERIAL_RESPONSE_BUFFER_SIZE);

    /* Get local IP address*/
    send_Test_Read_AT_CMD((uint8_t*)AT_CMD_GET_LOCAL_IP);

    memset(&serialResponseBuffer,0,SERIAL_RESPONSE_BUFFER_SIZE);
    memset(&stringAux,0,sizeof(stringAux));

    sprintf((char *)stringAux,"\"%*s\",\"%*s\",\"%*s\"",strlen(connection),connection,strlen(ip_address),ip_address,strlen(port),port);
    send_Write_Execution_AT_CMD((uint8_t*)AT_CMD_START_TCPUDP_CONEXION,stringAux);

    read_Response_AT_Command_CIPSTART(&serialResponseBuffer);

    if(strstr((char *)serialResponseBuffer,(char *)CONNECT_OK))
    	statusTcpUdpConnection = OK;

    return statusTcpUdpConnection;
}

/**
 * @brief	Close TCP or UDP connection.
 * @note	AT command used: AT+CIPCLOSE
 * 			This command only closes connection at corresponding status of TCP/UDP stack.
 * @param	None.
 * @retval	Integer Value:
 * 			OK(0) - Close successful.
 * 			ERROR(1) - Close fail.
 */
uint8_t close_TCPUDP_Connection(void)
{

    uint8_t statusCloseConnection = ERROR;

    memset(&serialResponseBuffer,0,SERIAL_RESPONSE_BUFFER_SIZE);
    send_Test_Read_AT_CMD((uint8_t*)AT_CMD_CLOSE_TCPUDP_CONEXION);
    read_Response_SIM(&serialResponseBuffer);

    if(strstr((char*)serialResponseBuffer,(char*)CLOSE_OK))
    	statusCloseConnection = OK;

    return statusCloseConnection;
}

/**
 * @brief	Send data through TCP or UDP connection to the server.
 * @note	AT command used: AT+CIPSEND
 * @param	Pointer to buffer of type uint8_t containing the data to send
 * @param	Integer value that specifies the application mode: COMMAND MODE(0) or TRANSPARENT MODE(1)
 * @retval	Integer Value:
 * 			OK(0) - Data successfully sent.
 * 			ERROR(1) - Error sending data.
 */
uint8_t send_Data_TCPUDP(uint8_t *data, uint8_t tcpip_appMode)
{
    uint8_t serialRespBufferSIM800[256];
    uint8_t ctrlz[2];
    uint8_t statusSendData = ERROR;

    memset(&serialRespBufferSIM800, 0,sizeof(serialRespBufferSIM800));
    if(tcpip_appMode == COMMAND_MODE)
    {
        send_Test_Read_AT_CMD((uint8_t*)AT_CMD_SEND_DATA_TCPUDP);
        read_Response_SIM(&serialRespBufferSIM800);

        /* Checks if the SIM returned the '>' character to enter the data to be sent */
        if(strstr((char *)serialRespBufferSIM800,(char *)INPUT_DATA))
        {
        	memset(&serialRespBufferSIM800, 0,sizeof(serialRespBufferSIM800));

        	/* Enter the data and send with the ASCII character CTRLZ(0x1A)*/
        	write_Data_UART(data);
        	HAL_Delay(300);
        	sprintf((char *)ctrlz,"%c",CTRL_Z);
        	write_Data_UART(ctrlz);

        	read_Response_SIM(&serialRespBufferSIM800);

        	if(strstr((char*)serialRespBufferSIM800,(char*)SEND_OK))
        		statusSendData = OK;
        }
    }
    else if(tcpip_appMode == TRANSPARENT_MODE)
    {
    	write_Data_UART(data);
    	statusSendData = OK;
    }

    return statusSendData;
}

/**
 * @brief	Temporarily enable AT commands in transparent mode.
 * @note	AT command used: AT+CIPCLOSE
 * @param	None.
 * @retval	None
 */
void enable_AT_CMD_In_Transparent_Mode(void)
{
    uint8_t i;

    for(i = 0; i<3; i++){
        write_Data_UART((uint8_t *)"+");
        HAL_Delay(1000);
    }
}

/**
 * @brief	Reads the UART receive buffer and stores the response to AT commands.
 * @note	The reading is done by polling.
 * @param	Pointer to the buffer of type uint8_t where the SIM response is stored.
 * @retval	None
 */
static void read_Response_SIM(uint8_t *pBuffer)
{
  uint8_t data[1];

  do
  {
	  /* Get a character from the USART and save Buffer*/
	  data[0] = read_Data_UART();
	  *pBuffer = data[0];

	  /*Verify if the SIM response is the character for data entry*/
	  if (*pBuffer == (uint8_t)INPUT_DATA)
		  break;

	  /**
	   * Commands are usually followed by a response that includes <CR><LF><response><CR><LF>.
	   * In this case check if the response is OK.
	   * */
	  else if((*pBuffer == 'K') && (*(pBuffer - 1) == 'O'))
	  {
		  /* This sequence is used to read the last <CR><LF> without buffering them */
		  ++pBuffer;
		  read_Data_UART();

		  ++pBuffer;
		  read_Data_UART();

		  break;
	  }

  }while(pBuffer++);

}

/**
 * @brief	Reads the UART receive buffer and stores the response of the "AT+CIPSTART" command only.
 * @note	The reading is done by polling. This function is only called by start_Up_TCPUDP_Connection() function.
 * @param	Pointer to the buffer of type uint8_t where the SIM response is stored.
 * @retval	None
 */
static void read_Response_AT_Command_CIPSTART(uint8_t *pBuffer)
{
  uint8_t data[1];

    do
    {
          data[0] = read_Data_UART();
          *pBuffer = data[0];

          /**
           * The response to a successful connection is <CR><LF>CONNECT OK<CR><LF>.
           * In this case we only search for the character sequence "T OK" as successful confirmation.
           * */
          if((*pBuffer == 'K') && (*(pBuffer - 1) == 'O') && (*(pBuffer - 2) == ' ') && (*(pBuffer - 3) == 'T'))
          {
        	/* This sequence is used to read the last <CR><LF> without buffering them */
        	++pBuffer;
          	read_Data_UART();

          	++pBuffer;
          	read_Data_UART();

          	break;
          }

    }while(pBuffer++);

}

/**
 * @brief	Turn on SIM.
 * @param	None.
 * @retval	None.
 */
void SIM800_On()
{
	exit_Reset_SIM();
	power_On_SIM();
}

/**
 * @brief	Turn off SIM.
 * @param	None.
 * @retval	None.
 */
void SIM800_Off(void)
{
	power_Down_SIM();
	keep_Reset_SIM();
}

/**
 * @brief	Restart SIM.
 * @param	None.
 * @retval	None.
 */
void SIM800_restart(void)
{
	restart_SIM();
}
