#include "kernel.h"

void planificador_LargoPlazo(){
    char* estado = "";
    int i = 0;
    while(1){
        if(list_size(procesosSuspendedReady) > 0){

            sem_wait(&prioridad_SuspendedReady); // Binario P.M.P
            sem_wait(&grado_multiprogramacion); // El signal lo da el Planificador Mediano Plazo

        }else if ( list_size(procesosNew) > 0){

            sem_wait(&grado_multiprogramacion);

            pcb * nuevoProceso = list_remove(procesosNew, 0);

            //Envio de mensaje a Modulo de Memoria para generar estructuras
            estado = "Inicializa";
            //avisar_a_memoria(socket_memoria, estado, nuevoProceso, loggerKernel);
            //Obtengo las estructuras y se las asigno al PCB
           //nuevoProceso -> tabla_paginas = deserializarInt(socket_memoria);

            sem_wait(&mutexReady); // Mutex
            list_add(procesosReady, nuevoProceso);
            sem_post(&mutexReady);
            sem_post(&nuevoProcesoReady); // Binario P.C.P ---> Aviso que hay un nuevo proceso

            //free(nuevoProceso);

            log_info(loggerKernel, "Proceso ingreso a LP");
        }
        if(list_size(procesosExit) > 0 ){

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

}
