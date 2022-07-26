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
			case 0:
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
	/*	void* tamanio = sizeof(int);
	recv(socket_emisor, tamanio, sizeof(int), 0);
	void* contenido = malloc((int)tamanio);
	recv(socket_emisor, contenido, tamanio, 0);
	int contador = 0;
	uint32_t *idPCB = malloc(sizeof(uint32_t));
	uint32_t *tamanioPCB = malloc(sizeof(uint32_t));
	uint32_t *program_counter = malloc(sizeof(uint32_t));
	uint32_t *tabla_paginas = malloc(sizeof(uint32_t));
	double *est = malloc(sizeof(uint32_t));
	uint32_t *cantLista = malloc(sizeof(uint32_t));
	t_queue *instrucciones = queue_create();
	memcpy(idPCB, contenido + contador, sizeof(uint32_t));
	contador += sizeof(uint32_t);
	memcpy(tamanioPCB, contenido + contador, sizeof(uint32_t));
	contador += sizeof(uint32_t);
	memcpy(program_counter, contenido + contador, sizeof(uint32_t));
	contador += sizeof(uint32_t);
	memcpy(tabla_paginas, contenido + contador, sizeof(uint32_t));
	contador += sizeof(uint32_t);
	memcpy(est, contenido + contador, sizeof(double));
	contador += sizeof(double);

	memcpy(cantLista, contenido + contador, sizeof(uint32_t));
	contador += sizeof(uint32_t);

	for(int i = 0; i < 3; i++){
		t_instruccion* instruc = malloc(sizeof(t_instruccion));
		uint32_t *identificador = malloc(sizeof(ID_INSTRUCCION));
		instruc -> parametros = queue_create();
		memcpy(&(instruc -> identificador), contenido + contador, sizeof(ID_INSTRUCCION));
		contador += sizeof(ID_INSTRUCCION);
		for(int j = 0; j < cantidad_de_parametros(*identificador); j++){
			uint32_t *param = malloc(sizeof(uint32_t));
			memcpy(param, contenido + contador, sizeof(uint32_t));
			contador += sizeof(uint32_t);
			queue_push(instruc -> parametros, *param);
		}
		queue_push(instrucciones, instruc);
	}
	unPCB -> id = *idPCB;
	unPCB -> tamanio = *tamanioPCB;
	unPCB -> program_counter = *program_counter;
	unPCB -> tabla_paginas = *tabla_paginas;
	unPCB -> estimacion_rafaga = *est;
	unPCB -> instrucciones = instrucciones -> elements;

	log_info(loggerCpu, "SIZE INST %i", list_size(unPCB -> instrucciones));
*/
	unPCB -> id = deserializarInt32(socket_emisor);
	log_info(loggerCpu, "ID PCB %i", unPCB -> id);
	unPCB -> tamanio = deserializarInt32(socket_emisor);
	unPCB -> program_counter = deserializarInt32(socket_emisor);
	unPCB -> tabla_paginas = deserializarInt32(socket_emisor);
	unPCB -> estimacion_rafaga = deserializarDouble(socket_emisor);
	unPCB -> instrucciones = list_create();
	unPCB -> instrucciones = deserializarListaInstruccionesK(socket_emisor);
	log_info(loggerCpu, "SIZE INST %i", list_size(unPCB -> instrucciones));
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
			case 99:
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


int cantidad_de_parametros(ID_INSTRUCCION identificador) {

	switch (identificador) {

		case IO:
			return 1;
			break;
		case NO_OP:
			return 1;
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


