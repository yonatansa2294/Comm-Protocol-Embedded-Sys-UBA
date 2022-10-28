/*
 * API_DelayNB.h
 *
 *  Created on: Oct 27, 2022
 *      Author: Jonatan
 */

#ifndef API_INC_API_DELAYNB_H_
#define API_INC_API_DELAYNB_H_

/*--------------------------------- Includes ---------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "stm32f4xx_hal.h"

/*----------------------------- Private defines ------------------------------*/
#define MAX_DELAY	30000	//maximum delay in milliseconds to be implemented

/*----------------------------- Exported types -------------------------------*/
typedef uint32_t tick_t;
typedef bool bool_t;

/*-- definition of delay_t as a type of structure for delay implementation ---*/
typedef struct{
   tick_t startTime;
   tick_t duration;
   bool_t running;
} delay_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/*---------------------- Exported functions prototypes -----------------------*/
void delayInit( delay_t * delay, tick_t duration );
bool_t delayRead( delay_t * delay );
void delayWrite( delay_t * delay, tick_t duration );

#endif /* API_INC_API_DELAYNB_H_ */
