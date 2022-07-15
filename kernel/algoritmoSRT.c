#include "kernel.h"
#include "utils.h"

void algoritmo_SRT(){
    pthread_t administradorRespuestasCPU;
    pthread_create(&administradorRespuestaCPU, NULL, (void *) administradorRespuestaCPU(), NULL);
    pthread_detach(administradorRespuestaCPU);

    while(1){
        sem_wait(nuevoProcesoReady); // Espero a que el P.L.P me avise que hay un proceso en Ready
        
        if (ejecutando == 0) { // Primera vez que llega un proceso ejecutando estará 0

		    sem_wait(mutexReady); 
		    pcb * unProceso = list_remove(procesosReady, 0);
		    sem_post(mutexReady); 

            serilizar_enviar_pcb(socket_cpu_dispatch, unProceso, logger);

            ejecutando = 1;

        } else {
            
            avisar_a_cpu_interrupt();

        }



    }
}

void administradorRespuestaCPU(){
    t_list* respuestaCPU = list_create();
    while(1){

        respuestaCPU = recibir_devolucion_cpu(socket_cpu_dispatch);
        pcb * unProceso = list_get(respuestaCPU, 0);
        char* motivoDeRegreso =  list_get(respuestaCPU, 2);
        int rafagaEjecutada = list_get(respuesta, 1); // Si viene EXIT deberia representarse con (-1)

		if( motivoDeRegreso == "EXIT" ){
			avisar_a_planificador_LP(unProceso);
			list_clean(respuestaCPU);
        }

        if(motivoDeRegreso == "I/O"){
			int tiempoBloqueo =  list_get(respuestaCPU, 3);
            estimador(unProceso, 0,5, rafagaEjecutada);

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

            void serilizar_enviar_pcb(socket_cpu_dispatch, unProceso, logger);

		}

        if(motivoDeRegreso == "DESALOJO"){
            estimador(unProceso, 0,5, rafagaEjecutada);

            wait(mutexReady);
            list_add(procesosReady, unProceso);
            list_sort(procesosReady, ordenarSRT);
            unProceso = list_get(procesosReady, 0);
            signal(mutexReady);

            list_clean(respuestaCPU);

            void serilizar_enviar_pcb(socket_cpu_dispatch, unProceso, logger);

        }

    }

}




/* ANTERIOR
void algoritmo_SRT(){

	 // a mati pone un proceso en ready

	 * join thread desalojo_PCB()
	 * join thread respuesta_cpu()

}
*/


/* ************************** ANTERIOR SRT ********************************

void desalojo_PCB() {
	while(1) {
        sem_wait(algo) //  Me bloquea si el cpu me manda un mensaje : )

		//sem_wait(agregarAReady)  mientras la lee no quiero q cambie pensar si es necesario
		int enReady = list_size(procesosReady); // Procesos en ready [ B , J ]
		//sem_post(agregarAReady)

        //ejecutando variable global un bool  [ a , b]
         *
		wait(nuevoProcesoReady) binario

        if(enReady == 1 && ejecutando == 0){ // la primera vez q ejecuta [ ]

            //EJECUTANDO...
            sem_wait(agregarAReady);
            pcb *pcbExecute = list_remove(procesosReady, 0);
            sem_post(agregarAReady);

			// MANDO EL PCB POR CONEXION dispatch unProceso
            enviar_paquete(pcbExecute, cpu_Dispatch); // serializar
            ejecutando = 1;
            //EJECUTANDO... del cpu

            free(pcbExecute);

            sem_post(algo) // 1
        }else if (enReady >= 1 && ejecutando == 1){ // si llega un nuevo proceso y esta ejecutando

            enviar_paquete('nuevo proceso', INTERRUP);
            // wait(respuestaCpu)
			paquete = recibirPaquete(dispatch); // deserializar
            int rafagas_executed = paquete->rafagas_executed;
            pcb *pcbExecuted = paquete->pcb;

            estimador(pcbExecuted, 0,5, rafagas_executed); // actualizo pcb

            sem_wait(agregarAReady);
            list_add(procesosReady, pcbExecuted);
            sem_post(agregarAReady);

            list_sort(procesosReady, ordenarSRT); // tomo lista de ready y ordeno con todos los procesos

            free(pcbExecuted);

            //EJECUTANDO...
            sem_wait(agregarAReady);
            pcb *pcbExecute = list_remove(procesosReady, 0);
            sem_post(agregarAReady);

			// MANDO EL PCB POR CONEXION dispatch unProceso
            enviar_paquete(pcbExecute, cpu_Dispatch); // serializar
            ejecutando = 1;
            //EJECUTANDO...

            free(pcbExecute);

            sem_post(algo)
        }

        sem_post(algo)
    }
}

void respuesta_cpu(){
	administrar_bloqueos(); // HILO
    while(1){
        sem_wait(respuestaCpu);
        sem_wait(algo); // no me acuerdo para q era esto @Mati
        pcb * pcb;
        paquetedeCPU = recibir_paquete(conexion_dispatch); // PCB + Tiempo
        pcb = list_get(paquetedeCPU, 1);
        char instruccion =  list_get(paquetedeCPU, 2);
        int tiempo =  list_get(paquetedeCPU, 3);

        if(instruccion == "EXIT"){
            finalizar_pcb(paquetedeCPU);
        }
        if(instruccion == "I/O"){
            list_add(procesosBlocked, paquete->pcb); // EL PRIMERO ES EL Q ESTA BLOQUEADO
			list_add(tiemposBlocked, tiempo);
        }
        sem_post(algo)
    }
}

************************** ANTERIOR SRT FIN ******************************** */