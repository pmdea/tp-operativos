#include "kernel.h"

// REDISEÑO LARGO PLAZO
void planificador_LargoPlazo(){

    pthread_create(&estadoExitHilo, NULL, estadoExit, NULL);
    pthread_create(&estadoReadyHilo, NULL, estadoReady, NULL);
    pthread_join(estadoReadyHilo, NULL);
    pthread_join(estadoExitHilo, NULL);
}

void estadoReady(){
	int enNew, enSuspendedReady, enReady;
	PCB* unProceso;
	while(1){
		sem_wait(&hayProcesoAnalizar);
		pthread_mutex_lock(&mutexSuspendido);
		enSuspendedReady = list_size(procesosSuspendedReady);
		pthread_mutex_unlock(&mutexSuspendido);

		pthread_mutex_lock(&mutexNew);
		enNew = list_size(procesosNew);
		pthread_mutex_unlock(&mutexNew);

		switch(enSuspendedReady){
			case 0:
				if(enNew > 0){
					sem_wait(&grado_multiprogramacion); // Inicializa en 4
					pthread_mutex_lock(&mutexNew);
					unProceso = list_remove(procesosNew, 0);
					pthread_mutex_unlock(&mutexNew);

					avisar_a_memoria(INICIALIZA, *unProceso, loggerKernel);
					unProceso -> tabla_paginas = deserializarInt32(socket_memoria);

					pthread_mutex_lock(&mutexReady);
					list_add(procesosReady, unProceso);
					enReady = list_size(procesosReady);
					pthread_mutex_unlock(&mutexReady);

					sem_post(&nuevoProcesoReady);
					pthread_mutex_lock(&variableEjecutando);
					log_warning(loggerKernel, "SIZE %i EJECUTANDO %i", enReady, ejecutando);
					if(string_contains("SRT", config_kernel.algoritmo_planificacion) && (enReady >= 0 && ejecutando == 1)){
					                sem_post(&enviarInterrupcion);
					}
					pthread_mutex_unlock(&variableEjecutando);

					log_info(loggerKernel, "INGRESO PROCESO ID %i A READY DESDE NEW", unProceso -> id);
				}
				break;
			default:
				if(enSuspendedReady > 0){
					sem_wait(&grado_multiprogramacion); // Inicializa en 4
					pthread_mutex_lock(&mutexSuspendido);
					unProceso = list_remove(procesosSuspendedReady, 0);
					pthread_mutex_unlock(&mutexSuspendido);

					pthread_mutex_lock(&mutexReady);
					list_add(procesosReady, unProceso);
					enReady = list_size(procesosReady);
					pthread_mutex_unlock(&mutexReady);

					sem_post(&nuevoProcesoReady);

					pthread_mutex_lock(&variableEjecutando);
					if(string_contains("SRT", config_kernel.algoritmo_planificacion) && (enReady >= 0 && ejecutando == 1)){
					                sem_post(&enviarInterrupcion);
					}
					pthread_mutex_unlock(&variableEjecutando);
					log_info(loggerKernel, "INGRESO PROCESO ID %i A READY DESDE READY-SUSPENDED", unProceso -> id);
				}
		}
	}
}

void estadoExit(){
	PCB* unProceso;
	while(1){
		sem_wait(&finalizoProceso); // Inicializa en 0 (Me avisa C)

		pthread_mutex_lock(&mutexExit);
		unProceso = list_remove(procesosExit, 0);
		pthread_mutex_unlock(&mutexExit);

		avisar_a_memoria(FINALIZA, *unProceso, loggerKernel);
		uint32_t retorno = deserializarInt32(socket_memoria);

		log_info(loggerKernel, "PROCESO ID %i FINALIZO - AVISANDO A CONSOLA", unProceso -> id);
		avisar_a_consola(unProceso);
		sem_post(&grado_multiprogramacion);
		sem_post(&hayProcesoAnalizar);
	}
}
