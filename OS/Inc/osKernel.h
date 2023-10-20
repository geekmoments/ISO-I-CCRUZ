

#ifndef INC_OSKERNEL_H_
#define INC_OSKERNEL_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <assert.h>
#include <stdbool.h>

#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
#include "stdint.h"
#include "core_cm4.h"
#include "cmsis_gcc.h"

#include "osSemaphore.h"
#include "osQueue.h"

/* Exported macro ------------------------------------------------------------*/
#define MAX_TASKS            8U
#define MAX_STACK_SIZE       256U
#define MAX_PRIORITY         4U          // MAX_STACK_SIZE Defines the maximum amount of priority.
#define MAX_TASK_NAME_CHAR   10
#define STACK_FRAME_SIZE     17
#define OS_SYSTICK_TICK         1000        // In milliseconds

/* Bits positions on Stack Frame */
#define XPSR_VALUE              1 << 24     // xPSR thumb = 1
#define EXEC_RETURN_VALUE       0xFFFFFFF9  // EXEC_RETURN value. Return to thread mode with MSP, not use FPU
#define XPSR_REG_POSITION       1
#define PC_REG_POSTION          2
#define LR_REG_POSTION          3
#define R12_REG_POSTION         4
#define R3_REG_POSTION          5
#define R2_REG_POSTION          6
#define R1_REG_POSTION          7
#define R0_REG_POSTION          8
#define LR_PREV_VALUE_POSTION   9
#define R4_REG_POSTION          10
#define R5_REG_POSTION          11
#define R6_REG_POSTION          12
#define R7_REG_POSTION          13
#define R8_REG_POSTION          14
#define R9_REG_POSTION          15
#define R10_REG_POSTION         16
#define R11_REG_POSTION         17


typedef enum{
    OS_STATUS_RUNNING   = 0,
    OS_STATUS_RESET     = 1,
    OS_STATUS_STOPPED   = 2,
	OS_STATUS_IRQ		= 3,

}OsStatus;


typedef enum{
    OS_TASK_READY       = 0,
    OS_TASK_RUNNING     = 1,
    OS_TASK_BLOCK       = 2,
    OS_TASK_SUSPENDED   = 3,
}osTaskStatusType;


typedef enum{
    OS_VERYHIGH_PRIORITY    = 0,                // Highest Priority
    OS_HIGH_PRIORITY    	= 1,
    OS_NORMAL_PRIORITY    	= 2,
    OS_LOW_PRIORITY    		= 3,                // Less Priority
}osPriorityType;


typedef struct{
    uint32_t memory[MAX_STACK_SIZE/4];    // Memory Size
    uint32_t taskStackPointer;                   // Store the task SP
    void* taskEntryPoint;                   // Entry point for the task
    osTaskStatusType taskExecStatus;        // Task current execution status
    osPriorityType taskPriority;       // Task priority (Not in used for now)
    uint32_t taskID;                             // Task ID
    char* taskName[MAX_TASK_NAME_CHAR];  // Task name in string
    uint32_t taskTickCounter;
    //---queue
    bool  taskBlockedByFullQueue;
    bool  taskBlockedByEmptyQueue;
    osQueueObject *queueFull;
    osQueueObject *queueEmpty;
    //---semaphore
    bool  semaphoreBlocked;
    osSemaphoreObject *semaphoreTask;

}osTaskObject;


bool osTaskCreate(osTaskObject* handler, osPriorityType priority, void* taskCallback);
/**
 * @brief Función de inicio del sistema operativo.
 */
void osStart(void);
/**
 * @brief Función para obtener el contexto de la tarea actual.
 * @return Puntero al objeto de tarea de la tarea actual.
 */
osTaskObject* getTask(void);
//void osCallSche(void);
/**
 * @brief Función para bloquear una tarea durante un número de ticks.
 * @param tick Número de ticks para bloquear la tarea.
 */
void osDelay(const uint32_t tick);
/**
 * @brief Función para obtener el estado del sistema operativo.
 * @return Estado actual del sistema operativo.
 */
OsStatus osGetStatus(void); //---
/**
 * @brief Función para establecer el estado del sistema operativo.
 * @param status Nuevo estado del sistema operativo.
 */
void osSetStatus(OsStatus s);//---
//---CR
/**
 * @brief Bloquea una tarea por una cola.
 * @param queue Puntero a la cola.
 * @param sender Indica si la tarea fue bloqueada por cola llena (0) o cola vacía (1).
 */
void blockTaskFromQueue(osQueueObject *queue, uint8_t sender);

/**
 * @brief Comprueba y desbloquea tareas bloqueadas por cola.
 * @param queue Puntero a la cola.
 * @param sender Indica si la tarea fue bloqueada por cola llena (0) o cola vacía (1).
 */
void checkBlockedTaskFromQueue(osQueueObject *queue, uint8_t sender);

/**
 * @brief Bloquea una tarea por un semáforo.
 * @param semaphore Puntero al semáforo.
 */

void blockTaskFromSem(osSemaphoreObject* semaphore);

/**
 * @brief Comprueba y desbloquea tareas bloqueadas por semáforo.
 * @param semaphore Puntero al semáforo.
 */
void checkBlockedTaskFromSem(osSemaphoreObject *semaphore);

//----

/**
 * @brief Función para entrar en una sección crítica.
 */

void osEnterCriticalSection(void);

/**
 * @brief Declare the beginning of the critical section.
 */
void osExitCriticalSection(void);


void osSysTickHook(void);
__attribute__((weak)) void osReturnTaskHook(void);
/**
 * @brief Función de manejo de retorno de error.
 * @param caller Llamador de la función de error.
 */
__attribute__((weak)) void osErrorHook(void* caller);
/**
 * @brief Tarea ociosa del sistema.
 */
__attribute__((weak)) void osIdleTask(void);


#ifdef __cplusplus
}
#endif

#endif /* __OS_CORE_H__ */
