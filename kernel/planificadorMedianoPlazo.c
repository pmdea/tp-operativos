#include "kernel.h"
#include "utils.h"

void planificadorMedianoPlazo ();

void planificadorMedianoPlazo(){
    while(1){
        sem_wait(bloqueoMax)
        pcb* procesoBloqueadoSuspendido = list_remove(procesosSuspendedBlocked, 0);

        log_info(logger, "Iniciando bloqueo de %d........",tiemposBlockedSuspended;);
        usleep(tiemposBlockedSuspended);
        log_info(logger, "Finalizando bloqueo");
        tiemposBlockedSuspended = 0; // para q quede siempre en 0

        // no necesito mutex porque soy elunico q agrego nadie puede sacar sin el signal
        list_add(procesosSuspendedReady,procesoBloqueadoSuspendido);
        sem_post(prioridad_SuspendedReady);
    }
}
