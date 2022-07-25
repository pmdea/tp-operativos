#include "kernel.h"

void planificador_LargoPlazo(){

    pthread_create(&estadoExitHilo, NULL, estadoExit, NULL);
    pthread_create(&estadoReadyHilo, NULL, estadoReady, NULL);
    pthread_join(estadoReadyHilo, NULL);
    pthread_join(estadoExitHilo, NULL);
}


void estadoReady(){
    int tamanioNew;
    int tamanioSuspendido;
    while(1){
        pthread_mutex_lock(&mutexSuspendido);
        tamanioSuspendido = list_size(procesosSuspendedReady);
        pthread_mutex_unlock(&mutexSuspendido);

        pthread_mutex_lock(&mutexReady);
        tamanioNew = list_size(procesosNew);
        pthread_mutex_unlock(&mutexReady);

        if(tamanioSuspendido > 0){

            sem_wait(&prioridad_SuspendedReady); // Binario P.M.P
            sem_wait(&grado_multiprogramacion); // El signal lo da el Planificador Mediano Plazo

            pthread_mutex_lock(&mutexSuspendido);
            PCB* procesoSuspendido = list_get(procesosSuspendedReady, 0);
            list_add(procesosReady, procesoSuspendido);
            pthread_mutex_unlock(&mutexSuspendido);
            log_info(loggerKernel, "Ingreso el Proceso de ID: %i a Ready por Prioridad de ReadySuspended", procesoSuspendido -> id);

        }else if ( tamanioNew > 0){
            sem_wait(&grado_multiprogramacion);
            // AGREGO ESTA VALIDACION POR SI JUSTO SE TRABA EN EL SEMAFORO
            // LLEGA UN PROCESO A NEW Y SEGUNDOS DESPUES UN SUSPENDIDO
            // PERO COMO YA HABIA VALUADO QUE HABIA UNO EN NEW
            pthread_mutex_lock(&mutexSuspendido);
            tamanioSuspendido = list_size(procesosSuspendedReady);
            pthread_mutex_unlock(&mutexSuspendido);
            if(list_size(procesosSuspendedReady) > 0){
                sem_wait(&prioridad_SuspendedReady); // Binario P.M.P

                pthread_mutex_lock(&mutexSuspendido);
                PCB* procesoSuspendido = list_get(procesosSuspendedReady, 0);
                list_add(procesosReady, procesoSuspendido);
                pthread_mutex_unlock(&mutexSuspendido);
                log_info(loggerKernel, "Ingreso el Proceso de ID: %i a Ready por Prioridad de ReadySuspended", procesoSuspendido -> id);

            } else {
                pthread_mutex_lock(&mutexNew);
                PCB* nuevoProceso = list_remove(procesosNew, 0);
                pthread_mutex_unlock(&mutexNew);
                //Envio de mensaje a Modulo de Memoria para generar estructuras
                //avisar_a_memoria(socket_memoria, INICIALIZA, nuevoProceso, loggerKernel);
                //Obtengo las estructuras y se las asigno al PCB
                //nuevoProceso -> tabla_paginas = deserializarInt32(socket_memoria);

                pthread_mutex_lock(&mutexReady); // Mutex
                list_add(procesosReady, nuevoProceso);
                pthread_mutex_unlock(&mutexReady);
                log_info(loggerKernel, "Ingreso el Proceso de ID: %i a Ready", nuevoProceso -> id);
                sem_post(&nuevoProcesoReady); // Binario P.C.P ---> Aviso que hay un nuevo proceso

                //free(nuevoProceso);
            }
        }
    }
}


void estadoExit(){
	PCB* procesoFinalizado;
	while (1){
		sem_wait(&finalizoProceso);
        pthread_mutex_lock(&mutexExit);
        int tamanioExit = list_size(procesosExit);
        pthread_mutex_unlock(&mutexExit);

        if(tamanioExit > 0 ){
            // Obtengo el PCB que finalizo
        	pthread_mutex_lock(&mutexExit);
            procesoFinalizado = list_remove(procesosExit, 0);
            pthread_mutex_unlock(&mutexExit);
            // Aviso a memoria para que libere
            //avisar_a_memoria(socket_memoria, FINALIZA, procesoFinalizado, loggerKernel);

            // Envio el mensaje de finalización

            //avisar_a_consola(procesoFinalizado);

            // Libero memoria
            //free(procesoFinalizado);
            // Incremento el grado de multiprogramacion en 1
            log_info(loggerKernel, "Finalizo correctamente el Proceso de ID: %i", procesoFinalizado -> id);
            sem_post(&grado_multiprogramacion);
        }
	}
}
