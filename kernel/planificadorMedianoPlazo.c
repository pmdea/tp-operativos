#include "kernel.h"

void planificador_MedianoPlazo(){
    while(1){
        sem_wait(&bloqueoMax);
        pthread_mutex_lock(&mutexBloqueoSuspendido);
        PCB* procesoBloqueadoSuspendido = list_remove(procesosSuspendedBlocked, 0);
        int tiemposBlockedSuspended = list_remove(tiemposBlockedSuspendMax, 0);
        pthread_mutex_unlock(&mutexBloqueoSuspendido);
        //avisar_a_memoria(socket_memoria, SUSPENDE, procesoBloqueadoSuspendido, loggerKernel);
        sem_post(&grado_multiprogramacion);
        log_info(loggerKernel, "Iniciando bloqueo en suspendido del Proceso de ID %i por un tiempo de %d........",procesoBloqueadoSuspendido -> id, tiemposBlockedSuspended);
        sleep(tiemposBlockedSuspended/1000);
        log_info(loggerKernel, "Finalizando en suspendido bloqueo del Proceso de ID %i", procesoBloqueadoSuspendido -> id);

        // no necesito mutex porque soy elunico q agrego nadie puede sacar sin el signal
        pthread_mutex_lock(&mutexSuspendido);
        list_add(procesosSuspendedReady,procesoBloqueadoSuspendido);
        pthread_mutex_unlock(&mutexSuspendido);
        sem_post(&prioridad_SuspendedReady);
    }
}
