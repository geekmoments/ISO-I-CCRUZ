#include "../../OS/Inc/osKernel.h"

// Definición de una tarea especial llamada "idle" y contador de tareas creadas

osTaskObject idle;
uint8_t osTasksCreated = 0;


// Estructura que almacena información del kernel del sistema operativo

typedef struct {
    uint32_t osLastError;               // Último error del sistema
    OsStatus osStatus;                  // Estado actual del sistema operativo
    uint32_t osScheduleExec;            // Bandera de ejecución del planificador
    osTaskObject* osCurrTaskCallback;   // Tarea actual
    osTaskObject* osNextTaskCallback;   // Próxima tarea a ejecutar
    osTaskObject* listTask[MAX_TASKS];  // Lista de tareas
    osTaskObject* osTaskPriorityList[MAX_TASKS];  // Lista de tareas ordenadas por prioridad
} osKernelObject;

static osKernelObject OsKernel;


// Declaración de funciones

static void scheduler(void);
static uint32_t getNextContext(uint32_t currentStaskPointer);
void taskByPriority(uint8_t n);
void manageTaskDelays(void);



// Inicialización de una tarea

exceptionType osTaskInit(osTaskObject* taskHandler, void* taskCallback, OsTaskPriorityLevel priority)
{
    static uint8_t osTaskCount = 0;

    assert(taskCallback != NULL);
    assert(taskHandler != NULL);

    if (osTaskCount == 0)
    {
        for (uint8_t i = 0; i < MAX_TASKS - 1; i++)
        {
            OsKernel.listTask[i] = NULL;
        }
    }
    else if (osTaskCount >= MAX_TASKS - 1)
    {
        return ERROR_CODE;
    }

    taskHandler->TaskMemoryStack[MAX_STACK_SIZE/4 - XPSR_REG_POSITION] = XPSR_VALUE;
    taskHandler->TaskMemoryStack[MAX_STACK_SIZE/4 - PC_REG_POSTION] = (uint32_t)taskCallback;
    taskHandler->TaskMemoryStack[MAX_STACK_SIZE/4 - LR_PREV_VALUE_POSTION] = EXEC_RETURN_VALUE;
    taskHandler->taskStackPointer = (uint32_t)(taskHandler->TaskMemoryStack + MAX_STACK_SIZE/4 - STACK_FRAME_SIZE);
    taskHandler->taskEntryPoint = taskCallback;
    taskHandler->taskExecStatus = OS_TASK_READY;
    taskHandler->taskPriority = priority;

    OsKernel.listTask[osTaskCount] = taskHandler;
    taskHandler->taskID = osTaskCount;
    osTaskCount++;

    return (exceptionType)OK_CODE;
}


// Inicio del sistema operativo

exceptionType osStart(void)
{
    exceptionType initResult = OK_CODE;

    for (uint8_t i = 0; i < MAX_TASKS - 1; i++)
    {
        if (NULL != OsKernel.listTask[i]) osTasksCreated++;
    }
    taskByPriority(osTasksCreated);
    initResult = osTaskInit(&idle, osIdleTask, PRIORITY_4);
    if (initResult != OK_CODE) return ERROR_CODE;

    NVIC_DisableIRQ(SysTick_IRQn);
    NVIC_DisableIRQ(PendSV_IRQn);

    OsKernel.osStatus = OS_STATUS_STOPPED;
    OsKernel.osCurrTaskCallback = NULL;
    OsKernel.osNextTaskCallback = NULL;

    NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);

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
        OsKernel.osCurrTaskCallback->taskExecStatus = OS_TASK_RUNNING;
        OsKernel.osStatus = OS_STATUS_RUNNING;
        return OsKernel.osCurrTaskCallback->taskStackPointer;
    }

    OsKernel.osCurrTaskCallback->taskStackPointer = currentStaskPointer;
    if (OsKernel.osCurrTaskCallback->delay == 0 || OsKernel.osCurrTaskCallback->taskExecStatus != OS_TASK_BLOCKED)
    {
        OsKernel.osCurrTaskCallback->taskExecStatus = OS_TASK_READY;
    }


    OsKernel.osCurrTaskCallback = OsKernel.osNextTaskCallback;
    OsKernel.osCurrTaskCallback->taskExecStatus = OS_TASK_RUNNING;

    return OsKernel.osCurrTaskCallback->taskStackPointer;
}
// Función de planificación de tareas

static void scheduler(void)
{
    static uint8_t osTaskIndex = 0;
    osTaskStatusType taskStatus;

    if (OsKernel.osStatus != OS_STATUS_RUNNING)
    {
        OsKernel.osCurrTaskCallback = OsKernel.listTask[0];
        return;
    }

    manageTaskDelays();

    uint8_t firstReadyTaskIndex = osTasksCreated;

    for (uint8_t taskIterator = 0; taskIterator < osTasksCreated; taskIterator++)
    {
        taskStatus = OsKernel.listTask[taskIterator]->taskExecStatus;

        switch (taskStatus)
        {
            case OS_TASK_RUNNING:
            {
                if (osTaskIndex == osTasksCreated - 1)
                {
                    osTaskIndex = 0;
                    OsKernel.osNextTaskCallback = OsKernel.listTask[osTaskIndex];
                }
            }
            break;

            case OS_TASK_SUSPENDED:
                break;

            case OS_TASK_READY:
            {
                if (taskIterator > osTaskIndex)
                {
                    OsKernel.osNextTaskCallback = OsKernel.listTask[taskIterator];
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
        OsKernel.osNextTaskCallback = OsKernel.listTask[firstReadyTaskIndex];
        osTaskIndex = firstReadyTaskIndex;
    }
    else
    {
        // Todas las tareas están bloqueadas, seleccionamos la tarea especial.
        if (OsKernel.osCurrTaskCallback != OsKernel.listTask[osTasksCreated])
        {
            OsKernel.osNextTaskCallback = OsKernel.listTask[osTasksCreated];
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

void SysTick_Handler(void)
{
    scheduler();
    osSysTickHook();
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
    __ISB();
    __DSB();
}
// Función para ordenar las tareas por prioridad

void taskByPriority(uint8_t n)
{
    for (uint8_t i = 0; i < n - 1; i++)
    {
        uint8_t minIndex = i;

        for (uint8_t j = i + 1; j < n; j++)
        {
            if (OsKernel.listTask[j]->taskPriority < OsKernel.listTask[minIndex]->taskPriority)
            {
                minIndex = j;
            }
        }

        if (minIndex != i)
        {
            osTaskObject *temp = OsKernel.listTask[i];
            OsKernel.listTask[i] = OsKernel.listTask[minIndex];
            OsKernel.listTask[minIndex] = temp;
        }
    }
}
// Función para contar el retraso de las tareas

void manageTaskDelays(void)
{
    osTaskObject *task = NULL;

    for (uint8_t i = 0; i < osTasksCreated; i++)
     {
         osTaskObject *task = OsKernel.listTask[i];

         if (task->taskExecStatus == OS_TASK_BLOCKED && task->delay > 0)
         {
             task->delay--;
             if (task->delay == 0)
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
        if (OsKernel.listTask[i]->taskExecStatus == OS_TASK_RUNNING)
        {
            task = OsKernel.listTask[i];
            break;
        }
    }

    task->taskExecStatus = OS_TASK_BLOCKED;
    task->delay = tick;

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
