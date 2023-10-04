/*
 * osSemaphore.h
 *
 *  Created on: Oct 3, 2023
 *      Author: cesarcruz
 */
#include <osSemaphore.h>


void osSemaphoreInit(osSemaphoreObject* semaphore, const uint32_t maxCount, const uint32_t count){
	semaphore->take=true; //  start take true always
	semaphore->assignedTask=NULL;
}

bool osSemaphoreTake(osSemaphoreObject* semaphore){

	osTaskObject* task;

	task = getTask();
	if (task->taskExecStatus == OS_TASK_RUNNING)  {


			if(semaphore->take)  {
				task->taskExecStatus = OS_TASK_BLOCKED;
				semaphore->assignedTask = task;
			}
			else  {
				semaphore->take		 = true;
				return true;
			}
		}

}
void osSemaphoreGive(osSemaphoreObject* semaphore){

}
