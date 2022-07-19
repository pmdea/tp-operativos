# tp-2022-1c-Los-Domadores-de-SO

## KERNEL

Gestiona la ejecución de los diferentes procesos que se ingresan al sistema mediante las consolas, planificando su ejecución en la cpu del sistema

Debe mantener conexiones con la CPU, la memoria y las diferentes consolas. Se implementa mediante una estrategia de multiples hilos de ejecucion.

cualquier fallo debe ser informado mediante un mensaje en el archivo de log y el sistema debe seguir funcionando.

Gestiona la planificacion con el PCB y los tres planificadores distintos.

Solo habra una cola de procesos bloqueados, con planificacion FIFO

##PCB
Debe incluir

* id: identificador del proceso
* tamaño: tamaño en bytes del proceso
* instrucciones: lista de instrucciones
* PC: dir de la proxima instruccion a ejecutar
* tabla_paginas: identificador de la tabla de paginas del proceso en memoria.
* estimacion_rafaga: valor inicial en el archivo config y es recalculada con promedio ponderado.

## PLANIFICADOR LARGO PLAZO
Al conectarse una consola al kernel, se genera la estructura PCB y asigna este proceso al estado new.

Cuando un proceso pasa a READY, se envia un mensaje al módulo memoria para que inicialice las estructuras necesarias y obtenga el valor de la tabla de paginas para el PCB

Cuando se recibe un PCB con motivo de finalizarlo, se pasa el proceso a EXIT y se avisa a la memoria para que libere las estructuras. Cuando se libera la memoria, se avisa a la consola la finalizacion.

## PLANIFICADOR CORTO PLAZO

Los procesos en READY son planificados por:
FIFO y SRT (SJF con desalojo)

En el caso de los algoritmos con desalojo, cuando un proceso llegue a la cola READY se deberá enviar una Interrupción al proceso CPU a través de la conexión de interrupt para indicarle que deberá desalojar al proceso que se encuentra actualmente en ejecución.

Al recibir el PCB del proceso en ejecución, se calcularán las estimaciones correspondientes, según indique el algoritmo, para seleccionar el siguiente a ejecutar.

Cabe  aclarar  que  en  todos  los casos el PCB será recibido a través de la conexión de dispatch, quedando la conexión de interrupt dedicada solamente a enviar mensajes de interrupción.

## PLANIFICADOR MEDIANO PLAZO

Se encarga de las transiciones BLOCKED

Para que un proceso entre en suspensión se debe cumplir que el mismo esté bloqueado por un tiempo mayor al límite definido por archivo de configuración.

Al suspenderse, se envia un mensaje a Memoria con la info necesaria y se espera confirmacion.

Un proceso en SUSPEDED-READY es mas prioritario que los NEW
