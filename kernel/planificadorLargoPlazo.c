#include "kernel.h"
#include "utils.h"

void planificadorLargoPlazo();

void planificadorLargoPlazo(){
	char* estado = "";
	while(1){
		if(list_size(procesosSuspendedReady) > 0){

			wait(prioridad_SuspendedReady); // Binario P.M.P
			wait(grado_multiprogramacion); // El signal lo da el Planificador Mediano Plazo

		}else if ( list_size(procesoNew) > 0){

			wait(grado_multiprogramacion);

			pcb * nuevoProceso = list_remove(procesosNew, 0);
			
			//Envio de mensaje a Modulo de Memoria para generar estructuras
			char* estado = "Inicializa";
			avisar_a_memoria(socket_memoria, estado, nuevoProceso, logger);
			//Obtengo las estructuras y se las asigno al PCB
			nuevoProceso -> tabla_paginas = deserializarInt(socket_memoria);

			wait(mutexReady); // Mutex
			list_add(procesosReady, nuevoProceso);
			signal(mutexReady); 
			signal(nuevoProcesoReady); // Binario P.C.P ---> Aviso que hay un nuevo proceso

			//free(nuevoProceso);
		}
		if(list_size(procesosExit) > 0 ){

			// Obtengo el PCB que finalizo
            sem_wait(mutexExit);
			pcb * procesoFinalizado = list_remove(procesosExit, 0);
			sem_post(mutexExit);
			// Aviso a memoria para que libere
			char* estado = "Finaliza";
			avisar_a_memoria(socket_memoria, estado, nuevoProceso, logger);

			// Envio el mensaje de finalización

			avisar_a_consola(procesoFinalizado);
			
			// Libero memoria
			//free(procesoFinalizado);
			
			// Incremento el grado de multiprogramacion en 1
			signal(grado_multiprogramacion);
		}

	}
}