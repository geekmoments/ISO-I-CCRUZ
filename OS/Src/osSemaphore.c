/*
 * osSemaphore.h
 *
 *  Created on: Oct 3, 2023
 *      Author: cesarcruz
 */
#include <osSemaphore.h>
#include "osKernel.h"

// Inicializa un objeto de semáforo
void osSemaphoreInit(osSemaphoreObject* semaphore, const uint32_t maxCount, const uint32_t count){
    semaphore->maxCount = maxCount; // Establece el valor máximo permitido
    semaphore->count = count;       // Establece el contador inicial
    semaphore->lockedFlag = false;  // Inicialmente no bloqueado
}

// Intenta tomar el semáforo
bool osSemaphoreTake(osSemaphoreObject* semaphore){

    osEnterCriticalSection(); // Entra en la sección crítica

    if (semaphore->lockedFlag == true)
    {
        blockTaskFromSem(semaphore); // Bloquea la tarea actual si el semáforo ya está tomado
        osExitCriticalSection();    // Sale de la sección crítica
        return false;               // Devuelve false indicando que el semáforo no se tomó
    }
    else
    {
        semaphore->lockedFlag = true; // Marca el semáforo como tomado
    }

    osExitCriticalSection(); // Sale de la sección crítica
    return true;             // Devuelve true indicando que el semáforo se tomó con éxito
}

// Libera el semáforo
void osSemaphoreGive(osSemaphoreObject* semaphore){
    osEnterCriticalSection(); // Entra en la sección crítica

    semaphore->lockedFlag = false; // Marca el semáforo como liberado

    checkBlockedTaskFromSem(semaphore); // Verifica si hay tareas bloqueadas esperando el semáforo

    osExitCriticalSection(); // Sale de la sección crítica
}
