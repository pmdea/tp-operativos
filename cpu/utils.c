#include "cpu.h"

void* asignarMemoria(int cantidad){
	void* buffer = malloc(cantidad);
	if(buffer == NULL){
		printf("No hay espacio\n");
		exit(-99);
	}
	return buffer;
}

void enviarMensaje(int socket, void* mensaje, int tamanio){
	send(socket, mensaje, tamanio, 0);
}

int recibirMensaje(int socketEmisor, void* buffer, int bytesMaximos){
	int bytesRecibidos = recv(socketEmisor, buffer, bytesMaximos, MSG_WAITALL);
	if(bytesRecibidos<=0){
	// error o conexión cerrada por el cliente
	if (bytesRecibidos == 0) {
		// conexión cerrada
		printf("conexion cerrada\n");
		exit(1);
	} else {
		perror("error en el recv");
	}
	}
	return bytesRecibidos;
}


void enviarStringSerializado(char* texto, int socket){
	int tamanioTexto = strlen(texto) + 1;
	int tamanioMensaje = sizeof(int) + tamanioTexto;

	void* mensaje = asignarMemoria(tamanioMensaje);

	int desplazamiento = 0;
	concatenarString(mensaje, &desplazamiento, texto);
	enviarMensaje(socket, mensaje, tamanioMensaje);
	free(mensaje);
}

void enviarIntSerializado(int numero, int socket_memoria){
	int tamanioMensaje = sizeof(int);
	void* mensaje = asignarMemoria(tamanioMensaje);

	int desplazamiento = 0;

	concatenarInt(mensaje, &desplazamiento, numero);

	enviarMensaje(socket_memoria, mensaje, tamanioMensaje);

	free(mensaje);
}
void enviar_respuesta_kernel(int socket, pcb* unPCB, int rafagaCPU , int motivoRetorno, int tiempoBloqueo, t_log* logger){
	//Asigno tamanio al buffer
	int tamanioInstrucciones = tamanio_listaInst(unPCB -> instrucciones);
	int tamanioLista = list_size(unPCB -> instrucciones);
	int tamanioBuffer = sizeof(int)*5 + sizeof(double) + tamanioInstrucciones + sizeof(int)*tamanioLista + sizeof(int)*3;
	void* buffer = asignarMemoria(tamanioBuffer);
	//Lleno el buffer
	int desplazamiento = 0;

	concatenarInt(buffer, &desplazamiento, unPCB -> id);
	concatenarInt(buffer, &desplazamiento, unPCB -> tamanio);
	concatenarInt(buffer, &desplazamiento, unPCB -> program_counter);
	concatenarInt(buffer, &desplazamiento, unPCB -> tabla_paginas);
	concatenarDouble(buffer, &desplazamiento, unPCB -> estimacion_rafaga);
	concatenarInt(buffer, &desplazamiento, tamanioLista);

	for(int i = 0; i < tamanioLista; i++){
		t_instruccion* unaInstruccion = list_get(unPCB -> instrucciones, i);
		concatenarInstruccion(buffer, &desplazamiento, unaInstruccion);
	}

	// Rafaga CPU
	concatenarInt(buffer, &desplazamiento, rafagaCPU);

	// Motivo retorno
	concatenarInt(buffer, &desplazamiento, motivoRetorno);

	// Tiempo bloqueo
	concatenarInt(buffer, &desplazamiento, tiempoBloqueo);


	enviarMensaje(socket, buffer, tamanioBuffer);

	log_info(loggerCpu, "***Envie el PCB DE ID : %i", unPCB -> id);
	log_info(loggerCpu, "***RAFAGA EJECUTADA : %f", rafagaCPU);
	free(buffer);
}

void concatenarInstruccion(void* buffer, int* desplazamiento, t_instruccion* unaInstruccion){
	concatenarString(buffer, desplazamiento, unaInstruccion -> identificador);
	concatenarListaInt(buffer, desplazamiento, unaInstruccion -> parametros -> elements);
}

void concatenarInt(void* buffer, int* desplazamiento, int numero){
	memcpy(buffer + *desplazamiento, &numero, sizeof(int));
	*desplazamiento = *desplazamiento + sizeof(int);
}

void concatenarInt32(void* buffer, int* desplazamiento, uint32_t numero){
	memcpy(buffer + *desplazamiento, &numero, sizeof(uint32_t));
	*desplazamiento = *desplazamiento + sizeof(uint32_t);
}

