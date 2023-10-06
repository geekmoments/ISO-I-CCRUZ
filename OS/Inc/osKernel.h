

#ifndef INC_OSKERNEL_H_
#define INC_OSKERNEL_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
#include "stdint.h"
#include "core_cm4.h"
#include "cmsis_gcc.h"
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
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
    OK_CODE          =  0,
    ERROR_CODE       = -1,
    OS_ERROR_CODE    = -2,
}exceptionType;


typedef enum{
    OS_STATUS_RUNNING   = 0,
    OS_STATUS_RESET     = 1,
    OS_STATUS_STOPPED   = 2,
}OsStatus;


typedef enum{
    OS_TASK_RUNNING     = 0,
    OS_TASK_READY       = 1,
    OS_TASK_BLOCKED     = 2,
    OS_TASK_SUSPENDED   = 3,
}osTaskStatusType;


typedef enum{
    OS_VERYHIGH_PRIORITY    = 0,                // Highest Priority
    OS_HIGH_PRIORITY    = 1,
    OS_NORMAL_PRIORITY    = 2,
    PRIORITY_IDLE    = 100,                // Less Priority
}osPriorityType;


typedef struct{
    uint32_t TaskMemoryStack[MAX_STACK_SIZE/4];    // Memory Size
    uint32_t taskStackPointer;                   // Store the task SP
    void* taskEntryPoint;                   // Entry point for the task
    osTaskStatusType taskExecStatus;        // Task current execution status
    osPriorityType taskPriority;       // Task priority (Not in used for now)
    uint32_t taskID;                             // Task ID
    char* taskName[MAX_TASK_NAME_CHAR];  // Task name in string
    uint32_t taskTickCounter;
}osTaskObject;


bool osTaskCreate(osTaskObject* handler, osPriorityType priority, void* taskCallback);

bool osStart(void);
osTaskObject* getTask(void);
void osCallSche(void);



void osDelayAndCount(const uint32_t tick);

void osSysTickHook(void);
__attribute__((weak)) void osReturnTaskHook(void);
__attribute__((weak)) void osErrorHook(void* caller);
__attribute__((weak)) void osIdleTask(void);


#ifdef __cplusplus
}
#endif

#endif /* __OS_CORE_H__ */
