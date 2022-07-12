#include "kernel.h"
#include "utils.h"


int main(void)
{
	t_log* logger;
	t_config* config;
	logger = iniciar_logger_kernel();
	log_info(logger, "Modulo Kernel");
	config = iniciar_config_kernel(); // Traer datos del archivo de configuracion

	// Semaforos
	sem_init(&grado_multiprogramacion, 0, config_kernel.grado_multiprogramacion);
	sem_init(&prioridad_SuspendedReady, 0, 0);
	sem_init(&nuevoProcesoReady, 0, 0);

	// Listas
	procesosNew = list_create();
	procesosReady = list_create();
	procesosExecute = list_create();
	procesosBlocked = list_create();
	tiemposBlocked = list_create();
	procesosSuspendedBlocked = list_create();
	procesosSuspendedReady = list_create();
	procesosExit = list_create();
	conexiones_pcb = list_create();
	bool ejecutando  = 0 ;
	// lo puse en el de desalojo paquetedeCPU_Desalojo = list_create();
	// paquetedeCPU_Analisis = list_create();

	// Hilos
	pthread_create(&planificadorLargoPlazo, NULL, (void *) planificador_LargoPlazo, NULL);
	pthread_create(&planificadorMedianoPlazo, NULL, (void *) planificador_MedianoPlazo, NULL);
	pthread_create(&planificadorCortoPlazo, NULL, (void *) planificador_CortoPlazo, NULL);

	pthread_detach(planificadorLargoPlazo);
	pthread_detach(planificadorMedianoPlazo);
	pthread_detach(planificadorCortoPlazo);


}


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

void administrar_bloqueos(){ // hilo

    int enBlock = list_size(procesosBlocked);
    if(enBlock > 0){
        pcb * pcb = list_remove(procesosBlocked, 0);
        int tiempo = list_remove(tiemposBlocked, 0);

        if(tiempo > config_kernel.tiempo_maximo_bloqueado){
            tiemposBlockedSuspended = tiempo - config_kernel.tiempo_maximo_bloqueado
            tiempo = config_kernel.tiempo_maximo_bloqueado
        }

        log_info(logger, "Iniciando bloqueo de %d........",tiempo);
        usleep(tiempo);
        log_info(logger, "Finalizando bloqueo");

        if(tiemposBlockedSuspended > 0){
            list_add(procesosSuspendedBlocked, pcb)
            sem_post(bloqueoMax) // le digo al mediano q hay un proceso en procesosSupendedBlocked
        }else{
            sem_wait(agregarAReady)
            list_add(procesosReady,pcb);
            sem_post(agregarAReady);
        }
        free(pcb)
    }
 }

 **********************************FIN FIFO NUEVO ************************************** */


/*
void algoritmo_SRT(){

	 // a mati pone un proceso en ready

	 * join thread desalojo_PCB()
	 * join thread respuesta_cpu()

}
*/


/* ************************** NUEVO SRT ********************************

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

************************** NUEVO SRT FIN ******************************** */

// SRT Analisis // variable global => conexion_dispatch
 *
 *void interrupcion_io_PCB(){
	 //

 *	paquetedeCPU_Analisis = list_create();
 *	int tiempoBloqueado;
 *	int indice;
 *	t_list* tiempo_bloqueado;
 *	char** string;
 *
 *	while(1){
 *		wait(Esperar_CPU);
 *		pcb * pcb_actualizado;
 *		paquetedeCPU_SoloAnalisis = recibir_paquete(conexion_dispatch); // PCB + Tiempo
 *		pcb_actualizado = list_get(paquetedeCPU_SoloAnalisis, 1);
 *		indice = pcb_actualizado -> program_counter - 1;
 *		if(indice < 0){
 *			indice = 0;
 *		}
 *		string = list_get(pcb_actualizar->instrucciones, indice);
 *		char** split = string_split(string, " ");
 *
 *		if(split[0] == "EXIT"){
 *			WAIT SEM MANEJO LISTAS
 *			list_add(procesosExit, pcb_actualizado);
 *			list_remove(procesosExec, 0);
 *			SIGNAL SEM MANEJO LISTAS
 *		}
 *
 *		if(split[0] == "I/O"){
 *			WAIT SEM MANEJO LISTAS
 *			list_add(procesosBlocked, pcb_actualizado);
 *			list_remove(procesosExecute, 0);
 *			SIGNAL SEM MANEJO LISTAS
 *			tiempoBloqueado = list_get(paquetedeCPU_SoloAnalisis, 1) + 1;
 *			list_add(tiempo_bloqueado, tiempoBloqueado);
 *		}
 *
 *		tiempoBloqueado = list_get(tiempoABlockear, 0); // 0
 *		if(tiempoBloqueado){ // si
 *			if(tiempoBloqueado == 0){ // si
 *
 *				// SEMAFORO  CON EL DE LARGO PLAZO MUTEX sem_wait(agregarAReady)
 *				list_add(procesosReady, list_remove(procesosBlocked, 0)); // [ a, c ] => [ c ] ** devuelve un pcb
 *				// SEMAFORO  CON EL DE LARGO PLAZO MUTEX sem_post(agregarAReady)
 *
 *				// se termino el bloqueo por lo tanto lo meto devuelta en la lista de ready
 *				list_remove(tiempo_bloqueado, 0); // [ 0, 4 ] => [ 4 ]
 *
 *			}
 *			list_replace(tiempo_bloqueado, 0, tiempoBloqueado--); // [ 3 ]
 * 		}
 *
 *
 *}
 *

*/




