#include "kernel.h"
void algoritmo_FIFO(){
    t_list* respuestaCPU = list_create();
    while(1){
    	usleep(1);
        sem_wait(&nuevoProcesoReady); // Espero a que el P.L.P me avise que hay un proceso en Ready

        sem_wait(&mutexReady);
        pcb * unProceso = list_remove(procesosReady, 0);
        sem_post(&mutexReady);

        //Enviar proceso a CPU
        //serilizar_enviar_pcb(socket_cpu_dispatch, unProceso, loggerKernel);

        // Espero respuesta del CPU con PCB/Motivo/Bloqueo
       // respuestaCPU = recibir_devolucion_cpu(socket_cpu_dispatch);
        //unProceso = list_get(respuestaCPU, 0);
        //char* motivoDeRegreso =  list_get(respuestaCPU, 2);
        char* motivoDeRegreso = "EXIT";
        if( motivoDeRegreso == "EXIT" ){
        	log_info(loggerKernel, "ESTOY EN FIFO - EXIT");
        	log_info(loggerKernel,"PCB ID %i ", unProceso -> id);
            avisar_a_planificador_LP(unProceso);
            log_info(loggerKernel,"TOTAL LISTA %i ", list_size(procesosExit));
            list_clean(respuestaCPU);
        }else if(motivoDeRegreso == "I/O"){
            int tiempoBloqueo =  list_get(respuestaCPU, 3);
            // no creo q necesite un mutex porque yo solo uso esto
            list_add(procesosBlocked, unProceso);
            list_add(tiemposBlocked, tiempoBloqueo);
            list_clean(respuestaCPU);

            sem_post(&procesoBloqueado);
        }
    }
}

void avisar_a_planificador_LP(pcb* pcbFinalizado){ //hilo <--- creo que no seria un hilo sino una funcion auxiliar
    sem_wait(&mutexExit);
    list_add(procesosExit, pcbFinalizado);
    sem_post(&mutexExit);
    log_info(loggerKernel, "AGREGUE PCB A EXIT");
}
