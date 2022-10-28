/*
 * API_Debounce.h
 *
 *  Created on: Oct 27, 2022
 *      Author: Jonatan
 */

#ifndef API_INC_API_DEBOUNCE_H_
#define API_INC_API_DEBOUNCE_H_

#include "main.h"
#include "API_DelayNB.h"

#define DEBOUNCE_DELAY	40U	//delay for debounce in [ms]

/*------ Prototype Publics Functions */
void debounceFSM_init();
void debounceFSM_update();
bool_t readKey();
bool_t fallingEdge();
bool_t raisingEdge();

#endif /* API_INC_API_DEBOUNCE_H_ */
