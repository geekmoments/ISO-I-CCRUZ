
/**
 * @file osKernel.c
 * @brief Funciones principales del sistema operativo.
 */
#include "../../OS/Inc/osKernel.h"

#define IDLEPRIORIRY 100

osTaskObject idle;
uint8_t osTasksCreated = 0;
uint8_t currentTaskIndex = 0;

/**
 * @struct osKernelObject
 * @brief Estructura que contiene información del sistema operativo.
 */
typedef struct {

    	osTaskObject* osCurrentTaskCallback;    ///< Tarea actual
        osTaskObject* osNextTaskCallback;       ///< Próxima tarea a ejecutar
        osTaskObject* osListTask[MAX_TASKS];    ///< Lista de tareas
        OsStatus osStatus;                      ///< Estado actual del sistema operativo
//---CR
        osTaskObject* osTaskPriorityList[MAX_TASKS];///< Lista de prioridades de tareas
        bool inISRContext; // rastreamos si el SO usa sem o queue desde ISR
//---

} osKernelObject;

static osKernelObject OsKernel;


// Declaración de funciones
/**
 * @brief Planificador de tareas.
 */
	static void scheduler(void);
	/**
	 * @brief Obtiene el contexto de la siguiente tarea.
	 * @param currentStackPointer Puntero de pila actual.
	 * @return Puntero de pila de la siguiente tarea.
	 */
	static uint32_t getNextContext(uint32_t currentStaskPointer);
	/**
	 * @brief Ordena las tareas por prioridad.
	 * @param task Cantidad de tareas a ordenar.
	 */

	void taskByPriority(uint8_t task);
	/**
	 * @brief Gestiona las demoras de las tareas bloqueadas.
	 */
	void manageTaskDelays(void);
	/**
	 * @brief Encuentra la tarea bloqueada por un semáforo.
	 * @param semaphore Puntero al semáforo.
	 * @return Puntero a la tarea bloqueada o NULL si no se encuentra.
	 */
	osTaskObject* findBlockedTaskFromSemaphore(osSemaphoreObject *semaphore);
	/**
	 * @brief Encuentra la tarea bloqueada por una cola.
	 * @param sender Indica si la tarea fue bloqueada por cola llena (0) o cola vacía (1).
	 * @return Puntero a la tarea bloqueada o NULL si no se encuentra.
	 */
	osTaskObject* findBlockedTaskFromQueue(uint8_t sender);
	/**
	 * @brief Obtiene la tarea en ejecución.
	 * @return Puntero a la tarea en ejecución o NULL si no hay ninguna.
	 */

	osTaskObject* getRunningTask(void);
	/**
	 * @brief Realiza un cambio de contexto forzado.
	 */


// Initializing a task  Step I

bool osTaskCreate(osTaskObject* handler, osPriorityType priority, void* taskCallback)
{
	//==== *variableInitialization*  on taskInit===
    static uint8_t osTaskCount = 0;

    if (taskCallback == NULL || handler == NULL) {
        // Manejo de error si taskCallback o handler son NULL
        return false; // O toma otra acción de manejo de errores
    }

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
        return false;
    }
    //== end else if

    //===initialization of *taskObject*
    handler->memory[MAX_STACK_SIZE/4 - XPSR_REG_POSITION] = XPSR_VALUE;//1 << 24     // xPSR.T = 1
    handler->memory[MAX_STACK_SIZE/4 - PC_REG_POSTION] = (uint32_t)taskCallback; // address
    handler->memory[MAX_STACK_SIZE/4 - LR_PREV_VALUE_POSTION] = EXEC_RETURN_VALUE; // 0xFFFFFFF9
    handler->taskStackPointer = (uint32_t)(handler->memory + MAX_STACK_SIZE/4 - STACK_FRAME_SIZE);
    handler->taskEntryPoint = taskCallback;
    handler->taskExecStatus = OS_TASK_READY;
    handler->taskPriority = priority;
    handler->taskTickCounter = 0;
    //===end initialization of *taskObject*

    OsKernel.osListTask[osTaskCount] = handler; // -- storage pointer object handler
    handler->taskID = osTaskCount;
    osTaskCount++;

    return true;
}


