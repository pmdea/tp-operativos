#include "kernel.h"

void planificador_LargoPlazo(){
    char* estado = "";
    while(1){
    	usleep(1);
    	log_info(loggerKernel,"INFINITO");
        if(list_size(procesosSuspendedReady) > 0){

            sem_wait(&prioridad_SuspendedReady); // Binario P.M.P
            sem_wait(&grado_multiprogramacion); // El signal lo da el Planificador Mediano Plazo

        }else if ( list_size(procesosNew) > 0){

            sem_wait(&grado_multiprogramacion);
            log_info(loggerKernel,"ENTRE A NEW");
            pcb * nuevoProceso = list_remove(procesosNew, 0);

            //Envio de mensaje a Modulo de Memoria para generar estructuras
            estado = "Inicializa";
            //avisar_a_memoria(socket_memoria, estado, nuevoProceso, loggerKernel);
            //Obtengo las estructuras y se las asigno al PCB
           //nuevoProceso -> tabla_paginas = deserializarInt(socket_memoria);

            sem_wait(&mutexReady); // Mutex
            list_add(procesosReady, nuevoProceso);
            sem_post(&mutexReady);
            log_info(loggerKernel, "Proceso ingreso a LP");
            sem_post(&nuevoProcesoReady); // Binario P.C.P ---> Aviso que hay un nuevo proceso

            //free(nuevoProceso);


        }
        sem_wait(&mutexExit);
        int tamanioExit = list_size(procesosExit);
        sem_post(&mutexExit);
        log_info(loggerKernel, "TAMANIO LISTA LISADSADASD %i ", tamanioExit);

        if(tamanioExit > 0 ){
        	log_info(loggerKernel, "SALIO UN PROCESO");
            // Obtengo el PCB que finalizo
            sem_wait(&mutexExit);
            pcb * procesoFinalizado = list_remove(procesosExit, 0);
            sem_post(&mutexExit);
            // Aviso a memoria para que libere
            estado = "Finaliza";
            //avisar_a_memoria(socket_memoria, estado, procesoFinalizado, loggerKernel);

            // Envio el mensaje de finalización

            //avisar_a_consola(procesoFinalizado);

            // Libero memoria
            //free(procesoFinalizado);
            // Incremento el grado de multiprogramacion en 1
            sem_post(&grado_multiprogramacion);
        }
    }
    log_info(loggerKernel,"TERMINE DE EJECUTAR");

}
