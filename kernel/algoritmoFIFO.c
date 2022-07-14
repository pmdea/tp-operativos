#include "kernel.h"
#include "utils.h"

/* ********************************FIFO NUEVO ************************************
void algoritmo_FIFO(){
	t_list* respuestaCPU = list_create();
	while(1){

		sem_wait(nuevoProcesoReady) // Espero a que el P.L.P me avise que hay un proceso en Ready
			pcb * unProceso;

			//sem_wait(agregarAReady) QUIZA --> Creo q no es necesario
			unProceso = list_remove(procesosReady, 0);
			//sem_post(agregarAReady) QUIZA --> Creo q no es necesario

			MANDO EL PCB POR CONEXION dispatch unProceso
			serializar_enviar_pcb(unProceso, socket_cpu_dispatch);
			//espero mensaje de cpu (un exit, bloqueo, desalojo)


            // lo que me devuelve CPU
            respuestaCPU = recibir_paquete(socket_cpu_dispatch);
            unProceso = list_get(respuestaCPU, 0);
            char* instruccion =  list_get(respuestaCPU, 1);

			if( instruccion == 'EXIT' ){
				avisar_a_planificador_LP(unProceso);
				list_clean(respuestaCPU);
			}else if(instruccion == 'I/O'){
				int tiempoBloqueo =  list_get(respuestaCPU, 2);
                // no creo q necesite un mutex porque yo solo uso esto
				list_add(procesosBlocked, unProceso);
				list_add(tiemposBlocked, tiempoBloqueo);
				list_clean(respuestaCPU);
			}
		//}
	}
}

void avisar_a_planificador_LP(pcb* pcbFinalizado){ //hilo <--- creo que no seria un hilo sino una funcion auxiliar
    //sem_wait(procesoExit)
    list_add(procesosExit, pcbFinalizado);
    //sem_post(procesoExit)
}



 **********************************FIN FIFO NUEVO ************************************** */