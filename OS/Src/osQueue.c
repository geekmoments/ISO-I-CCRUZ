/*
 * osQueue.c
 *
 *  Created on: Oct 3, 2023
 *      Author: cesarcruz
 */
#include <stdlib.h>
#include <string.h>

#include "osQueue.h"
#include "osKernel.h"

bool osQueueInit(osQueueObject* queue, const uint32_t dataSize)
{
	    if (queue != NULL) {
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
    osEnterCriticalSection();

    // Verificar si la cola está llena
    if (queue->currentSize >= MAX_SIZE_QUEUE)
    {
        blockTaskFromQueue(queue, 1);
        osExitCriticalSection();
        return false;  // Cola llena, no se puede enviar
    }

    // Calcular el índice del próximo elemento
    queue->endIndex = (queue->endIndex + 1) % MAX_SIZE_QUEUE;

    // Intentar asignar memoria
    queue->data[queue->endIndex] = malloc(queue->dataSize);

    // Verificar si la asignación de memoria fue exitosa
    if (queue->data[queue->endIndex] == NULL)
    {
        // Liberar memoria asignada previamente, si la hubiera
        for (uint32_t i = 0; i < queue->currentSize; i++)
        {
            free(queue->data[i]);
        }

        osExitCriticalSection();
        return false;  // Error en la asignación de memoria
    }

    // Copiar los datos al nuevo elemento
    memcpy(queue->data[queue->endIndex], data, queue->dataSize);

    // Incrementar el tamaño actual de la cola
    queue->currentSize++;

    if (queue->currentSize == 1)
    {
        checkBlockedTaskFromQueue(queue, 1); // Comprobar solo en el límite
    }

    osExitCriticalSection();
    return true;  // Envío exitoso
}


bool osQueueReceive(osQueueObject* queue, void* buffer, const uint32_t timeout)
{
	osEnterCriticalSection();
	if (queue->currentSize > 0)
    {
		memcpy(buffer, queue->data[queue->startIndex], queue->dataSize);
		free(queue->data[queue->startIndex]);

        queue->startIndex = (queue->startIndex + 1)%MAX_SIZE_QUEUE;
        queue->currentSize--;

        if (queue->currentSize == MAX_SIZE_QUEUE - 1) checkBlockedTaskFromQueue(queue, 0);
    }
    else
    {
        blockTaskFromQueue(queue,0);
        osExitCriticalSection();
        return false;
    }

	osExitCriticalSection();
    return true;

}





