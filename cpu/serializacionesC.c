#include "cpu.h"

void enviarMensaje(int socket, void* mensaje, int tamanio){
//	send(socket, (void*) &tamanio, sizeof(int), 0);
	send(socket, mensaje, tamanio, MSG_NOSIGNAL);
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
		//exit(1);
		return -1;
	} else {
		perror("error en el recv");
	}
	}
	return bytesRecibidos;
}

double deserializarDouble(int emisor){
	double mensaje;
	if(recibirMensaje(emisor, &mensaje, sizeof(double)) == -1){
		return -1;
	}
	return mensaje;
}


uint32_t deserializarInt32(int emisor){
	uint32_t mensaje;
	if(recibirMensaje(emisor, &mensaje, sizeof(uint32_t)) == -1){
		return -1;
	}
	return mensaje;
}

char* deserializarString(int emisor){
	uint32_t tamanioMensaje = deserializarInt32(emisor);
	char* mensaje = asignarMemoria(tamanioMensaje + 1);
	if(recibirMensaje(emisor, mensaje, tamanioMensaje) == -1){
		free(mensaje);
		return -1;
	}
	mensaje[tamanioMensaje - 1] = '\0';
	return mensaje;
}
