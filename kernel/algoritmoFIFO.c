#include "kernel.h"

void avisar_a_planificador_LP(PCB* pcbFinalizado){ //hilo <--- creo que no seria un hilo sino una funcion auxiliar
	pthread_mutex_lock(&mutexExit);
    list_add(procesosExit, pcbFinalizado);
    pthread_mutex_unlock(&mutexExit);
    sem_post(&finalizoProceso);
}

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
		pcb_destroyer(unProceso);
		respuestaDeCPU = recibirRespuestaCPU(socket_dispatch);

		unProceso = list_get(respuestaDeCPU, 0);
		motivoDeRegreso = list_get(respuestaDeCPU, 1);

		switch(motivoDeRegreso){
			case IO_PCB:
				tiempoDeBloqueo = list_get(respuestaDeCPU, 3);

				blockedPCB* datosPCB = asignarMemoria(sizeof(blockedPCB));
				datosPCB -> unPCB = unProceso;
				datosPCB -> tiempo = tiempoDeBloqueo;
				datosPCB -> aux = 0;
				datosPCB -> suspendido = 0;

				pthread_mutex_lock(&mutexBloqueo);
				list_add(procesosBlocked, datosPCB);
				pthread_mutex_unlock(&mutexBloqueo);

				sem_post(&procesoBloqueado);
				break;

			case EXIT_PCB:
				avisar_a_planificador_LP(unProceso);
		}
		list_destroy(respuestaDeCPU);
	}
}


