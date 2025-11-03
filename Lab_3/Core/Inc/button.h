/*
 * button.h
 *
 *  Created on: Oct 13, 2025
 *      Author: LENOVO
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "global.h"

#define NO_BUTTON 3

#define PRESSED 0
#define RELEASED 1

int isButtonNoPressed(int);
void buttonRead();


#endif /* INC_BUTTON_H_ */