// Operating system startup Step II

void osStart(void)
{
    //== iterate and count valid addresses in the list
    for (uint8_t i = 0; i < MAX_TASKS - 1; i++)
    {
        if (NULL != OsKernel.osListTask[i]) osTasksCreated++;
    }

    // == order tasks first high priority tasks
    taskByPriority(osTasksCreated);
    // == end order

    // idle tasks initialization
    osTaskCreate(&idle, IDLEPRIORIRY, osIdleTask);

    NVIC_DisableIRQ(SysTick_IRQn);
    NVIC_DisableIRQ(PendSV_IRQn);

    // initialization Os
    OsKernel.osStatus = OS_STATUS_STOPPED;
    OsKernel.osCurrentTaskCallback = NULL;
    OsKernel.osNextTaskCallback = NULL;
    OsKernel.inISRContext = false;
    NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);

    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / OS_SYSTICK_TICK);

    NVIC_EnableIRQ(PendSV_IRQn);
    NVIC_EnableIRQ(SysTick_IRQn);

}

// Function para obtener el siguiente contexto

static uint32_t getNextContext(uint32_t currentStackPointer)
{
    // Si es la primera vez que se ejecuta el sistema operativo
    if (OsKernel.osStatus != OS_STATUS_RUNNING)
    {
        // Establece el estado de la tarea actual como en ejecución
        OsKernel.osCurrentTaskCallback->taskExecStatus = OS_TASK_RUNNING;
        // Actualiza el estado del sistema operativo a en ejecución
        OsKernel.osStatus = OS_STATUS_RUNNING;
        // Devuelve el puntero de pila de la tarea actual
        return OsKernel.osCurrentTaskCallback->taskStackPointer;
    }

    // Almacena el último puntero de pila utilizado en la tarea actual y cambia su estado a lista para ejecutar
    OsKernel.osCurrentTaskCallback->taskStackPointer = currentStackPointer;

    // Verifica si la tarea actual estaba bloqueada y tiene un contador de tiempo pendiente
    if (OsKernel.osCurrentTaskCallback->taskTickCounter != 0 && OsKernel.osCurrentTaskCallback->taskExecStatus == OS_TASK_BLOCK)
    {
        // la tarea permanece bloqueada
    }
    else
    {
        // Establece el estado de la tarea actual como lista para ejecutar
        OsKernel.osCurrentTaskCallback->taskExecStatus = OS_TASK_READY;
    }

    // Cambia a la siguiente tarea en la cola y cambia su estado a en ejecución
    OsKernel.osCurrentTaskCallback = OsKernel.osNextTaskCallback;
    OsKernel.osCurrentTaskCallback->taskExecStatus = OS_TASK_RUNNING;

    // Devuelve el puntero de pila de la tarea actual (que ahora está en ejecución)
    return OsKernel.osCurrentTaskCallback->taskStackPointer;
}

