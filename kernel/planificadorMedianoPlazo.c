#include "kernel.h"
#include "utils.h"

void planificadorMedianoPlazo(){
    char* estadoSuspend = "Suspende";
    while(1){
        sem_wait(bloqueoMax);
        pcb* procesoBloqueadoSuspendido = list_remove(procesosSuspendedBlocked, 0);
        avisar_a_memoria(socket_memoria, estadoSuspend, procesoBloqueadoSuspendido, logger);
        signal(grado_multiprogramacion);
        log_info(logger, "Iniciando bloqueo de %d........",tiemposBlockedSuspended;);
        usleep(tiemposBlockedSuspended);
        log_info(logger, "Finalizando bloqueo");
        tiemposBlockedSuspended = 0; // para q quede siempre en 0

        // no necesito mutex porque soy elunico q agrego nadie puede sacar sin el signal
        list_add(procesosSuspendedReady,procesoBloqueadoSuspendido);
        sem_post(prioridad_SuspendedReady);
    }
}
