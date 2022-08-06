#include "kernel.h"

PCB crearPCB(int idPCB, t_proceso* proceso){
	PCB unPCB;
	unPCB.id = idPCB;
	unPCB.tamanio = proceso -> tamanio_proceso;
	unPCB.instrucciones = list_create();
	unPCB.program_counter = 0;
	unPCB.tabla_paginas = 0;
	unPCB.estimacion_rafaga = config_kernel.estimacion_inicial;
	list_add_all(unPCB . instrucciones, proceso -> instrucciones -> elements);

	log_debug(loggerKernel,"PCB creado");
	return unPCB;
}

void agregarEstadoNew(PCB* unPCB ){
	pthread_mutex_lock(&mutexNew);
	list_add(procesosNew, unPCB);
	pthread_mutex_unlock(&mutexNew);
	sem_post(&hayProcesoAnalizar);
}

void generarEstructuraPCB(int idPCB, t_proceso* proceso){
	PCB* unPCB = asignarMemoria(sizeof(PCB));
	*unPCB = crearPCB(idPCB, proceso);
	agregarEstadoNew(unPCB);
}

bool ordenarSRT(PCB* unPCB, PCB* otroPCB){
    return unPCB->estimacion_rafaga < otroPCB->estimacion_rafaga;
}


void estimador(PCB* unPCB, double alfa, int rafaga_ejecutada){
    unPCB -> estimacion_rafaga = (alfa * rafaga_ejecutada + (1 - alfa) * unPCB->estimacion_rafaga);
}
// VER CUANDO SE IMPLEMENTE LA UNION CON CONSOLA

int devolverID_PCB(int socket){
    bool el_ID_es_igual(consola_pcb* unaConexion){
        if(unaConexion->socket_consola == socket){
            return 1;
        }
        return 0;
    }
    consola_pcb * conexionBuscada = list_find(conexiones_pcb, (void*) el_ID_es_igual);
    return conexionBuscada->pcbVinculado;
}

int devolverID_CONSOLA(PCB* unPCB){
    bool el_ID_es_igual(consola_pcb* unaConexion){
        if(unaConexion->pcbVinculado == unPCB->id){
            return 1;
        }
        return 0;
    }
    consola_pcb * conexionBuscada = list_find(conexiones_pcb, (void*) el_ID_es_igual);
    return conexionBuscada -> socket_consola;
}

