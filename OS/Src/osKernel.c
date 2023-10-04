#include "../../OS/Inc/osKernel.h"

// Definition of a special task called "idle" and counter of created tasks

osTaskObject idle;
uint8_t osTasksCreated = 0;


//Structure that stores operating system kernel information

typedef struct {
    uint32_t osLastError;               // Último error del sistema
    OsStatus osStatus;                  // Estado actual del sistema operativo
    uint32_t osScheduleExec;            // Bandera de ejecución del planificador
    osTaskObject* osCurrentTaskCallback;   // Tarea actual
    osTaskObject* osNextTaskCallback;   // Próxima tarea a ejecutar
    osTaskObject* osListTask[MAX_TASKS];  // Lista de tareas
    osTaskObject* osTaskPriorityList[MAX_TASKS];  // Lista de tareas ordenadas por prioridad
} osKernelObject;

static osKernelObject OsKernel;


//=== Function declaration

static void scheduler(void);
static uint32_t getNextContext(uint32_t currentStaskPointer);
void taskByPriority(uint8_t n);
void manageTaskDelays(void);
//=== end declaration


// Initializing a task  Step I

exceptionType osTaskCreate(osTaskObject* taskHandler, void* taskCallback, OsTaskPriorityLevel priority)
{
	//==== *variableInitialization*  on taskInit===
    static uint8_t osTaskCount = 0;

    assert(taskCallback != NULL);
    assert(taskHandler != NULL);
    //=== end *variableInitialization* taskInit===
    if (osTaskCount == 0) //-- if is the first time, initialize array with NUll
    {
        for (uint8_t i = 0; i < MAX_TASKS - 1; i++)
        {
            OsKernel.osListTask[i] = NULL;
        }
    }					//end if
    else if (osTaskCount >= MAX_TASKS - 1) // --- if there is more task than the max value, return error
    {
        return ERROR_CODE;
    }
    //== end else if

    //===initialization of *taskObject*
    taskHandler->TaskMemoryStack[MAX_STACK_SIZE/4 - XPSR_REG_POSITION] = XPSR_VALUE;//1 << 24     // xPSR.T = 1
    taskHandler->TaskMemoryStack[MAX_STACK_SIZE/4 - PC_REG_POSTION] = (uint32_t)taskCallback; // address
    taskHandler->TaskMemoryStack[MAX_STACK_SIZE/4 - LR_PREV_VALUE_POSTION] = EXEC_RETURN_VALUE; // 0xFFFFFFF9
    taskHandler->taskStackPointer = (uint32_t)(taskHandler->TaskMemoryStack + MAX_STACK_SIZE/4 - STACK_FRAME_SIZE);
    taskHandler->taskEntryPoint = taskCallback;
    taskHandler->taskExecStatus = OS_TASK_READY;
    taskHandler->taskPriority = priority;
    taskHandler->taskTickCounter = 0;
    //===end initialization of *taskObject*

    OsKernel.osListTask[osTaskCount] = taskHandler; // -- storage pointer object handler
    taskHandler->taskID = osTaskCount;
    osTaskCount++;

    return (exceptionType)OK_CODE;
}


// Operating system startup Step II

exceptionType osStart(void)
{
    exceptionType initResult = OK_CODE; // error manage variable
    //== iterate and count valid addresses in the list
    for (uint8_t i = 0; i < MAX_TASKS - 1; i++)
    {
        if (NULL != OsKernel.osListTask[i]) osTasksCreated++;
    }

    // == order tasks first high priority tasks
    taskByPriority(osTasksCreated);
    // == end order

    // idle tasks initialization
    initResult = osTaskCreate(&idle, osIdleTask, PRIORITY_IDLE); // high value of priority is the most low priority

    if (initResult != OK_CODE) return ERROR_CODE;

    NVIC_DisableIRQ(SysTick_IRQn);
    NVIC_DisableIRQ(PendSV_IRQn);

    // initialization Os
    OsKernel.osStatus = OS_STATUS_STOPPED;
    OsKernel.osCurrentTaskCallback = NULL;
    OsKernel.osNextTaskCallback = NULL;
    // end initialization

    NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);// set low priority

    // update and setup clock
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / OS_SYSTICK_TICK);

    NVIC_EnableIRQ(PendSV_IRQn);
    NVIC_EnableIRQ(SysTick_IRQn);

    return initResult;
}

// Función para obtener el siguiente contexto

