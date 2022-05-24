/*
 * kernel.c
 *
 *  Created on: Apr 23, 2022
 *      Author: Daniel Aizcorbe
 */
#include "kernel.h"
#include <commons/log.h>

#define PUERTO 8000
#define IP "127.0.0.1"

void inicializar_direccion_kernel(struct sockaddr_in direccion_kernel);
void bindear_kernel(int kernel, void* direccion_kernel);
void escuchar(int kernel);
void procesar_entradas_de_consolas(int kernel);
bool handshake(int consola);
void atender_consola(int consola);

//void terminar(int kernel, struct sockaddr_in direccion_kernel);
t_log* log_kernel;

int main(void) {
	// INICIAR KERNEL
	log_kernel = log_create("log_kernel.log", "KERNEL", 1, LOG_LEVEL_DEBUG);
	struct sockaddr_in direccion_kernel;
	inicializar_direccion_kernel(direccion_kernel);

	int kernel = socket(AF_INET, SOCK_STREAM, 0);
	int activado = 1;
	setsockopt(kernel, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	//ESPERAR CONEXION CONSOLA

	bindear_kernel(kernel, &direccion_kernel);

	escuchar(kernel);

	//CREAR HILOS

	procesar_entradas_de_consolas(kernel);

	//FINALIZAR KERNEL

	//terminar(kernel, direccion_kernel);

	return 0;

}
void inicializar_direccion_kernel(struct sockaddr_in direccion_kernel) {

	direccion_kernel.sin_family = AF_INET;
	direccion_kernel.sin_addr.s_addr = INADDR_ANY;
	direccion_kernel.sin_port = htons(PUERTO);

}

void bindear_kernel(int kernel, void* direccion_kernel) {
	if (bind(kernel, (void*) &direccion_kernel, sizeof(direccion_kernel))
			!= 0) {
		perror("fallo el bind");
		log_error(log_kernel, "Fallo el bind\n");
	} else {
		log_info(log_kernel, "El kernel fue bindeado con exito\n");
	}
}

void escuchar(int kernel) {
	listen(kernel, SOMAXCONN);
	log_info(log_kernel, "El kernel esta escuchando el puerto: %d\n", PUERTO);
	log_info(log_kernel, "Esperando conexiones...\n");
}

void procesar_entradas_de_consolas(int kernel) {

	while (1) {
		pthread_t hilo;
		struct sockaddr_in *direccion_consola;
		uint32_t *direccion_size;
		int *consola = malloc(sizeof(int));
		consola = accept(kernel, (void*) *&direccion_consola, *&direccion_size);

		if (*consola == -1) {
			log_error(log_kernel,"No se pudo aceptar a la consola\n");
			perror("No se pudo aceptar a la consola");

		} else if(handshake(*consola)) {
			log_info(log_kernel,"el handshake con la consola fue exitoso\n");
			pthread_create(&hilo, NULL, (void*) atender_consola, consola);
			pthread_detach(hilo);
		}
		free(consola);
		free(direccion_size);

		log_info(log_kernel, "Esperando nueva conexion...\n");
	}

}

bool handshake(int consola) {
	uint32_t* handshake = malloc(sizeof(uint32_t));
	int aprobado = 0;
	int rechazado = -1;

	recv(consola, &handshake, sizeof(uint32_t), MSG_WAITALL);
	if(*handshake == 1) {
		send(consola, aprobado,sizeof(aprobado),0);
		free(handshake);
		return true;
	} else {
		send(consola, rechazado,sizeof(rechazado),0);
		free(handshake);
		return false;
	}
}

void atender_consola(int consola) {
	log_info(log_kernel, "preparado para recibir datos de la consola...\n");
}
