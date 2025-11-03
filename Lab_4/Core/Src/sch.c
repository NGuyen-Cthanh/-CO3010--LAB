/*
 * sch.c
 *
 *  Created on: Oct 26, 2025
 *      Author: LENOVO
 */

#include "sch.h"

uint8_t currentTasks = 0;
uint32_t ERROR_CODE_G = 0;
Tasks taskList[SCH_MAX_TASKS];

void SCH_Init(void){
	deleteAllTasks();
	currentTasks = 0;
	ERROR_CODE_G = 0;
}

uint32_t SCH_Add_Task(void (*task)(), uint32_t delay, uint32_t period){
	if (currentTasks >= SCH_MAX_TASKS){
		ERROR_CODE_G = ERROR_SCH_TOO_MANY_TASKS;
		HAL_GPIO_TogglePin(led4_GPIO_Port, led4_Pin);
		return SCH_MAX_TASKS;
	}

	// Validate inputs
	if (task == NULL){
		ERROR_CODE_G = ERROR_SCH_INVALID_INDEX;
		return SCH_MAX_TASKS;
	}

	taskList[currentTasks].funcPtr = task;
	taskList[currentTasks].delay = delay;
	taskList[currentTasks].period = period;
	taskList[currentTasks].runme = 0;
	taskList[currentTasks].taskID = currentTasks;
	taskList[currentTasks].isEmpty = false;
	taskList[currentTasks].isOneShot = (period == 0);

	currentTasks++;
	return currentTasks - 1;
}

void SCH_Update(void){
	// ROBUST FIX: Handle all edge cases safely
	for (int i = 0; i < currentTasks; i++){
		// Safety check: ensure delay hasn't underflowed (become very large)
		// For uint32_t, values > 0x7FFFFFFF indicate potential underflow
		if (taskList[i].delay > 0x7FFFFFFF){
			// Underflow detected! Reset to 0 and mark task as ready
			taskList[i].delay = 0;
			taskList[i].runme++;
			ERROR_CODE_G = ERROR_SCH_INVALID_INDEX;

			// Reload period if it's a periodic task
			if (taskList[i].period > 0){
				taskList[i].delay = taskList[i].period;
			}
		}
		// Normal case: decrement delay if greater than 0
		else if (taskList[i].delay > 0){
			taskList[i].delay--;

			// Check if task just became ready to run
			if (taskList[i].delay == 0){
				taskList[i].runme++;

				// If periodic task, reload the delay
				if (taskList[i].period > 0){
					taskList[i].delay = taskList[i].period;
				}
			}
		}
		// delay == 0 case
		else {
			// Task is ready to run
			taskList[i].runme++;

			// If periodic task, reload the delay
			if (taskList[i].period > 0){
				taskList[i].delay = taskList[i].period;
			}
		}
	}
}

void SCH_Dispatch_Tasks(void){
	for (int i = 0; i < currentTasks; i++){
		if (taskList[i].runme > 0){
			// Safety check: ensure function pointer is valid
			if (taskList[i].funcPtr != NULL){
				taskList[i].runme--;
				(*taskList[i].funcPtr)();

				// Delete one-shot tasks after execution
				if (taskList[i].isOneShot == true){
					SCH_Delete_Task(taskList[i].taskID);
					i--;  // Adjust index since array shifted
				}
			} else {
				// Invalid function pointer - clear the runme flag
				taskList[i].runme = 0;
				ERROR_CODE_G = ERROR_SCH_INVALID_INDEX;
			}
		}
	}
}

uint32_t SCH_Delete_Task(uint32_t taskID){
	if (taskID >= currentTasks){
		ERROR_CODE_G = ERROR_SCH_INVALID_INDEX;
		return ERROR_CODE_G;
	}

	if (taskList[taskID].isEmpty == true){
		ERROR_CODE_G = ERROR_SCH_INVALID_INDEX;
		return ERROR_CODE_G;
	}

	// Shift all tasks after this one down
	for (int i = taskID; i < currentTasks - 1; i++){
		taskList[i] = taskList[i + 1];
		// Update taskID to match new array position
		taskList[i].taskID = i;
	}

	// Clear the last slot
	int lastIdx = currentTasks - 1;
	taskList[lastIdx].funcPtr = NULL;
	taskList[lastIdx].delay = 0;
	taskList[lastIdx].period = 0;
	taskList[lastIdx].runme = 0;
	taskList[lastIdx].taskID = 0;
	taskList[lastIdx].isEmpty = true;
	taskList[lastIdx].isOneShot = false;

	currentTasks--;
	return taskID;
}

void deleteAllTasks(void){
	for (int i = 0; i < SCH_MAX_TASKS; i++){
		taskList[i].funcPtr = NULL;
		taskList[i].delay = 0;
		taskList[i].period = 0;
		taskList[i].runme = 0;
		taskList[i].taskID = 0;
		taskList[i].isEmpty = true;
		taskList[i].isOneShot = false;
	}
	currentTasks = 0;
}
