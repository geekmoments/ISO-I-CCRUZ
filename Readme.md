# Implementación de Sistemas Operativos

Este documento describe la implementación de sistemas operativos basada en los recursos proporcionados por el curso ISO_MSE de FIUBA.

## Lista de Desarrollo

### TP1 - Scheduler

El TP1 se enfoca en la implementación del Scheduler, que es un componente fundamental en un sistema operativo en tiempo real. Se abordan los siguientes aspectos:

- **Base de Tiempos del Sistema:** Se basa en la interrupción del Systick para sincronizar las operaciones del sistema.
- **Llamada del Scheduler:** Se realiza en cada interrupción (tick) para tomar decisiones sobre qué tarea debe ejecutarse a continuación.
- **Lógica del Scheduler:** Se desarrolla la lógica para determinar la próxima tarea a ejecutar, teniendo en cuenta prioridades y estados de las tareas.
- **Cambio de Contexto:** Una vez seleccionada la próxima tarea, se configura la interrupción PendSV para llevar a cabo el cambio de contexto necesario.

### TP2 - Estado de Tareas

En este TP se abordan los estados de las tareas en un sistema operativo en tiempo real (RTOS). Se definen los siguientes estados posibles:

- **Ready:** Indica que una tarea está lista para ejecutarse.
- **Running:** Se utiliza para la tarea que se encuentra en ejecución en un momento dado.
- **Blocked:** Indica que una tarea está bloqueada y no puede ejecutarse en ese momento.

#### Tarea Idle

- **Idle Task:** Se describe la tarea Idle, que generalmente no realiza operaciones y debe llamar a la instrucción "Wait For Interrupt" para ahorrar energía.
- **Implementación WEAK:** La tarea Idle se define como "WEAK" para permitir que su implementación pueda ser sobrescrita según las necesidades del sistema.

#### API Delay

##### Prioridades

- **Prioridades:** Se establece que debe haber al menos cuatro (4) niveles de prioridad, donde el nivel cero (0) es el más prioritario y el nivel tres (3) el de menor prioridad.
- **Lógica de Ejecución:** En el scheduler se evalúa cuál tarea será la siguiente en ser ejecutada, teniendo en cuenta tanto el nivel de prioridad como el estado de las tareas.
- **Round Robin:** Entre tareas de igual prioridad, se utiliza la lógica de ejecución circular (round robin). Esto significa que si existen tres tareas de igual prioridad con el estado válido para su ejecución, el orden de ejecución será: Tarea 1 -> Tarea 2 -> Tarea 3 -> Tarea 1 -> Tarea 2 -> ...

### TP3 - Semáforos

En el TP3 se aborda la implementación de semáforos, que son fundamentales para la sincronización en sistemas operativos en tiempo real. Se consideran dos tipos de semáforos:

- **Semaforos Binarios:** Se utilizan para controlar el acceso a recursos compartidos de forma binaria (bloqueado/desbloqueado).
- **Semaforos Contadores:** Permiten un mayor control y asignación de recursos, ya que pueden tener un valor entero como contador.

#### Mecanismo de Semaforo para el RTOS

- **Operaciones de Semaforo:** Se describen las tres operaciones fundamentales en el mecanismo de semáforo en el RTOS:
  1. Creación del semáforo.
  2. Toma del semáforo (bloqueo hasta que esté disponible).
  3. Liberación del semáforo.

### Colas

En este apartado se abordan las colas, que son estructuras de datos esenciales en un RTOS para la comunicación entre tareas. Se detallan las siguientes operaciones:

- **Creación de la Cola:** Se describe cómo crear una cola para permitir la transferencia de datos entre tareas.
- **Envío de Datos a la Cola:** Cómo las tareas pueden enviar datos a la cola para que otras tareas los consuman.
- **Toma de Datos de la Cola:** Cómo las tareas pueden tomar datos de la cola para su procesamiento.

Este documento proporciona una visión general de la implementación de sistemas operativos en tiempo real, cubriendo aspectos clave como el Scheduler, los estados de las tareas, semáforos y colas, que son elementos esenciales en la construcción de sistemas robustos y eficientes.
