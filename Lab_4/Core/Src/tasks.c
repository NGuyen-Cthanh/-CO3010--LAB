/*
 * tasks.c
 *
 *  Created on: Oct 26, 2025
 *      Author: LENOVO
 */

#include "tasks.h"

void task1(void){
    HAL_GPIO_TogglePin(led0_GPIO_Port, led0_Pin);  // PA1 - D1
}

void task2(void){
    HAL_GPIO_TogglePin(led1_GPIO_Port, led1_Pin);  // PA2 - D2
}

void task3(void){
    HAL_GPIO_TogglePin(led2_GPIO_Port, led2_Pin);  // PA3 - D3
}

void task4(void){
    HAL_GPIO_TogglePin(led3_GPIO_Port, led3_Pin);  // PA4 - D4
}

void task5(void){
    HAL_GPIO_TogglePin(led4_GPIO_Port, led4_Pin);  // PB0 - D5
}

