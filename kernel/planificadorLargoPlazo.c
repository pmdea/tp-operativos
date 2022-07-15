#include "kernel.h"

void planificador_LargoPlazo(){

    pthread_create(&estadoExitHilo, NULL, estadoExit, NULL);
    pthread_create(&estadoReadyHilo, NULL, estadoReady, NULL);
    pthread_join(estadoReadyHilo, NULL);
    pthread_join(estadoExitHilo, NULL);
}


void estadoReady(){
	char* estadoReady = "Inicializa";
    while(1){
    	usleep(1);
        if(list_size(procesosSuspendedReady) > 0){

            sem_wait(&prioridad_SuspendedReady); // Binario P.M.P
            sem_wait(&grado_multiprogramacion); // El signal lo da el Planificador Mediano Plazo

        }else if ( list_size(procesosNew) > 0){

            sem_wait(&grado_multiprogramacion);
            pcb * nuevoProceso = list_remove(procesosNew, 0);

            //Envio de mensaje a Modulo de Memoria para generar estructuras
            //avisar_a_memoria(socket_memoria, estado, nuevoProceso, loggerKernel);
            //Obtengo las estructuras y se las asigno al PCB
           //nuevoProceso -> tabla_paginas = deserializarInt(socket_memoria);

            pthread_mutex_lock(&mutexReady); // Mutex
            list_add(procesosReady, nuevoProceso);
            pthread_mutex_unlock(&mutexReady);
            log_info(loggerKernel, "Proceso ingreso a LP...");
            sem_post(&nuevoProcesoReady); // Binario P.C.P ---> Aviso que hay un nuevo proceso

            //free(nuevoProceso);


        }
    }
}


void estadoExit(){
	char* estadoExit = "Finaliza";
	pcb* procesoFinalizado;
	while (1){
		usleep(1);
        pthread_mutex_lock(&mutexExit);
        int tamanioExit = list_size(procesosExit);
        pthread_mutex_unlock(&mutexExit);

        if(tamanioExit > 0 ){
            // Obtengo el PCB que finalizo
        	pthread_mutex_lock(&mutexExit);
            pcb * procesoFinalizado = list_remove(procesosExit, 0);
            pthread_mutex_unlock(&mutexExit);
            // Aviso a memoria para que libere
            //avisar_a_memoria(socket_memoria, estado, procesoFinalizado, loggerKernel);

            // Envio el mensaje de finalización

            //avisar_a_consola(procesoFinalizado);

            // Libero memoria
            //free(procesoFinalizado);
            // Incremento el grado de multiprogramacion en 1
            log_info(loggerKernel, "Proceso salio de LP....");
            sem_post(&grado_multiprogramacion);
        }
	}
}
