#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<commons/collections/list.h>
#include <semaphore.h>
#include <pthread.h>

void avisar_a_consola(pcb* pcbFinalizado);
void avisar_a_memoria_NuevoPCB(pcb nuevoPCB, int socket_memoria, t_log* logger);
void aviso_a_memoria_endPCB(pcb* pcbFinalizado, int socket_memoria);

void enviarStringSerializado(char* mensaje, int socket);
void enviarIntSerializado(int numero, int socket_memoria);

void avisar_a_consola(pcb* pcbFinalizado){
    int socket_consola = devolverID_CONSOLA(procesoFinalizado);
    char* mensajeFinalizacion = "El proceso ha terminado su ejecucion";

    enviarStringSerializado(mensajeFinalizacion, socket_consola);
}

void avisar_a_memoria_NuevoPCB(pcb nuevoPCB, int socket_memoria, t_log* logger){
    int tamanioBuffer = sizeof(int), sizeof(int);
    
    void* buffer = asignarMemoria(tamanioBuffer);

    int desplazamiento = 0;

    concatenarInt(buffer, &desplazamiento, pcb.tamanio);
    concatenarInt(buffer, &desplazamiento, pcb.id);

    enviarMensaje(socket_memoria, buffer, tamanioBuffer);

    free(buffer);
    
}

void aviso_a_memoria_endPCB(pcb* pcbFinalizado, int socket_memoria){
    int numero = pcbFinalizado -> id;

	enviarIntSerializado(numero, socket_memoria){
}

void enviarStringSerializado(char* mensaje, int socket){
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


