#include "kernel.h"
#include "utils.h"

void planificadorCortoPlazo();
void administrar_bloqueos();

void planificadorCortoPlazo(){
    char* algoritmo = config_kernel.algoritmo_planificacion;
	// int ciclosTotales=0; NO SE SI VA
	pthread_create(&administradorBloqueos, NULL, (void *) administrar_bloqueos, NULL);
	pthread_detach(administradorBloqueos);

	if(algoritmo == 'FIFO'){
		algoritmo_FIFO()
	}else{
		algoritmo_SRT()
	}
}


void administrar_bloqueos(){ // hilo
	while(1){
		int enBlock = list_size(procesosBlocked);
		if(enBlock > 0){
			pcb * procesoBloqueado = list_remove(procesosBlocked, 0);
			int tiempo = list_remove(tiemposBlocked, 0);

			if(tiempo > config_kernel.tiempo_maximo_bloqueado){
				tiemposBlockedSuspended = tiempo - config_kernel.tiempo_maximo_bloqueado
				tiempo = config_kernel.tiempo_maximo_bloqueado
			}

			log_info(logger, "Iniciando bloqueo de %d........",tiempo);
			usleep(tiempo);
			log_info(logger, "Finalizando bloqueo");

			if(tiemposBlockedSuspended > 0){
				list_add(procesosSuspendedBlocked, procesoBloqueado)
				sem_post(bloqueoMax) // le digo al mediano q hay un proceso en procesosSupendedBlocked
			}else{
				sem_wait(agregarAReady)
				list_add(procesosReady,procesoBloqueado);
				sem_post(agregarAReady);
			}
			//free(pcb)
		}
	}

 }