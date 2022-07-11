#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<commons/collections/list.h>
#include <semaphore.h>
#include <pthread.h>


bool ordenarSTR(pcb* unPCB,pcb* otroPCB){
	double est1 = unPCB->estimacion_rafaga;
	double est2 = otroPCB->estimacion_rafaga;
	return est2 > est1;
}

void estimador(pcb* unPCB, double alfa, int rafaga_ejecutada){
	unPCB -> estimacion_rafaga = (alfa * rafaga_ejecutada + (1 - alfa) * unPCB->estimacion_rafaga);
}

int devolverID_PCB (int socket){
	bool el_ID_es_igual(consola_pcb* unaConexion){
	    if(unaConexion->socket_consola == socket){
	        return 1;
	    }
	    return 0;
	}
	consola_pcb * conexionBuscada = list_find(conexiones_pcb, (void*) el_ID_es_igual);
	return conexionBuscada -> pcbVinculado;
}

int devolverID_CONSOLA (pcb* unPCB){
	bool el_ID_es_igual(consola_pcb* unaConexion){
	    if(unaConexion->pcbVinculado == unPCB -> id){
	        return 1;
	    }
	    return 0;
	}
	consola_pcb * conexionBuscada = list_find(conexiones_pcb, (void*) el_ID_es_igual);
	return conexionBuscada -> socket_consola;
}


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


