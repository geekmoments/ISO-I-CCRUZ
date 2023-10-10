#ifndef INC_OSSEMAPHORE_H
#define INC_OSSEMAPHORE_H

#include <stdint.h>
#include <stdbool.h>



typedef struct
{
	uint32_t  maxCount;
	uint32_t  count;
	bool  lockedFlag;


}osSemaphoreObject;

void osSemaphoreInit(osSemaphoreObject* semaphore, const uint32_t maxCount, const uint32_t count);
bool osSemaphoreTake(osSemaphoreObject* semaphore);
void osSemaphoreGive(osSemaphoreObject* semaphore);


#endif // INC_OSSEMAPHORE_H
