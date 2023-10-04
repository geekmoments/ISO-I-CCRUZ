/*
 * osSemaphore.h
 *
 *  Created on: Oct 3, 2023
 *      Author: cesarcruz
 */
#include <osSemaphore.h>

/*
void osSemaphoreInit(osSemaphore* semaphore, bool isBinary, uint8_t maxCount) {
    semaphore->isBinary = isBinary;
    if (isBinary) {
        semaphore->data.binary.status = false;
        semaphore->data.binary.taskSemaphore = NULL;
    } else {
        semaphore->data.counter.counter = maxCount;
        semaphore->data.counter.max = maxCount;
    }
}
*/