static uint32_t getNextContext(uint32_t currentStaskPointer)
{
    if (OsKernel.osStatus != OS_STATUS_RUNNING)
    {
        OsKernel.osCurrentTaskCallback->taskExecStatus = OS_TASK_RUNNING;
        OsKernel.osStatus = OS_STATUS_RUNNING;
        return OsKernel.osCurrentTaskCallback->taskStackPointer;
    }

    OsKernel.osCurrentTaskCallback->taskStackPointer = currentStaskPointer;
    if (OsKernel.osCurrentTaskCallback->taskTickCounter == 0 || OsKernel.osCurrentTaskCallback->taskExecStatus != OS_TASK_BLOCKED)
    {
        OsKernel.osCurrentTaskCallback->taskExecStatus = OS_TASK_READY;
    }


    OsKernel.osCurrentTaskCallback = OsKernel.osNextTaskCallback;
    OsKernel.osCurrentTaskCallback->taskExecStatus = OS_TASK_RUNNING;

    return OsKernel.osCurrentTaskCallback->taskStackPointer;
}
// Task scheduling function Step IV

static void scheduler(void)
{
    static uint8_t osTaskIndex = 0;
    osTaskStatusType taskStatus;

    if (OsKernel.osStatus != OS_STATUS_RUNNING)
    {
        OsKernel.osCurrentTaskCallback = OsKernel.osListTask[0];
        return;
    }

    manageTaskDelays(); // manage and update ticks counter

    uint8_t firstReadyTaskIndex = osTasksCreated;

    for (uint8_t taskIterator = 0; taskIterator < osTasksCreated; taskIterator++)
    {
        taskStatus = OsKernel.osListTask[taskIterator]->taskExecStatus;

        switch (taskStatus)
        {
            case OS_TASK_RUNNING:
            {
                if (osTaskIndex == osTasksCreated - 1)
                {
                    osTaskIndex = 0;
                    OsKernel.osNextTaskCallback = OsKernel.osListTask[osTaskIndex];
                }
            }
            break;

            case OS_TASK_SUSPENDED:
                break;

            case OS_TASK_READY:
            {
                if (taskIterator > osTaskIndex)
                {
                    OsKernel.osNextTaskCallback = OsKernel.osListTask[taskIterator];
                    osTaskIndex = taskIterator;
                    return;
                }
                else if (taskIterator < firstReadyTaskIndex)
                {
                    firstReadyTaskIndex = taskIterator;
                }
            }
            break;

            case OS_TASK_BLOCKED:
            {
                // No es necesario hacer nada para tareas bloqueadas.
            }
            break;
        }
    }

    // Si no se encontró una tarea READY o RUNNING,
    // seleccionamos la primera tarea READY encontrada antes.
    if (firstReadyTaskIndex < osTasksCreated)
    {
        OsKernel.osNextTaskCallback = OsKernel.osListTask[firstReadyTaskIndex];
        osTaskIndex = firstReadyTaskIndex;
    }
    else
    {
        // Todas las tareas están bloqueadas, seleccionamos la tarea especial.
        if (OsKernel.osCurrentTaskCallback != OsKernel.osListTask[osTasksCreated])
        {
            OsKernel.osNextTaskCallback = OsKernel.osListTask[osTasksCreated];
            osTaskIndex = osTasksCreated;
        }
    }
}


