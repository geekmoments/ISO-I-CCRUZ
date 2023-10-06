
#ifndef INC_OSQUEUE_H
#define INC_OSQUEUE_H

#include <stdint.h>
#include <stdbool.h>
#include "../../OS/Inc/osKernel.h"
#include <string.h>

#define MAX_SIZE_QUEUE  128     // Maximum buffer

typedef struct
{
	uint8_t *data[MAX_SIZE_QUEUE];
	osTaskObject* assignedTaskQ;
	uint32_t startIndex;
	uint32_t endIndex;
	uint32_t dataSize;
	uint32_t currentSize;


}osQueueObject;

bool osQueueInit(osQueueObject* queue, const uint32_t dataSize);

bool osQueueSend(osQueueObject* queue, const void* data, const uint32_t timeout);
bool osQueueReceive(osQueueObject* queue, void* buffer, const uint32_t timeout);

#endif // INC_OSQUEUE_H
