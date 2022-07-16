#include "kernel.h"

void algoritmo_SRT(){
    pthread_create(&ejecucionProcesoSRTHilo, NULL, ejecucionProcesoSRT, NULL);
    pthread_create(&administradorRespuestaCPUHilo, NULL, administradorRespuestaCPU, NULL);
    pthread_join(ejecucionProcesoSRTHilo, NULL);
    pthread_join(administradorRespuestaCPUHilo, NULL);

}

void ejecucionProcesoSRT(){
	ejecutando = 0;
    while(1){
        sem_wait(&nuevoProcesoReady); // Espero a que el P.L.P me avise que hay un proceso en Ready

        if (ejecutando == 0) { // Primera vez que llega un proceso ejecutando estarÃ¡ 0

        	pthread_mutex_lock(&mutexReady);
		    pcb * unProceso = list_remove(procesosReady, 0);
		    pthread_mutex_unlock(&mutexReady);

            //serilizar_enviar_pcb(socket_cpu_dispatch, unProceso, loggerKernel);

        	pthread_mutex_lock(&variableEjecutando);
        	ejecutando = 1;
        	pthread_mutex_unlock(&variableEjecutando);

        } else {

            //avisar_a_cpu_interrupt();

        }

    }
}

void administradorRespuestaCPU(){
    t_list* respuestaCPU = list_create();
    while(1){

        //respuestaCPU = recibir_devolucion_cpu(socket_cpu_dispatch);
    	pthread_mutex_lock(&variableEjecutando);
    	ejecutando = 0;
    	pthread_mutex_unlock(&variableEjecutando);
        pcb * unProceso = list_get(respuestaCPU, 0);
        char* motivoDeRegreso =  list_get(respuestaCPU, 2);
        int rafagaEjecutada = list_get(respuestaCPU, 1); // Si viene EXIT deberia representarse con (-1)

		if( motivoDeRegreso == "EXIT" ){
			avisar_a_planificador_LP(unProceso);
			list_clean(respuestaCPU);
        }

        if(motivoDeRegreso == "I/O"){
			int tiempoBloqueo =  list_get(respuestaCPU, 3);
            estimador(unProceso, 0.5, rafagaEjecutada);

            wait(mutexBloqueo);
			list_add(procesosBlocked, unProceso);
			list_add(tiemposBlocked, tiempoBloqueo);
            signal(mutexBloqueo);

			list_clean(respuestaCPU);

            signal(procesoBloqueado);

            wait(mutexReady);
            list_sort(procesosReady, ordenarSRT);
            unProceso = list_get(procesosReady, 0);
            signal(mutexReady);

           //serilizar_enviar_pcb(socket_cpu_dispatch, unProceso, loggerKernel);

		}

        if(motivoDeRegreso == "DESALOJO"){
            estimador(unProceso, 0.5, rafagaEjecutada);

            wait(mutexReady);
            list_add(procesosReady, unProceso);
            list_sort(procesosReady, ordenarSRT);
            unProceso = list_get(procesosReady, 0);
            signal(mutexReady);

            list_clean(respuestaCPU);

            //serilizar_enviar_pcb(socket_cpu_dispatch, unProceso, loggerKernel);

        }

    }

}
