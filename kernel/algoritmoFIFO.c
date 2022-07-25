#include "kernel.h"

void algoritmo_FIFO(){
    t_list* respuestaCPU = list_create();
    while(1){
        sem_wait(&nuevoProcesoReady); // Espero a que el P.L.P me avise que hay un proceso en Ready
        pthread_mutex_lock(&mutexReady);
        PCB * unProceso = list_remove(procesosReady, 0);
        pthread_mutex_unlock(&mutexReady);
        //Enviar proceso a CPU
        enviarPCB(socket_dispatch, *unProceso, loggerKernel);

        log_info(loggerKernel, "ESPERANDO RESPUESTA");

        // Espero respuesta del CPU con PCB/Motivo/Bloqueo
        //respuestaCPU = recibirRespuestaCPU(socket_dispatch);
        //unProceso = list_get(respuestaCPU, 0);
        unProceso = deserializarPCB(socket_dispatch);
        //uint32_t motivoRegreso = list_get(respuestaCPU, 1);
        uint32_t motivoRegreso = IO_PCB;
        //uint32_t raf = list_get(respuestaCPU, 2);
        //uint32_t tb = list_get(respuestaCPU, 3);

        //log_warning(loggerKernel, "MOT: %i - RAF: %i - TB: %i", motivoRegreso, raf, tb);

        switch(motivoRegreso){
        	case EXIT_PCB:;
        		avisar_a_planificador_LP(unProceso);
        		break;
        	case IO_PCB:;
        		//int tiempoBloqueo =  list_get(respuestaCPU, 3);
        		int tiempoBloqueo =  5000;
        	    pthread_mutex_lock(&mutexBloqueo);
        	    list_add(procesosBlocked, unProceso);
        	    list_add(tiemposBlocked, tiempoBloqueo);
        	    pthread_mutex_unlock(&mutexBloqueo);
    	        sem_post(&procesoBloqueado);
    	        break;
        }
    }
}

void avisar_a_planificador_LP(PCB* pcbFinalizado){ //hilo <--- creo que no seria un hilo sino una funcion auxiliar
	pthread_mutex_lock(&mutexExit);
    list_add(procesosExit, pcbFinalizado);
    pthread_mutex_unlock(&mutexExit);
    sem_post(&finalizoProceso);
}
