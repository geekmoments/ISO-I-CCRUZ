#ifndef INC_OSSEMAPHORE_H
#define INC_OSSEMAPHORE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    bool isBinary;       // Indica si es un semáforo binario (true) o contador (false)
    union {
        struct {
            bool status;  // Estado del semáforo binario
         //   task* taskSemaphore; // Tarea que hace uso del semáforo
        } binary;
        struct {
            uint8_t counter; // Contador del semáforo
            uint8_t max;   // Valor máximo del contador
        } counter;
    } data;
}osSemaphoreObject;

/**
 * @brief Initializes semaphore binary or not.
 *
 * @param[in,out]   semaphore   Semaphore handler.
 * @param[in]       maxCount    Maximum count value that can be reached.
 * @param[in]       count       The count value assigned to the semaphore when it is created.
 */
//void osSemaphoreInit(osSemaphoreObject* semaphore, const uint32_t maxCount, const uint32_t count);

void osSemaphoreInit(osSemaphoreObject* semaphore, bool isBinary, uint8_t maxCount);


/**
 * @brief Take semaphore.
 *
 * @param[in,out]   semaphore   Semaphore handler.
 *
 * @return Returns true if the semaphore could be taken.
 */
//bool osSemaphoreTake(osSemaphoreObject* semaphore);
//void osSemaphoreTake(osSemaphore* semaphore, uint32_t timeout);

/**
 * @brief Give semaphore.
 *
 * @param[in,out]   semaphore   Semaphore handler.
 */
void osSemaphoreGive(osSemaphoreObject* semaphore);


#endif // INC_OSSEMAPHORE_H