// Task scheduling function Step IV
static void scheduler(void)
{
    static uint8_t osTaskIndex = 0;
	static uint8_t status[MAX_TASKS];
	osTaskStatusType taskStatus;
	uint8_t numBlockedTasks = 0;


    if (OsKernel.osStatus != OS_STATUS_RUNNING)
    {
        OsKernel.osCurrentTaskCallback = OsKernel.osListTask[0];
        return;
    }



	for (uint8_t taskIndex = 0; taskIndex < osTasksCreated; taskIndex++)
	{
		if (OsKernel.osListTask[taskIndex]->taskExecStatus == OS_TASK_BLOCK)
		{
			numBlockedTasks++;
		}
	}

	if (numBlockedTasks == osTasksCreated)
	{
		if (OsKernel.osCurrentTaskCallback != OsKernel.osListTask[osTasksCreated])
		{
			OsKernel.osNextTaskCallback = OsKernel.osListTask[osTasksCreated];

			osTaskIndex = osTasksCreated;
		}
		return;
	}

	for (uint8_t taskIndex = 0; taskIndex < osTasksCreated; taskIndex++)
	{
		taskStatus = OsKernel.osListTask[taskIndex]->taskExecStatus;
		switch(taskStatus)
		{
			case OS_TASK_READY:
			{
				if (osTasksCreated == 1 || taskIndex > osTaskIndex)
				{
				    OsKernel.osNextTaskCallback = OsKernel.osListTask[taskIndex];
				    osTaskIndex = taskIndex;
				    return;
				}

				for (uint8_t blockedTaskIndex = 0; blockedTaskIndex < osTasksCreated; blockedTaskIndex++)
				{
				    if (status[blockedTaskIndex] == 1 && OsKernel.osListTask[taskIndex]->taskExecStatus == OS_TASK_READY)
				    {
				        OsKernel.osNextTaskCallback = OsKernel.osListTask[taskIndex];
				        status[blockedTaskIndex] = 0;
				        osTaskIndex = taskIndex;
				        return;
				    }
				}

			}
			break;
			case OS_TASK_RUNNING:
			{
				if (osTaskIndex == osTasksCreated - 1)
				{
					osTaskIndex = 0;
					OsKernel.osNextTaskCallback = OsKernel.osListTask[osTaskIndex];
				}
			}
			break;
			case OS_TASK_BLOCK:
			{
			    status[taskIndex] = (taskIndex <= osTaskIndex) ? 1 : 0;

			}
			break;
			case OS_TASK_SUSPENDED: break;




		}
	}

}


__attribute__ ((naked)) void PendSV_Handler(void)
{
    // Se entra a la seccion critica y se deshabilita las interrupciones.
	__ASM volatile ("cpsid i");
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

    // Se sale de la seccion critica y se habilita las interrupciones.
	__ASM volatile ("cpsie i");

    /* Se hace un branch indirect con el valor de LR que es nuevamente EXEC_RETURN */
    __ASM volatile ("bx lr");

}


void SysTick_Handler(void)
{
    scheduler();
    manageTaskDelays();

    osSysTickHook();
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
    __ISB();
    __DSB(); //
}

void taskByPriority(uint8_t task) {
	    for (uint8_t i = 0; i < task - 1; i++) {
	        for (uint8_t j = 0; j < task - i - 1; j++) {
	            if (OsKernel.osListTask[j]->taskPriority > OsKernel.osListTask[j + 1]->taskPriority) {
	                OsKernel.osListTask[j] = (osTaskObject*)((uintptr_t)OsKernel.osListTask[j] ^ (uintptr_t)OsKernel.osListTask[j + 1]);
	                OsKernel.osListTask[j + 1] = (osTaskObject*)((uintptr_t)OsKernel.osListTask[j] ^ (uintptr_t)OsKernel.osListTask[j + 1]);
	                OsKernel.osListTask[j] = (osTaskObject*)((uintptr_t)OsKernel.osListTask[j] ^ (uintptr_t)OsKernel.osListTask[j + 1]);
	            }
	        }
	    }
}