__attribute__ ((naked)) void PendSV_Handler(void)
{
	 /**
		     * Implementación de stacking para FPU:
		     *
		     * Las tres primeras corresponden a un testeo del bit EXEC_RETURN[4]. La instruccion TST hace un
		     * AND estilo bitwise (bit a bit) entre el registro LR y el literal inmediato. El resultado de esta
		     * operacion no se guarda y los bits N y Z son actualizados. En este caso, si el bit EXEC_RETURN[4] = 0
		     * el resultado de la operacion sera cero, y la bandera Z = 1, por lo que se da la condicion EQ y
		     * se hace el push de los registros de FPU restantes
		     */
		    __ASM volatile ("tst lr, 0x10");
		    __ASM volatile ("it eq");
		    __ASM volatile ("vpusheq {s16-s31}");

		    /**
		     * Cuando se ingresa al handler de PendSV lo primero que se ejecuta es un push para
			 * guardar los registros R4-R11 y el valor de LR, que en este punto es EXEC_RETURN
			 * El push se hace al reves de como se escribe en la instruccion, por lo que LR
			 * se guarda en la posicion 9 (luego del stack frame). Como la funcion getNextContext
			 * se llama con un branch con link, el valor del LR es modificado guardando la direccion
			 * de retorno una vez se complete la ejecucion de la funcion
			 * El pasaje de argumentos a getContextoSiguiente se hace como especifica el AAPCS siendo
			 * el unico argumento pasado por RO, y el valor de retorno tambien se almacena en R0
			 *
			 * NOTA: El primer ingreso a este handler (luego del reset) implica que el push se hace sobre el
			 * stack inicial, ese stack se pierde porque no hay seguimiento del MSP en el primer ingreso
		     */
		    __ASM volatile ("push {r4-r11, lr}");
		    __ASM volatile ("mrs r0, msp");
		    __ASM volatile ("bl %0" :: "i"(getNextContext));
		    __ASM volatile ("msr msp, r0");
		    __ASM volatile ("pop {r4-r11, lr}");    //Recuperados todos los valores de registros

		    /**
		     * Implementación de unstacking para FPU:
		     *
		     * Habiendo hecho el cambio de contexto y recuperado los valores de los registros, es necesario
		     * determinar si el contexto tiene guardados registros correspondientes a la FPU. si este es el caso
		     * se hace el unstacking de los que se hizo PUSH manualmente.
		     */
		    __ASM volatile ("tst lr,0x10");
		    __ASM volatile ("it eq");
		    __ASM volatile ("vpopeq {s16-s31}");

		    /* Se hace un branch indirect con el valor de LR que es nuevamente EXEC_RETURN */
		    __ASM volatile ("bx lr");
}
 //==== except by systick timer Step IV

void SysTick_Handler(void)
{
    scheduler(); //first we execute the schedule in the SystickHandler
    osSysTickHook();
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk; // config pendSV for contex change
    __ISB(); // for complete
    __DSB(); // actions
}
// Function to sort tasks by priority

void taskByPriority(uint8_t n) // basic algorithm for ascending order
{
    for (uint8_t i = 0; i < n - 1; i++) // iterate index tasks
    {
        uint8_t minIndex = i;

        for (uint8_t j = i + 1; j < n; j++) // from the second
        {
            if (OsKernel.osListTask[j]->taskPriority < OsKernel.osListTask[minIndex]->taskPriority)//compare priorities
            {
                minIndex = j;
            }
        }

        if (minIndex != i)// if the value is less
        {
        	// change position of task --- high priority first position
            osTaskObject *temp = OsKernel.osListTask[i];
            OsKernel.osListTask[i] = OsKernel.osListTask[minIndex];
            OsKernel.osListTask[minIndex] = temp;
        }
    }
}
// Función para contar el retraso de las tareas

void manageTaskDelays(void)
{
    osTaskObject *task = NULL;

    for (uint8_t i = 0; i < osTasksCreated; i++)
     {
         osTaskObject *task = OsKernel.osListTask[i];

         if (task->taskExecStatus == OS_TASK_BLOCKED && task->taskTickCounter > 0)
         {
             task->taskTickCounter--;
             if (task->taskTickCounter == 0)
             {
                 task->taskExecStatus = OS_TASK_READY;
             }
         }
     }
}
// Función para bloquear una tarea durante un número de ticks

void osDelay(const uint32_t tick)
{
    NVIC_DisableIRQ(SysTick_IRQn);

    osTaskObject *task = NULL;

    for (uint8_t i = 0; i < osTasksCreated; i++)
    {
        if (OsKernel.osListTask[i]->taskExecStatus == OS_TASK_RUNNING)
        {
            task = OsKernel.osListTask[i];
            break;
        }
    }

    task->taskExecStatus = OS_TASK_BLOCKED;
    task->taskTickCounter = tick;

    scheduler();

    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;

    __ISB();
    __DSB();

    NVIC_EnableIRQ(SysTick_IRQn);
}


// Hooks débiles

__attribute__((weak)) void osReturnTaskHook(void)
{
    while (1)
    {
        __WFI();
    }
}

__attribute__((weak)) void osSysTickHook(void)
{
    __ASM volatile ("nop");
}

__attribute__((weak)) void osErrorHook(void* caller)
{
    while (1)
    {
    }
}

__attribute__((weak)) void osIdleTask(void)
{
    while (1)
    {
        __WFI();
    }
}
