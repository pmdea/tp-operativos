#include "kernel.h"

void algoritmo_SRT(){
	PCB* unProceso;
	uint32_t motivoDeRegreso;
	uint32_t tiempoDeBloqueo;
	uint32_t rafagaEjecutada;
	t_list* respuestaDeCPU;
	ejecutando = 0;
	while(1){
		sem_wait(&nuevoProcesoReady);

		pthread_mutex_lock(&mutexReady);
		if(list_size(procesosReady) > 1){
			log_info(loggerKernel, "REPLANIFICANDO...");
			list_sort(procesosReady,ordenarSRT);
		}
		unProceso = list_remove(procesosReady, 0);
		pthread_mutex_unlock(&mutexReady);
		enviarPCB(socket_dispatch, *unProceso, loggerKernel);
		pcb_destroyer(unProceso);
		pthread_mutex_lock(&variableEjecutando);
		ejecutando = 1;
		pthread_mutex_unlock(&variableEjecutando);

		respuestaDeCPU = recibirRespuestaCPU(socket_dispatch);

		unProceso = list_get(respuestaDeCPU, 0);
		motivoDeRegreso = list_get(respuestaDeCPU, 1);

		switch(motivoDeRegreso){
			case IO_PCB:
				tiempoDeBloqueo = list_get(respuestaDeCPU, 3);
				rafagaEjecutada = list_get(respuestaDeCPU, 2);
				estimador(unProceso, config_kernel.alfa, rafagaEjecutada);
				blockedPCB* datosPCB = asignarMemoria(sizeof(blockedPCB));
				datosPCB -> unPCB = unProceso;
				datosPCB -> tiempo = tiempoDeBloqueo;
				datosPCB -> aux = 0;
				datosPCB -> suspendido = 0;

				pthread_mutex_lock(&mutexBloqueo);
				list_add(procesosBlocked, datosPCB);
				pthread_mutex_unlock(&mutexBloqueo);

				pthread_mutex_lock(&variableEjecutando);
				ejecutando = 0;
				pthread_mutex_unlock(&variableEjecutando);

				sem_post(&procesoBloqueado);
				break;

			case DESALOJO_PCB:
				pthread_mutex_lock(&mutexReady);
				list_add(procesosReady, unProceso);
				pthread_mutex_unlock(&mutexReady);

				pthread_mutex_lock(&variableEjecutando);
				ejecutando = 0;
				pthread_mutex_unlock(&variableEjecutando);

				sem_post(&nuevoProcesoReady);
				break;

			case EXIT_PCB:
				avisar_a_planificador_LP(unProceso);
				pthread_mutex_lock(&variableEjecutando);
				ejecutando = 0;
				pthread_mutex_unlock(&variableEjecutando);
				break;
		}
		list_destroy(respuestaDeCPU);
	}
}

void administradorInterrupcionCPU(){
	while(1){
		sem_wait(&enviarInterrupcion);

		avisar_a_cpu_interrupt();
	}
}
