#include "kernel.h"


void algoritmo_FIFO(){
	PCB* unProceso;
	uint32_t motivoDeRegreso;
	uint32_t tiempoDeBloqueo;
	t_list* respuestaDeCPU;

	while(1){
		sem_wait(&nuevoProcesoReady);

		pthread_mutex_lock(&mutexReady);
		unProceso = list_remove(procesosReady, 0);
		pthread_mutex_unlock(&mutexReady);

		enviarPCB(socket_dispatch, *unProceso, loggerKernel);

		respuestaDeCPU = recibirRespuestaCPU(socket_dispatch);

		unProceso = list_get(respuestaDeCPU, 0);
		motivoDeRegreso = list_get(respuestaDeCPU, 1);

		switch(motivoDeRegreso){
			case IO_PCB:
				tiempoDeBloqueo = list_get(respuestaDeCPU, 3);

				pthread_mutex_lock(&mutexBloqueo);
				list_add(procesosBlocked, unProceso);
				list_add(tiemposBlocked, tiempoDeBloqueo);
				pthread_mutex_unlock(&mutexBloqueo);

				sem_post(&procesoBloqueado);
				break;

			case EXIT_PCB:
				avisar_a_planificador_LP(unProceso);
		}
	}
}

void avisar_a_planificador_LP(PCB* pcbFinalizado){ //hilo <--- creo que no seria un hilo sino una funcion auxiliar
	pthread_mutex_lock(&mutexExit);
    list_add(procesosExit, pcbFinalizado);
    pthread_mutex_unlock(&mutexExit);
    sem_post(&finalizoProceso);
}
