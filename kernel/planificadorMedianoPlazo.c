#include "kernel.h"

void planificador_MedianoPlazo(){
	PCB* procesoBloqueadoSuspendido;
	int tiemposBlockedSuspended;
    while(1){
        sem_wait(&bloqueoMax); // INCIALIZA EN 0
        pthread_mutex_lock(&mutexBloqueoSuspendido);
        procesoBloqueadoSuspendido = list_remove(procesosSuspendedBlocked, 0);
        tiemposBlockedSuspended = list_remove(tiemposBlockedSuspendMax, 0);
        pthread_mutex_unlock(&mutexBloqueoSuspendido);

        avisar_a_memoria(SUSPENDE, *procesoBloqueadoSuspendido, loggerKernel);
        //int32_t respuesta = deserializarInt32(socket_memoria);

        sem_post(&grado_multiprogramacion);
        log_info(loggerKernel, "BLOQUEANDO PROCESO SUSPENDIDO ID %i - TIEMPO BLOQUEO %i",procesoBloqueadoSuspendido -> id, tiemposBlockedSuspended);
        sleep(tiemposBlockedSuspended/1000);
        log_info(loggerKernel, "FINALIZO BLOQUEO PROCESO SUSPENDIDO ID %i", procesoBloqueadoSuspendido -> id);


        pthread_mutex_lock(&mutexSuspendido);
        list_add(procesosSuspendedReady,procesoBloqueadoSuspendido);
        pthread_mutex_unlock(&mutexSuspendido);

    }
}
