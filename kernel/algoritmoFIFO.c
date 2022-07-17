#include "kernel.h"

void algoritmo_FIFO(){
    t_list* respuestaCPU = list_create();
    while(1){
    	usleep(1);
        sem_wait(&nuevoProcesoReady); // Espero a que el P.L.P me avise que hay un proceso en Ready

        pthread_mutex_lock(&mutexReady);
        pcb * unProceso = list_remove(procesosReady, 0);
        pthread_mutex_unlock(&mutexReady);

        //Enviar proceso a CPU
        //serilizar_enviar_pcb(socket_cpu_dispatch, unProceso, loggerKernel);

        // Espero respuesta del CPU con PCB/Motivo/Bloqueo
       // respuestaCPU = recibir_devolucion_cpu(socket_cpu_dispatch);
        //unProceso = list_get(respuestaCPU, 0);
        //char* motivoDeRegreso =  list_get(respuestaCPU, 2);
        char* motivoDeRegreso = "I/O";
        if( motivoDeRegreso == "EXIT" ){
            avisar_a_planificador_LP(unProceso);
            list_clean(respuestaCPU);
        }else if(motivoDeRegreso == "I/O"){
            //int tiempoBloqueo =  list_get(respuestaCPU, 3);
        	int tiempoBloqueo = 2000000;
            // no creo q necesite un mutex porque yo solo uso esto
            list_add(procesosBlocked, unProceso);
            list_add(tiemposBlocked, tiempoBloqueo);
            list_clean(respuestaCPU);

            sem_post(&procesoBloqueado);
        }
    }
}

void avisar_a_planificador_LP(pcb* pcbFinalizado){ //hilo <--- creo que no seria un hilo sino una funcion auxiliar
	pthread_mutex_lock(&mutexExit);
    list_add(procesosExit, pcbFinalizado);
    pthread_mutex_unlock(&mutexExit);
    sem_post(&finalizoProceso);
}
