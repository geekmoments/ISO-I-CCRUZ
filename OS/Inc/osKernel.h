#ifndef INC_OSKERNEL_H_
#define INC_OSKERNEL_H_

#include <stdint.h>
#include <stdbool.h>

#define MAX_NUMBER_TASK         8U          // Defines maximum 8 task and idle
#define MAX_STACK_SIZE          256U        // Defines maximum stack size for a task.
#define MAX_NUMBER_PRIORITY     4U          // maximum task priority.

#define SYSTICK_PERIOD_MS       1U          // Systick period time in mili-second.
#define SIZE_STACK_FRAME        17U         // Size stack frame
#define SYSTICK_PERIOD_MS       1U          // Systick period ms


#define XPSR_VALUE              1 << 24     // xPSR.T = 1
#define EXEC_RETURN_VALUE       0xFFFFFFF9  // EXEC_RETURN value. Return to thread mode with MSP, not use FPU
#define XPSR_REG_POSITION       1U
#define PC_REG_POSTION          2U
#define LR_REG_POSTION          3U
#define R12_REG_POSTION         4U
#define R3_REG_POSTION          5U
#define R2_REG_POSTION          6U
#define R1_REG_POSTION          7U
#define R0_REG_POSTION          8U
#define LR_PREV_VALUE_POSTION   9U
#define R4_REG_POSTION          10U
#define R5_REG_POSTION          11U
#define R6_REG_POSTION          12U
#define R7_REG_POSTION          13U
#define R8_REG_POSTION          14U
#define R9_REG_POSTION          15U
#define R10_REG_POSTION         16U
#define R11_REG_POSTION         17U

/* Possible task status */
typedef enum
{
    OS_TASK_READY,      // 0Ready state
    OS_TASK_RUNNING,    // 1Running state
    OS_TASK_BLOCK,      // 2Blocked state
    OS_TASK_SUSPEND     // 3Suspended state
}osTaskStatusType;

typedef enum{
    PRIORITY_0,   // 0
    PRIORITY_1,	  // 1
    PRIORITY_2,	  // 2
    PRIORITY_3,   // 3
	PRIORITY_4,	  // 4
	PRIORITIES_QUANTITY // size of enum
}OsTaskPriorityNumber;


typedef struct
{
    uint32_t             memoryStack[MAX_STACK_SIZE/4];   // *
    uint32_t             stackPointer;               	 //  *
    void*                entryPoint;                 	 //  *
    OsTaskPriorityNumber taskPriority;					 //  *
    uint8_t              taskId;                         // Task ID, it is a number started in 0
    osTaskStatusType     taskStatus;                     // Status task.
    uint32_t			 tickCounter;

}osTaskObject;


/**
 * @brief Create task.
 *
 * @param[in,out]   handler     Data structure of task.
 * @param[in]       callback    Function executed on task
 *
 * @return Return true if task was success or false in otherwise.
 */
bool osTaskCreate(osTaskObject* handler, void* callback, OsTaskPriorityNumber priority);

/**
 * @brief Initialization pendSV exception with lowest priority possible.
 */
void osStart(void);
void osIdleTask(void);


#endif // INC_OSKERNEL_H_
