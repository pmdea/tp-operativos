#include "kernel.h"

void enviarMensaje(int socket, void* mensaje, int tamanio){
	send(socket, mensaje, tamanio, 0);
}

uint32_t tamanio_listaInst(t_list* listaInst){
	uint32_t respuesta = sizeof(uint32_t);
	for(int i = 0; i < listaInst -> elements_count; i++ ){
		t_instruccion* inst = list_get(listaInst, i);
		int cantidadParametros = inst -> parametros -> elements -> elements_count;
		respuesta += sizeof(uint32_t) +  strlen(inst -> identificador) + 1 + sizeof(uint32_t)*cantidadParametros;
	}
	return respuesta;
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
	concatenarInt32(buffer, desplazamiento, strlen(mensaje) + 1);
	memcpy(buffer + *desplazamiento, mensaje, strlen(mensaje) + 1);
	*desplazamiento = *desplazamiento + strlen(mensaje) + 1;
}

void concatenarListaInt32(void* buffer, int* desplazamiento, t_list* listaArchivos){
	concatenarInt32(buffer, desplazamiento, listaArchivos->elements_count);
	for(int i = 0; i < (listaArchivos->elements_count); i++){
		concatenarInt32(buffer, desplazamiento, list_get(listaArchivos, i));
	}
}

void* asignarMemoria(int cantidad){
	void* buffer = malloc(cantidad);
	if(buffer == NULL){
		printf("No hay espacio\n");
		exit(-99);
	}
	return buffer;
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

double deserializarDouble(int emisor){
	double mensaje;
	recibirMensaje(emisor, &mensaje, sizeof(double));
	return mensaje;
}

char* deserializarString(int emisor){
	uint32_t tamanioMensaje = deserializarInt32(emisor);
	char* mensaje = asignarMemoria(tamanioMensaje + 1);
	recibirMensaje(emisor, mensaje, tamanioMensaje);
	mensaje[tamanioMensaje - 1] = '\0';
	return mensaje;
}

uint32_t deserializarInt32(int emisor){
	uint32_t mensaje;
	recibirMensaje(emisor, &mensaje, sizeof(uint32_t));
	return mensaje;
}

t_list* deserializarListaInt32(int emisor){
	uint32_t elementosDeLalista = deserializarInt32(emisor);
	t_list* respuesta = list_create();
	for(int i = 0; i < elementosDeLalista; i++){
		list_add(respuesta, deserializarInt32(emisor));
	}
	return respuesta;
}
