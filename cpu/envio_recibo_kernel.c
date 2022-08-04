#include "cpu.h"

void enviarRespuestaKernel(int socket_receptor, PCB unPCB, uint32_t motivoRegreso, uint32_t rafagaEjecutada, uint32_t tiempoBloqueo, t_log* logger){
	uint32_t cantidadInstrucciones = list_size(unPCB . instrucciones);
	int tamanioBuffer = sizeof(uint32_t)*4 + sizeof(double) + tamanioParametros(unPCB . instrucciones) + cantidadInstrucciones*sizeof(ID_INSTRUCCION) + sizeof(uint32_t)*3;

	void* buffer = asignarMemoria(tamanioBuffer);

	int desplazamiento = 0;

	concatenarInt32(buffer, &desplazamiento, unPCB . id);
	concatenarInt32(buffer, &desplazamiento, unPCB . tamanio);
	concatenarInt32(buffer, &desplazamiento, unPCB . program_counter);
	concatenarInt32(buffer, &desplazamiento, unPCB . tabla_paginas);
	concatenarDouble(buffer, &desplazamiento, unPCB . estimacion_rafaga);

	// Concatenar Instrucciones
	concatenarInt32(buffer, &desplazamiento, cantidadInstrucciones);

	for(int k = 0; k < cantidadInstrucciones; k++){
		t_instruccion* instruccion = list_get(unPCB . instrucciones, k);
		int parametros = cantidad_de_parametros(instruccion -> identificador);
		concatenarInt32(buffer, &desplazamiento, (uint32_t) instruccion -> identificador);
		switch(parametros){
			case 1:
				concatenarInt32(buffer, &desplazamiento, list_get(instruccion -> parametros -> elements,0));
				break;
			case 2:
				concatenarInt32(buffer, &desplazamiento, list_get(instruccion -> parametros -> elements,0));
				concatenarInt32(buffer, &desplazamiento, list_get(instruccion -> parametros -> elements,1));
				break;
			default:
				break;
		}
	}

	concatenarInt32(buffer, &desplazamiento, motivoRegreso);
	concatenarInt32(buffer, &desplazamiento, rafagaEjecutada);
	concatenarInt32(buffer, &desplazamiento, tiempoBloqueo);


	enviarMensaje(socket_receptor, buffer, tamanioBuffer);
	free(buffer);
	log_debug(logger, "********Enviando PCB ID %i a KERNEL********", unPCB.id);
}

PCB* deserializarPCB(int socket_emisor){
	PCB* unPCB = asignarMemoria(sizeof(PCB));
	unPCB -> id = deserializarInt32(socket_emisor);
	unPCB -> tamanio = deserializarInt32(socket_emisor);
	unPCB -> program_counter = deserializarInt32(socket_emisor);
	unPCB -> tabla_paginas = deserializarInt32(socket_emisor);
	unPCB -> estimacion_rafaga = deserializarDouble(socket_emisor);
	unPCB -> instrucciones = list_create();
	t_list* recibirInstrucciones = list_create();
	recibirInstrucciones = deserializarListaInstruccionesK(socket_emisor);
	list_add_all(unPCB -> instrucciones, recibirInstrucciones);
	free(recibirInstrucciones);
	return unPCB;
}

t_list* deserializarListaInstruccionesK(int emisor){
	uint32_t tamanioLista = deserializarInt32(emisor);
	t_list* lista = list_create();
	for(int k = 0; k < tamanioLista; k++){
		t_instruccion* instruccion = asignarMemoria(sizeof(instruccion));
		instruccion -> identificador = deserializarInt32(emisor);
		instruccion -> parametros = queue_create();
		int param = cantidad_de_parametros(instruccion -> identificador);
		switch(param){
			case 1:
				list_add(instruccion -> parametros -> elements, deserializarInt32(emisor));
				break;
			case 2:
				list_add(instruccion -> parametros -> elements, deserializarInt32(emisor));
				list_add(instruccion -> parametros -> elements, deserializarInt32(emisor));
				break;
			default:
				break;
		}
		list_add(lista, instruccion);
	}
	return lista;
}

uint32_t tamanioParametros(t_list* lista){
	uint32_t cantidadInstrucciones = list_size(lista);
	uint32_t tamanio = sizeof(uint32_t);
	for(int i = 0; i<cantidadInstrucciones; i++){
		t_instruccion* instruccion = list_get(lista, i);
		tamanio += (list_size(instruccion -> parametros -> elements) * sizeof(uint32_t));
	}
	return tamanio;
}

int instruccion_a_realizar(ID_INSTRUCCION identificador) {

	switch (identificador) {

		case NO_OP:
			return 0;
			break;
		case IO:
			return 1;
			break;
		case READ:
			return 2;
			break;
		case COPY:
			return 3;
			break;
		case WRITE:
			return 3;
		case EXIT:
			return 4;
			break;
		}

		return 0;
}

int cantidad_de_parametros(ID_INSTRUCCION identificador) {

	switch (identificador) {

		case IO:
			return 1;
			break;
		case NO_OP:
			return 0;
			break;
		case READ:
			return 1;
			break;
		case EXIT:
			return 0;
			break;
		case COPY:
			return 2;
			break;
		case WRITE:
			return 2;
		}

		return 0;
}
