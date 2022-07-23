#include "kernel.h"

void enviarPCB(int socket_receptor, PCB unPCB, t_log* logger){
	int tamanioInstruccionesTotal = tamanio_listaInst(unPCB.instrucciones);
	uint32_t cantidadInstrucciones = list_size(unPCB.instrucciones);
	int tamanioBuffer = sizeof(uint32_t)*4 + sizeof(double) + sizeof(uint32_t) + tamanioInstruccionesTotal;

	void* buffer = asignarMemoria(tamanioBuffer);

	int desplazamiento = 0;

	concatenarInt32(buffer, &desplazamiento, unPCB . id);
	concatenarInt32(buffer, &desplazamiento, unPCB . tamanio);
	concatenarInt32(buffer, &desplazamiento, unPCB . program_counter);
	concatenarInt32(buffer, &desplazamiento, unPCB . tabla_paginas);
	concatenarDouble(buffer, &desplazamiento, unPCB . estimacion_rafaga);
	concatenarInt32(buffer, &desplazamiento, cantidadInstrucciones);
	for(int i = 0; i < cantidadInstrucciones; i++){
		t_instruccion* unaInstruccion = list_get(unPCB . instrucciones, i);
		concatenarString(buffer, &desplazamiento, unaInstruccion -> identificador);
		concatenarListaInt32(buffer, &desplazamiento, unaInstruccion -> parametros -> elements);
	}

	enviarMensaje(socket_receptor, buffer, tamanioBuffer);
	free(buffer);
//	log_info(logger, "********Enviando PCB ID %i a CPU********", unPCB.id);
	log_debug(logger, "********Enviando PCB ID %i a CPU********", unPCB.id);
//	log_error(logger, "********Enviando PCB ID %i a CPU********", unPCB.id);
//	log_trace(logger, "********Enviando PCB ID %i a CPU********", unPCB.id);
//	log_warning(logger, "********Enviando PCB ID %i a CPU********", unPCB.id);
}

t_list* recibirRespuestaCPU(int socket_emisor){
	t_list* respuesta = list_create();
	PCB* unPCB = deserializarPCB(socket_emisor, loggerKernel);
	uint32_t motivoRegreso = deserializarInt32(socket_emisor);
	uint32_t rafagaEjecutada = deserializarInt32(socket_emisor);
	uint32_t tiempoBloqueo = deserializarInt32(socket_emisor);

	list_add(respuesta, unPCB);
	list_add(respuesta, motivoRegreso);
	list_add(respuesta, rafagaEjecutada);
	list_add(respuesta, tiempoBloqueo);
	log_debug(loggerKernel, "********Recibi datos de CPU - PCB ID %i********", unPCB->id);
	return respuesta;

}

PCB* deserializarPCB(int socket_emisor, t_log* logger){
	PCB* unPCB = asignarMemoria(sizeof(PCB));
	unPCB -> id = deserializarInt32(socket_emisor);
	unPCB -> tamanio = deserializarInt32(socket_emisor);
	unPCB -> program_counter = deserializarInt32(socket_emisor);
	unPCB -> tabla_paginas = deserializarInt32(socket_emisor);
	unPCB -> estimacion_rafaga = deserializarDouble(socket_emisor);
	unPCB -> instrucciones = list_create();
	uint32_t cantidadInstrucciones = deserializarInt32(socket_emisor);
	for (int i = 0; i < cantidadInstrucciones; i++){
		t_instruccion* unaInstruccion = asignarMemoria(sizeof(t_instruccion));
		unaInstruccion -> identificador = deserializarString(socket_emisor);
		unaInstruccion -> parametros = queue_create();
		unaInstruccion -> parametros -> elements = deserializarListaInt32(socket_emisor);
		list_add(unPCB -> instrucciones, unaInstruccion);
	}

	//unPCB -> instrucciones = list_create();
	//t_list* recibirInstrucciones = list_create();

	//recibirInstrucciones = deserializarListaInstrucciones(socket_emisor);
	//list_add_all(unPCB -> instrucciones, recibirInstrucciones);
	return unPCB;
}

void mostrarDatosPCB(PCB unPCB, t_log* log){
	log_info(log, "ID PCB: %i", unPCB . id);
	log_info(log, "TAMANIO PCB: %i", unPCB . tamanio);
	log_info(log, "PC PCB: %i", unPCB . program_counter);
	log_info(log, "TP PCB: %i", unPCB . tabla_paginas);
	log_info(log, "ESTIMACION PCB: %f", unPCB . estimacion_rafaga);
	log_info(log, "CANTIDAD INST: %i", list_size(unPCB . instrucciones));

}

/*
void aviso_a_memoria(int socket, uint32_t estado, pcb* unProceso, t_log* logger){
	int tamanioBuffer = sizeof(uint32_t)*4;
	void* buffer = asignarMemoria(tamanioBuffer);

	int desplazamiento = 0;

	concatenarInt32(buffer, &desplazamiento, (uint32_t) 1);
	concatenarInt32(buffer, &desplazamiento, estado);
	concatenarInt32(buffer, &desplazamiento, (uint32_t) unProceso -> tamanio);
	concatenarInt32(buffer, &desplazamiento, (uint32_t) unProceso -> id);

	enviarMensaje(socket, buffer, tamanioBuffer);
	if(estado == INICIALIZA){
		log_info(logger, "Enviando aviso de Inicializacion a Memoria...");
	}
	if(estado == SUSPENDE){
		log_info(logger, "Enviando aviso de Suspension a Memoria...");
	}
	if(estado == FINALIZA){
		log_info(logger, "Enviando aviso de Finalizacion a Memoria...");
	}

}

void avisar_a_cpu_interrupt(){

	enviarIntSerializado(1, socket_cpu_interrupt);

	log_info(loggerKernel, "Enviando aviso de interrupcion a CPU...");

}

*/
