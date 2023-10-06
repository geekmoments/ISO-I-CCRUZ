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

	if(task->taskExecStatus == OS_TASK_RUNNING){
		while(semaphore->take)
		{
				task->taskExecStatus=OS_TASK_BLOCKED;
				semaphore->assignedTask=task;
				osCallSche();

		}
		semaphore->take=true;
		return semaphore->take;
	}


}
void osSemaphoreGive(osSemaphoreObject* semaphore){
	osTaskObject* task;

	task = os_getTareaActual();


	if (task->taskExecStatus == OS_TASK_RUNNING && semaphore->take)  {
		semaphore->take = false;
		semaphore->assignedTask->taskExecStatus = OS_TASK_READY;
	}
}
