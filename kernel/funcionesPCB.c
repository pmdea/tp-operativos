#include "kernel.h"

PCB crearPCB(int idPCB, t_proceso* proceso){
	PCB unPCB;
	unPCB.id = idPCB;
	unPCB.tamanio = proceso -> tamanio_proceso;
	unPCB.instrucciones = list_create();
	unPCB.program_counter = 45;
	unPCB.tabla_paginas = 86;
	unPCB.estimacion_rafaga = 10000;
	list_add_all(unPCB . instrucciones, proceso -> instrucciones -> elements);

	return unPCB;
}

void agregarEstadoNew(PCB* unPCB ){
	// MUTEX READY
	list_add(procesosNew, unPCB);
	// MUTEX READY
}

void generarEstructuraPCB(int idPCB, t_proceso* proceso){
	PCB* unPCB = asignarMemoria(sizeof(PCB));
	*unPCB = crearPCB(idPCB, proceso);
	agregarEstadoNew(unPCB);
}
