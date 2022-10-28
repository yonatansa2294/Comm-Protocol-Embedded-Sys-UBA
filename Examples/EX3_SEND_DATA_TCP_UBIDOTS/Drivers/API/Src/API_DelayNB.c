/*
 * API_Debounce.c
 *
 *  Created on: 07 set. 2022
 *  Author: Yonatan Aguirre
 *  Description: API for implement non-blocking delays.
 */

#include "API_DelayNB.h"

/* --------------------------------------------------------------------------------------------------------
 * Function: void delayInit(delay_t * delay, tick_t duration)
 *
 * Overview: Load the delay duration value into the structure. Also, initialize the running flag to 'false'.
 *
 * Note: This function does not initialize the delay count.
 *
 * -------------------------------------------------------------------------------------------------------*/
void delayInit( delay_t * delay, tick_t duration )
{
	/*------------------------------- verification of parameters -------------------------*/
	if(duration > MAX_DELAY)	//check if the delay value is higher than the allowed value
		duration = MAX_DELAY;

	if(delay != NULL)			//verify if the address of the structure is valid
	{
		delay->running = false; 	// Initialize flag running
		delay->duration = duration;	// Load the initial value of the delay
	}
}

/* --------------------------------------------------------------------------------------------------------
 * Function: bool_t delayRead( delay_t * delay )
 *
 * Overview: Checks if the delay was met. Returns a true value when the condition is met.
 *
 * Note: When the time is expired, the running flag is changed to false.
 *
 * -------------------------------------------------------------------------------------------------------*/
bool_t delayRead( delay_t * delay )
{
	bool toggleLed = false;	//flag to indicated when the led is to be toggle

	/*-------- check flag status to start delay calculation -------*/
	if(delay->running == false)
	{
		delay->startTime = HAL_GetTick();	//initial timestamp
		delay->running = true;				// update flag running
	}
	else
	{
		/*------------- check if the delay duration has been met ----------*/
		if((HAL_GetTick()-delay->startTime)>= delay->duration)
		{
			delay->running = false;	//reset flag of start
			toggleLed = true;	//returns true if the time has expired
		}
	}

	return toggleLed;
}

/* --------------------------------------------------------------------------------------------------------
 * Function: void delayWrite( delay_t * delay, tick_t duration )
 *
 * Overview: Modifies the duration time of an existing delay.
 *
 * -------------------------------------------------------------------------------------------------------*/
void delayWrite( delay_t * delay, tick_t duration )
{
	/*------------------------------- verification of parameters -------------------------*/
	if(duration > MAX_DELAY)
		duration = MAX_DELAY;

	if(delay != NULL)
		delay->duration = duration;
}
