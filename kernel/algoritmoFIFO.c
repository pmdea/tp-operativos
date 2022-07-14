#include "kernel.h"
#include "utils.h"

/* ********************************FIFO NUEVO ************************************
void algoritmo_FIFO(){
	administrar_bloqueos(); // HILO
	while(1){

		sem_wait(nuevoProcesoReady) // a mati pone un proceso en ready
		//if(enReady > 0){ // es necesario este if ? teniendo el wait arriba ???? si es q sea necesario agregar int enReady = list_size(procesosReady);
			pcb * unProceso;

			//sem_wait(agregarAReady) QUIZA
			unProceso = list_remove(procesosReady, 0);
			//sem_post(agregarAReady) QUIZA

			MANDO EL PCB POR CONEXION dispatch unProceso

			//espero mensaje de cpu (un exit, bloqueo, desalojo)
			//sem_wait(respuestaCpu)

			paqueteCPU = recibirPaquete(socket_memoria);

            // lo que me devuelve CPU
            pcb * pcb;
            paquetedeCPU = recibir_paquete(conexion_dispatch);
            pcb = list_get(paquetedeCPU, 1);
            char instruccion =  list_get(paquetedeCPU, 2);
            int tiempo =  list_get(paquetedeCPU, 3);

			if( instruccion == 'exit' ){
				finalizar_pcb(paquete->pcb);
			}else if(instruccion == 'i/o'){
                // no creo q necesite un mutex porque yo solo uso esto
				list_add(procesosBlocked, pcb);
				list_add(tiemposBlocked, tiempo);
			}
		//}
	}
}

void finalizar_pcb(pcb * pcbFinalizar){ //hilo
    //sem_wait(procesoExit)
    list_add(procesosExit, pcbFinalizar);
    //sem_post(procesoExit)
}



 **********************************FIN FIFO NUEVO ************************************** */