#include "kernel.h"
#include "utils.h"

void algoritmo_FIFO(){
	t_list* respuestaCPU = list_create();
	while(1){

		sem_wait(nuevoProcesoReady); // Espero a que el P.L.P me avise que hay un proceso en Ready

		sem_wait(mutexReady); 
		pcb * unProceso = list_remove(procesosReady, 0);
		sem_post(mutexReady); 

		//Enviar proceso a CPU
		serilizar_enviar_pcb(socket_cpu_dispatch, unProceso, logger);

        // Espero respuesta del CPU con PCB/Motivo/Bloqueo
        respuestaCPU = recibir_devolucion_cpu(socket_cpu_dispatch);
        unProceso = list_get(respuestaCPU, 0);
        char* motivoDeRegreso =  list_get(respuestaCPU, 2);
		if( motivoDeRegreso == "EXIT" ){
			avisar_a_planificador_LP(unProceso);
			list_clean(respuestaCPU);
		}else if(motivoDeRegreso == "I/O"){
			int tiempoBloqueo =  list_get(respuestaCPU, 3);
            // no creo q necesite un mutex porque yo solo uso esto
			list_add(procesosBlocked, unProceso);
			list_add(tiemposBlocked, tiempoBloqueo);
			list_clean(respuestaCPU);

			signal(procesoBloqueado);
		}
	}
}

void avisar_a_planificador_LP(pcb* pcbFinalizado){ //hilo <--- creo que no seria un hilo sino una funcion auxiliar
    sem_wait(mutexExit);
    list_add(procesosExit, pcbFinalizado);
    sem_post(mutexExit);
}
