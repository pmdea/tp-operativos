#include "kernel.h"

void planificador_CortoPlazo(){
    char* algoritmo = config_kernel.algoritmo_planificacion;
    pthread_create(&administradorBloqueosHilo, NULL, administrar_bloqueos, NULL);
    // int ciclosTotales=0; NO SE SI VA


    if(string_contains("FIFO", algoritmo)){
    	log_info(loggerKernel, "INICIANDO ALGORITMO FIFO");
    	pthread_create(&ejecucionAlgoritmoHilo, NULL, algoritmo_FIFO, NULL);
        //algoritmo_FIFO();
    }else{
    	log_info(loggerKernel, "INICIANDO ALGORITMO SRT");
    	pthread_create(&ejecucionAlgoritmoHilo, NULL, algoritmo_SRT, NULL);
        //algoritmo_SRT();
    }


    pthread_join(administradorBloqueosHilo, NULL);
    pthread_join(ejecucionAlgoritmoHilo, NULL);
}


void administrar_bloqueos(){
	blockedPCB* datosPCB;
	blockedPCB* datosAuxiliar;
	PCB* unProceso;
	PCB* unProcesoAuxiliar;
	int tiempoBloqueo, cantidadBloqueados, enReady;
	while(1){
		sem_wait(&procesoBloqueado);

		pthread_mutex_lock(&mutexBloqueo);
		datosPCB = list_remove(procesosBlocked, 0);
		pthread_mutex_unlock(&mutexBloqueo);
		unProceso = datosPCB -> unPCB;
		tiempoBloqueo = datosPCB -> tiempo;

		log_info(loggerKernel, "BLOQUEANDO PROCESO ID %i - TIEMPO BLOQUEO %i",unProceso -> id, tiempoBloqueo);
		for(int k = 0; k < (tiempoBloqueo/1000); k++){
			datosPCB -> aux += 1000;
			if(datosPCB -> aux > config_kernel.tiempo_maximo_bloqueado && datosPCB -> suspendido == 0){
				avisar_a_memoria(SUSPENDE, *datosPCB -> unPCB ,loggerKernel);
				uint32_t retorno = deserializarInt32(socket_memoria);
				sem_post(&grado_multiprogramacion);
				sem_post(&hayProcesoAnalizar);
				datosPCB -> suspendido = 1;
			}

			usleep(1000*1000);



			pthread_mutex_lock(&mutexBloqueo);
			cantidadBloqueados = list_size(procesosBlocked);
			pthread_mutex_unlock(&mutexBloqueo);

			if(cantidadBloqueados > 0){
				for(int j = 0 ; j < cantidadBloqueados; j++){
					datosAuxiliar = list_get(procesosBlocked, j);

					if(datosAuxiliar -> aux != -1){
						datosAuxiliar -> aux += 1000;

						if(datosAuxiliar -> aux > config_kernel.tiempo_maximo_bloqueado && datosAuxiliar -> suspendido == 0){
							avisar_a_memoria(SUSPENDE, *datosAuxiliar -> unPCB ,loggerKernel);
							uint32_t retorno = deserializarInt32(socket_memoria);
							sem_post(&grado_multiprogramacion);
							sem_post(&hayProcesoAnalizar);
							datosAuxiliar -> aux = -1;
							datosAuxiliar -> suspendido = 1;
						}
					}
					list_replace(procesosBlocked, j, datosAuxiliar);

				} // TERMINA FOR SECUNDARIO
			}

		}// ACA TERMINA FOR PRINCIPAL
		log_info(loggerKernel, "FINALIZO BLOQUEO PROCESO ID %i", unProceso -> id);

		switch(datosPCB -> suspendido){
			case 1:
		        pthread_mutex_lock(&mutexSuspendido);
		        list_add(procesosSuspendedReady,unProceso);
		        pthread_mutex_unlock(&mutexSuspendido);
		        sem_post(&hayProcesoAnalizar);
				break;
			case 0:
				pthread_mutex_lock(&mutexReady);
				list_add(procesosReady,unProceso);
				enReady = list_size(procesosReady);
				pthread_mutex_unlock(&mutexReady);
				sem_post(&nuevoProcesoReady);
				pthread_mutex_lock(&variableEjecutando);
				if(string_contains("SRT", config_kernel.algoritmo_planificacion) && (enReady >= 0 && ejecutando == 1)){
					sem_post(&enviarInterrupcion);
				}
				pthread_mutex_unlock(&variableEjecutando);
				break;
		}
	}
}


