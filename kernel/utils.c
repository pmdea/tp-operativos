#include "kernel.h"

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

void avisar_a_cpu_interrupt(){
	char* mensaje = "Interrupcion";

	enviarStringSerializado(mensaje, socket_cpu_interrupt);

}

void avisar_a_consola(pcb* pcbFinalizado){
    int socket_consola = devolverID_CONSOLA(pcbFinalizado);
    char* mensajeFinalizacion = "El proceso ha terminado su ejecucion";

    enviarStringSerializado(mensajeFinalizacion, socket_consola);
}

void avisar_a_memoria(int socket_memoria, char* estado, pcb* unPCB, t_log* logger){
    int tamanioEstado = strlen(estado)+1;
	int tamanioBuffer = sizeof(int) + sizeof(int) + tamanioEstado;

    void* buffer = asignarMemoria(tamanioBuffer);

    int desplazamiento = 0;

    concatenarInt(buffer, &desplazamiento, 1); // Int para indicar que viene de kernel
    concatenarInt(buffer, &desplazamiento, unPCB->tamanio);
    concatenarInt(buffer, &desplazamiento, unPCB->id);
	concatenarString(buffer, &desplazamiento, estado);

    enviarMensaje(socket_memoria, buffer, tamanioBuffer);
	log_info(logger, "Enviando aviso a memoria ' %s ' ...", estado);
    free(buffer);

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

void serilizar_enviar_pcb(int socket, pcb* unPCB , t_log* logger){
	//Asigno tamanio al buffer
	int tamanioInstrucciones = tamanio_listaInst(unPCB -> instrucciones);
	int tamanioLista = unPCB -> instrucciones -> elements_count;
	int tamanioBuffer = sizeof(int)*5 + sizeof(double) + tamanioInstrucciones + sizeof(int)*tamanioLista;
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

	enviarMensaje(socket, buffer, tamanioBuffer);

	log_info(logger, "Enviando PCB....");

	free(buffer);
}

void enviar_respuesta_kernel(int socket, pcb* unPCB, int rafagaCPU , char* motivoRetorno, int tiempoBloqueo, t_log* logger){
	//Asigno tamanio al buffer
	int tamanioMotivoRetorno = strlen(motivoRetorno) + 1;
	int tamanioInstrucciones = tamanio_listaInst(unPCB -> instrucciones);
	int tamanioLista = unPCB -> instrucciones -> elements_count;
	int tamanioBuffer = sizeof(int)*5 + sizeof(double) + tamanioInstrucciones
			+ sizeof(int)*tamanioLista
			+ tamanioMotivoRetorno
			+ sizeof(int)*2;
	void* buffer = asignarMemoria(tamanioBuffer);

	//Lleno el buffer
	int desplazamiento = 0;

	//Orden de serializacion // PCB
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
	concatenarString(buffer, &desplazamiento, motivoRetorno);

	// Tiempo bloqueo
	concatenarInt(buffer, &desplazamiento, tiempoBloqueo);


	enviarMensaje(socket, buffer, tamanioBuffer);

	log_info(logger, "Enviando PCB a Kernel....");

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
	t_list* instrucciones = list_create();
	unPCB -> id = deserializarInt(socket_kernel);
	unPCB -> tamanio = deserializarInt(socket_kernel);
	unPCB -> program_counter = deserializarInt(socket_kernel);
	unPCB -> tabla_paginas = deserializarInt(socket_kernel);
	unPCB -> estimacion_rafaga = deserializarDouble(socket_kernel);

	unPCB -> instrucciones = list_create();
	instrucciones = deserializarListaInst(socket_kernel);
	list_add_all(unPCB -> instrucciones, instrucciones );
	log_info(loggerKernel, "He recibido un proceso");
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

t_list* recibir_devolucion_cpu(int socket){
	t_list* respuesta = list_create();
	pcb* procesoActualizado = deserializarPCB(socket);
	int rafagaCPU = deserializarInt(socket);
	char* motivoRetorno = deserializarString(socket);
	int tiempoBloqueo = deserializarInt(socket);
	list_add(respuesta, procesoActualizado);
	list_add(respuesta, rafagaCPU);
	list_add(respuesta, motivoRetorno);
	list_add(respuesta, tiempoBloqueo);
	return respuesta;
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
