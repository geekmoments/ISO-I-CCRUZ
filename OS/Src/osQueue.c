/*
 * osQueue.c
 *
 *  Created on: Oct 3, 2023
 *      Author: cesarcruz
 */


#include "osQueue.h"


bool osQueueInit(osQueueObject* queue, const uint32_t dataSize)
{
	/* Verificar que el puntero de la cola sea válido */
	    if (queue != NULL) {
	        /* Inicializar la cola */
	        queue->dataSize = dataSize;
	        queue->currentSize = 0;
	        queue->endIndex = -1;
	        queue->startIndex = 0;
	        return true;
	    }
	    return false;

}
bool osQueueSend(osQueueObject* queue, const void* data, const uint32_t timeout)
{
	osTaskObject* task;
	task = getTask();

    if (queue->currentSize >= MAX_SIZE_QUEUE)
    {
    	//==
    	task->taskExecStatus=OS_TASK_BLOCKED;
    	//==
    	osCallSche();
        return false;
    }
    else
    {
        queue->endIndex = (queue->endIndex + 1)%MAX_SIZE_QUEUE;
        queue->data[queue->endIndex] = malloc(queue->dataSize);
        memcpy(queue->data[queue->endIndex], data, queue->dataSize);

        queue->currentSize++;

        if (queue->currentSize == 1){
        	task->taskExecStatus=OS_TASK_READY;

        	osCallSche();

        }
    }

    return true;
}

bool osQueueReceive(osQueueObject* queue, void* buffer, const uint32_t timeout)
{
	osTaskObject* task;
	task = getTask();
	if (queue->currentSize > 0)
	{
	    /* Verificar que el puntero no sea nulo antes de liberar memoria */
	    if (queue->data[queue->startIndex] != NULL) {
	        memcpy(buffer, queue->data[queue->startIndex], queue->dataSize);
	        free(queue->data[queue->startIndex]);

	        queue->startIndex = (queue->startIndex + 1) % MAX_SIZE_QUEUE;
	        queue->currentSize--;

	        if (queue->currentSize == MAX_SIZE_QUEUE - 1) {
	            task->taskExecStatus = OS_TASK_READY;
	            osCallSche();
	        }
	    }
	    else {
	        /* Manejo de error: el puntero es nulo, no se puede liberar memoria. */
	    }
	}
	else
	{
	    task->taskExecStatus = OS_TASK_BLOCKED;
	    osCallSche();
	    return false;
	}

	    return true;

}





