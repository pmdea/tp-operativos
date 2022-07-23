#include "cpu.h"

void enviarRespuestaKernel(int socket_receptor, PCB unPCB, uint32_t motivoRegreso, uint32_t rafagaEjecutada, uint32_t tiempoBloqueo, t_log* logger){
	int tamanioInstruccionesTotal = tamanio_listaInst(unPCB.instrucciones);
	uint32_t cantidadInstrucciones = list_size(unPCB.instrucciones);
	int tamanioBuffer = sizeof(uint32_t)*4 + sizeof(double) + sizeof(uint32_t) + tamanioInstruccionesTotal + sizeof(uint32_t)*3;

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

	concatenarInt32(buffer, &desplazamiento, (uint32_t) motivoRegreso);
	concatenarInt32(buffer, &desplazamiento, rafagaEjecutada);
	concatenarInt32(buffer, &desplazamiento, tiempoBloqueo);


	enviarMensaje(socket_receptor, buffer, tamanioBuffer);
	free(buffer);
//	log_info(logger, "********Enviando PCB ID %i a CPU********", unPCB.id);
	log_debug(logger, "********Enviando respuesta a Kernel - PCB ID %i********", unPCB.id);
//	log_error(logger, "********Enviando PCB ID %i a CPU********", unPCB.id);
//	log_trace(logger, "********Enviando PCB ID %i a CPU********", unPCB.id);
//	log_warning(logger, "********Enviando PCB ID %i a CPU********", unPCB.id);
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


