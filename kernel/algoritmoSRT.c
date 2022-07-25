#include "kernel.h"

void algoritmo_SRT(){
    pthread_create(&ejecucionProcesoSRTHilo, NULL, ejecucionProcesoSRT, NULL);
    pthread_create(&administradorRespuestaCPUHilo, NULL, administradorRespuestaCPU, NULL);
    pthread_join(ejecucionProcesoSRTHilo, NULL);
    pthread_join(administradorRespuestaCPUHilo, NULL);

}

void ejecucionProcesoSRT(){
	PCB *unPCB;
	ejecutando = 0;
    while(1){
        sem_wait(&nuevoProcesoReady); // Espero a que el P.L.P me avise que hay un proceso en Ready
        log_error(loggerKernel, "NO ROMPI %i", ejecutando);
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
        uint32_t raf = list_get(respuestaCPU, 2);
        uint32_t tb = list_get(respuestaCPU, 3);

        log_warning(loggerKernel, "MOT: %i - RAF: %i - TB: %i", motivoRegreso, raf, tb);

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
				log_info(loggerKernel, "Replanificacion por desalojo");
				rafagaEjecutada = list_get(respuestaCPU, 2);
				estimador(unPCB, config_kernel.alfa, rafagaEjecutada);

				pthread_mutex_lock(&mutexReady);
				list_add(procesosReady, unPCB);
				list_sort(procesosReady, ordenarSRT);
				unPCB = list_get(procesosReady, 0);
				pthread_mutex_unlock(&mutexReady);
				enviarPCB(socket_dispatch, *unPCB, loggerKernel);
				list_clean(respuestaCPU);
        		break;

        }

    }

}