void concatenarDouble(void* buffer, int* desplazamiento, double numero){
	memcpy(buffer + *desplazamiento, &numero, sizeof(double));
	*desplazamiento = *desplazamiento + sizeof(double);
}

void concatenarString(void* buffer, int* desplazamiento, char* mensaje){
	concatenarInt(buffer, desplazamiento, strlen(mensaje) + 1);
	memcpy(buffer + *desplazamiento, mensaje, strlen(mensaje) + 1);
	*desplazamiento = *desplazamiento + strlen(mensaje) + 1;
}

void concatenarListaInt(void* buffer, int* desplazamiento, t_list* listaArchivos){
	concatenarInt(buffer, desplazamiento, listaArchivos->elements_count);
	for(int i = 0; i < (listaArchivos->elements_count); i++){
		concatenarInt(buffer, desplazamiento, list_get(listaArchivos, i));
	}
}

pcb* deserializarPCB(int socket_kernel){
	pcb* unPCB = asignarMemoria(sizeof(pcb));
	log_info(loggerCpu, "ME LLEGO PROCESO PARA DESEREALIZAR");
	t_list* instrucciones = list_create();
	unPCB -> id = deserializarInt(socket_kernel);
	log_info(loggerCpu, "ME LLEGO PROCESO");
	unPCB -> tamanio = deserializarInt(socket_kernel);
	unPCB -> program_counter = deserializarInt(socket_kernel);
	log_info(loggerCpu, "ESPERO PROGRAM COUNTER = 2 Y OBTENGO = %i", unPCB -> program_counter);
	unPCB -> tabla_paginas = deserializarInt(socket_kernel);
	log_info(loggerCpu, "ESPERO tabla paginas = 0 Y OBTENGO = %i", unPCB -> tabla_paginas);
	unPCB -> estimacion_rafaga = deserializarDouble(socket_kernel);
	log_info(loggerCpu, "ESPERO rafagas = 10000 Y OBTENGO = %f", unPCB -> estimacion_rafaga);
	unPCB -> instrucciones = list_create();
	log_info(loggerCpu, "******ANTES DE RIPEAR*******");
	instrucciones = deserializarListaInst(socket_kernel);
	/* HASTA ACA */
	log_info(loggerCpu, "No rompi");
	list_add_all(unPCB -> instrucciones, instrucciones );
	return unPCB;
}

int deserializarInt(int emisor){
	int mensaje;
	recibirMensaje(emisor, &mensaje, sizeof(int));
	return mensaje;
}

double deserializarDouble(int emisor){
	double mensaje;
	recibirMensaje(emisor, &mensaje, sizeof(double));
	return mensaje;
}

char* deserializarString(int emisor){
	int tamanioMensaje = deserializarInt(emisor);
	char* mensaje = asignarMemoria(tamanioMensaje + 1);
	recibirMensaje(emisor, mensaje, tamanioMensaje);
	mensaje[tamanioMensaje - 1] = '\0';
	return mensaje;
}

t_list* deserializarListaInt(int emisor){
	int elementosDeLalista = deserializarInt(emisor);
	t_list* respuesta = list_create();
	for(int i = 0; i < elementosDeLalista; i++){
		list_add(respuesta, deserializarInt(emisor));
	}
	return respuesta;
}

t_list* deserializarListaInst(int emisor){
	int elementosDeLalista = deserializarInt(emisor);
	t_list* respuesta = list_create();
	for(int i = 0; i < elementosDeLalista; i++){
		//t_instruccion* inst = deserializarInst(emisor);
		list_add(respuesta, deserializarInst(emisor));
	}
	return respuesta;

}

t_instruccion* deserializarInst(int emisor){
	t_instruccion* unaInstruccion = asignarMemoria(sizeof(t_instruccion));
	unaInstruccion -> identificador = deserializarString(emisor);
	unaInstruccion -> parametros = queue_create();
	unaInstruccion -> parametros -> elements = deserializarListaInt(emisor);
	return unaInstruccion;
}

uint32_t deserializarInt32(int emisor){
	uint32_t mensaje;
	recibirMensaje(emisor, &mensaje, sizeof(uint32_t));
	return mensaje;
}

int tamanio_listaInst(t_list* listaInst){
	int respuesta = sizeof(int);
	for(int i = 0; i < listaInst -> elements_count; i++ ){
		t_instruccion* inst = list_get(listaInst, i);
		int cantidadParametros = inst -> parametros -> elements -> elements_count;
		respuesta += sizeof(int) +  strlen(inst -> identificador) + 1 + sizeof(int)*cantidadParametros;
	}
	return respuesta;
}
