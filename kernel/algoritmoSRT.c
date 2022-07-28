#include "kernel.h"

void algoritmo_SRT(){
    pthread_create(&ejecucionProcesoSRTHilo, NULL, ejecucionProcesoSRT, NULL);
    pthread_create(&administradorRespuestaCPUHilo, NULL, administradorRespuestaCPU, NULL);
    pthread_join(ejecucionProcesoSRTHilo, NULL);
    pthread_join(administradorRespuestaCPUHilo, NULL);
}

void ejecucionProcesoSRT(){
	PCB* unProceso;
	ejecutando = 0;
	while(1){
		sem_wait(&nuevoProcesoReady);

		if(ejecutando == 0){
			pthread_mutex_lock(&mutexReady);
			if(list_size(procesosReady) > 1){
				log_info(loggerKernel, "REPLANIFICANDO...");
				list_sort(procesosReady,ordenarSRT);
			}
			unProceso = list_remove(procesosReady, 0);
			pthread_mutex_unlock(&mutexReady);
			enviarPCB(socket_dispatch, *unProceso, loggerKernel);

			pthread_mutex_lock(&variableEjecutando);
			ejecutando = 1;
			pthread_mutex_unlock(&variableEjecutando);
		} else {
			avisar_a_cpu_interrupt();
		}

	}
}

void administradorRespuestaCPU(){
	PCB* unProceso;
	uint32_t motivoDeRegreso;
	uint32_t tiempoDeBloqueo;
	uint32_t rafagaEjecutada;
	t_list* respuestaDeCPU;
	while(1){
		respuestaDeCPU = recibirRespuestaCPU(socket_dispatch);

		unProceso = list_get(respuestaDeCPU, 0);
		motivoDeRegreso = list_get(respuestaDeCPU, 1);

		switch(motivoDeRegreso){
			case IO_PCB:
				tiempoDeBloqueo = list_get(respuestaDeCPU, 3);
				rafagaEjecutada = list_get(respuestaDeCPU, 2);
				estimador(unProceso, config_kernel.alfa, rafagaEjecutada);

				log_error(loggerKernel, "RAFAGA NUEVA %f, ID %i", unProceso -> estimacion_rafaga, unProceso -> id);

				pthread_mutex_lock(&mutexBloqueo);
				list_add(procesosBlocked, unProceso);
				list_add(tiemposBlocked, tiempoDeBloqueo);
				pthread_mutex_unlock(&mutexBloqueo);

				pthread_mutex_lock(&variableEjecutando);
				ejecutando = 0;
				pthread_mutex_unlock(&variableEjecutando);

				sem_post(&procesoBloqueado);
				break;

			case DESALOJO_PCB:
				rafagaEjecutada = list_get(respuestaDeCPU, 2);
				estimador(unProceso, config_kernel.alfa, rafagaEjecutada);
				log_warning(loggerKernel, "RAFAGA NUEVA %f, ID %i", unProceso -> estimacion_rafaga, unProceso -> id);
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
	}
}

















/*    pthread_create(&ejecucionProcesoSRTHilo, NULL, ejecucionProcesoSRT, NULL);
    pthread_create(&administradorRespuestaCPUHilo, NULL, administradorRespuestaCPU, NULL);
    pthread_join(ejecucionProcesoSRTHilo, NULL);
    pthread_join(administradorRespuestaCPUHilo, NULL);
*/



/*
void ejecucionProcesoSRT(){
	PCB *unPCB;
	ejecutando = 0;
    while(1){
        sem_wait(&nuevoProcesoReady); // Espero a que el P.L.P me avise que hay un proceso en Ready
        if (ejecutando == 0) { // Primera vez que llega un proceso ejecutando estarÃ¡ 0

        	pthread_mutex_lock(&mutexReady);
            if(list_size(procesosReady) > 1){
                log_info(loggerKernel, "Planifico por estimador");
                list_sort(procesosReady, ordenarSRT);
                unPCB = list_remove(procesosReady, 0);
                enviarPCB(socket_dispatch, *unPCB, loggerKernel);
            } else {
            	if(list_size(procesosReady) > 0){
            		unPCB = list_remove(procesosReady, 0);
            		enviarPCB(socket_dispatch, *unPCB, loggerKernel);
            	}
            }
		    pthread_mutex_unlock(&mutexReady);



        	pthread_mutex_lock(&variableEjecutando);
        	ejecutando = 1;
        	pthread_mutex_unlock(&variableEjecutando);

        } else {

            avisar_a_cpu_interrupt();

        }

    }
}

void administradorRespuestaCPU(){
    t_list* respuestaCPU = list_create();
    uint32_t rafagaEjecutada;
    while(1){

    	respuestaCPU = recibirRespuestaCPU(socket_dispatch);

        PCB * unPCB = list_get(respuestaCPU, 0);
        uint32_t motivoRegreso = list_get(respuestaCPU, 1);

        switch(motivoRegreso){
        	case EXIT_PCB:;
        		pthread_mutex_lock(&variableEjecutando);
				ejecutando = 0;
				pthread_mutex_unlock(&variableEjecutando);

        		avisar_a_planificador_LP(unPCB);
        		list_clean(respuestaCPU);
        		break;
        	case IO_PCB:;
    			uint32_t tiempoBloqueo =  list_get(respuestaCPU, 3);
    			rafagaEjecutada = list_get(respuestaCPU, 2);
                estimador(unPCB, config_kernel.alfa, rafagaEjecutada);
                pthread_mutex_lock(&variableEjecutando);
                ejecutando = 0;
                pthread_mutex_unlock(&variableEjecutando);
                pthread_mutex_lock(&mutexBloqueo);
    			list_add(procesosBlocked, unPCB);
    			list_add(tiemposBlocked, tiempoBloqueo);
                pthread_mutex_unlock(&mutexBloqueo);
                sem_post(&procesoBloqueado);
    			list_clean(respuestaCPU);

    	        break;
        	case DESALOJO_PCB:
	       		pthread_mutex_lock(&variableEjecutando);
					ejecutando = 1;
				pthread_mutex_unlock(&variableEjecutando);
				rafagaEjecutada = list_get(respuestaCPU, 2);
				estimador(unPCB, config_kernel.alfa, rafagaEjecutada);

				pthread_mutex_lock(&mutexReady);
				list_add(procesosReady, unPCB);
				list_sort(procesosReady, ordenarSRT);
				pthread_mutex_unlock(&mutexReady);
				enviarPCB(socket_dispatch, *unPCB, loggerKernel);
				sem_post(&nuevoProcesoReady);
				list_clean(respuestaCPU);
        		break;

        }

    }

}
*/
