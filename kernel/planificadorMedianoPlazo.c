#include "kernel.h"
#include "utils.h"

void planificadorMedianoPlazo ();
/********************************* MEDIANO PLAZO ********************************* */
/*
void planificadorMedianoPlazo(){
    while(1){
        sem_wait(bloqueoMax)
        pcb * pcb = list_remove(proceprocesosSuspendedBlocked, 0);

        log_info(logger, "Iniciando bloqueo de %d........",tiemposBlockedSuspended;);
        usleep(tiemposBlockedSuspended);
        log_info(logger, "Finalizando bloqueo");
        procesosSuspendedBlocked = 0; // para q quede siempre en 0

        // no necesito mutex porque soy elunico q agrego nadie puede sacar sin el signal
        list_add(procesosSuspendedReady,pcb);
        sem_post(prioridad_SuspendedReady);
    }
}
*/

/********************************* MEDIANO PLAZO ********************************* */