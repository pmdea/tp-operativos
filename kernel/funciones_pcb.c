#include "kernel.h"

void generar_PCB(int idPCB, t_proceso* proceso){ // Funcion para cargar los datos del proceso al PCB
    pcb *nuevoProceso = malloc(sizeof(pcb));
    nuevoProceso -> id = idPCB;
    nuevoProceso -> tamanio = proceso -> tamanio_proceso;
    nuevoProceso -> instrucciones = list_create(); // Lista proveniente de consola
    nuevoProceso ->  program_counter = 0;
    nuevoProceso ->  tabla_paginas = 0; // Esta como (0) por no tener una tabla asignada.
    nuevoProceso ->  estimacion_rafaga = config_kernel.estimacion_inicial;

    list_add_all(nuevoProceso -> instrucciones, proceso -> instrucciones -> elements);

	pthread_mutex_lock(&mutexNew);
    list_add(procesosNew, nuevoProceso);
	pthread_mutex_unlock(&mutexNew);

	log_info(loggerKernel,"TAMANIO INST %i ", list_size(nuevoProceso -> instrucciones));

    log_info(loggerKernel, "Se agrego correctamente el Proceso de ID: %i", nuevoProceso -> id);
    //free(nuevoProceso); Si lo descomento rompe la funcion y altera los valores
}

bool ordenarSRT(pcb* unPCB,pcb* otroPCB){
    double est1 = unPCB->estimacion_rafaga;
    double est2 = otroPCB->estimacion_rafaga;
    return est2 > est1;
}

void estimador(pcb* unPCB, double alfa, int rafaga_ejecutada){
    unPCB -> estimacion_rafaga = (alfa * rafaga_ejecutada + (1 - alfa) * unPCB->estimacion_rafaga);
}

int devolverID_PCB(int socket){
	bool el_ID_es_igual(consola_pcb* unaConexion){
	    if(unaConexion->socket_consola == socket){
	        return 1;
	    }
	    return 0;
	}
	consola_pcb * conexionBuscada = list_find(conexiones_pcb, (void*) el_ID_es_igual);
	return conexionBuscada -> pcbVinculado;
}

int devolverID_CONSOLA(pcb* unPCB){
	bool el_ID_es_igual(consola_pcb* unaConexion){
	    if(unaConexion->pcbVinculado == unPCB -> id){
	        return 1;
	    }
	    return 0;
	}
	consola_pcb * conexionBuscada = list_find(conexiones_pcb, (void*) el_ID_es_igual);
	return conexionBuscada -> socket_consola;
}