void manageTaskDelays(void)
{
    for (uint8_t i = 0; i < osTasksCreated; i++)
    {
        osTaskObject *task = OsKernel.osListTask[i];

        if (task->taskExecStatus == OS_TASK_BLOCK && task->taskTickCounter > 0)
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
    osEnterCriticalSection();

    osTaskObject *task = NULL;

    // Busca la tarea en ejecución
    for (uint8_t i = 0; i < osTasksCreated; i++)
    {
        if (OsKernel.osListTask[i]->taskExecStatus == OS_TASK_RUNNING)
        {
            task = OsKernel.osListTask[i];
            break;
        }
    }

    if (task != NULL)
    {
        // Bloquea la tarea actual y establece el contador de ticks
        task->taskExecStatus = OS_TASK_BLOCK;
        task->taskTickCounter = tick;

        // Yieldea para permitir que otras tareas se ejecuten
        osYield();

        // Solicita una interrupción PendSV para el cambio de contexto
        SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
    }

    osExitCriticalSection();
}

//==========new Functions
void blockTaskFromSem(osSemaphoreObject* semaphore)
{
    osTaskObject *task = NULL;
    task = getRunningTask();
    if (task != NULL)
    {
    	task->semaphoreTask = semaphore;
        task->semaphoreBlocked = true;
        task->taskExecStatus = OS_TASK_BLOCK;
    }
    osYield();
}

void checkBlockedTaskFromSem(osSemaphoreObject *semaphore)
{
    osTaskObject *task = NULL;
    task = findBlockedTaskFromSemaphore(semaphore);
    if (task != NULL)
    {
        task->taskExecStatus = OS_TASK_READY;
        task->semaphoreBlocked = false;
    	task->semaphoreTask = NULL;
    }
    osYield();
}

void blockTaskFromQueue(osQueueObject *queue, uint8_t sender)
{
    osTaskObject *task = NULL;
    task = getRunningTask();
    if (task != NULL)
    {
        if(sender)  task->taskBlockedByFullQueue = true;
        else        task->taskBlockedByEmptyQueue = true;
        if(sender)  task->queueFull  = queue;
        else        task->queueEmpty = queue;
        task->taskExecStatus = OS_TASK_BLOCK;
    }
    osYield();
}

void checkBlockedTaskFromQueue(osQueueObject *queue, uint8_t sender)
{
    osTaskObject *task = NULL;
    task = findBlockedTaskFromQueue(sender);
    if (task != NULL)
    {
        task->taskExecStatus = OS_TASK_READY;
        if (sender) task->taskBlockedByEmptyQueue = false;
        else        task->taskBlockedByFullQueue  = false;
        if (sender) task->queueFull = NULL;
        else        task->queueEmpty= NULL;
    }
    osYield();
}

osTaskObject* findBlockedTaskFromQueue(uint8_t sender)
{
    for (uint8_t i = 0; i < osTasksCreated; i++)
    {
        if( OsKernel.osListTask[i]->taskExecStatus == OS_TASK_BLOCK)
        {
            switch(sender)
            {
                case 0:
                {
                    if (OsKernel.osListTask[i]->taskBlockedByFullQueue == true)
                    {
                        return OsKernel.osListTask[i];
                    }
                }
                break;

                case 1:
                {
                    if (OsKernel.osListTask[i]->taskBlockedByEmptyQueue == true)
                    {
                        return OsKernel.osListTask[i];
                    }
                }
                break;
            }
        }
    }
    return NULL;
}

osTaskObject* findBlockedTaskFromSemaphore(osSemaphoreObject *semaphore)
{
	osTaskObject *task = NULL;
    for (uint8_t i = 0; i < osTasksCreated; i++)
    {
        if( OsKernel.osListTask[i]->taskExecStatus == OS_TASK_BLOCK)
        {
            if (OsKernel.osListTask[i]->semaphoreBlocked == true && OsKernel.osListTask[i]->semaphoreTask == semaphore)
            {
            	task = OsKernel.osListTask[i];
                return task;
            }
        }
    }
    return NULL;
}


osTaskObject* getRunningTask(void)
{
    osTaskObject *task = NULL;
    for (uint8_t i = 0; i < osTasksCreated; i++)
    {
        if(OsKernel.osListTask[i]->taskExecStatus == OS_TASK_RUNNING)
        {
            task = OsKernel.osListTask[i];
            return task;
        }
    }
    return task;
}

///====
osTaskObject* getTask(void)  {
	return OsKernel.osCurrentTaskCallback;
}


//==
/*
void osCallSche(void){

    scheduler();

}*/
void osYield(void)
{
    if (osGetStatus() == OS_STATUS_IRQ)
    {
        OsKernel.inISRContext = true;
    }

    scheduler();
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
    __ISB();
    __DSB();
}
OsStatus osGetStatus(void){
	return OsKernel.osStatus;

}
void osSetStatus(OsStatus status) {
	OsKernel.osStatus = status;

}
bool osIsInISRContext(void)//==
{
	return OsKernel.inISRContext;
}

void osSetInISRContext(bool val)//===
{
	OsKernel.inISRContext = val;
}



void osEnterCriticalSection(void)
{
    __disable_irq();
}
void osExitCriticalSection(void)
{
    __enable_irq();
}


// Hooks

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
