/*
 * kernel.c
 *
 *  Created on: Apr 23, 2022
 *      Author: pmdea
 */

#include "kernel.h"

#define PUERTO 8000
#define IP "127.0.0.1"

void inicializar_direccion_kernel(struct sockaddr_in direccion_kernel);
void bindear_kernel(int kernel, void* direccion_kernel, t_log* log_kernel);
void escuchar(int kernel, t_log* log_kernel);
void procesar_entradas_de_consolas(int kernel, t_log* log_kernel);
bool handshake(int consola);
void recibir_datos(int consola);
void recibir_paquete(int consola, t_paquete *paquete);

void terminar(int kernel, struct sockaddr_in direccion_kernel);

int main(void) {
	//PROCESOS
	t_queue *procesos_bloqueados = queue_create();
	t_queue *procesos_ready = queue_create();
	t_list *procesos_nuevos = list_create();


	//INICIAR KERNEL
	t_log* log_kernel = log_create("log_kernel.log", "KERNEL", 1,
			LOG_LEVEL_DEBUG);

	struct sockaddr_in direccion_kernel;
	inicializar_direccion_kernel(direccion_kernel);

	int kernel = socket(AF_INET, SOCK_STREAM, 0);
	int activado = 1;
	setsockopt(kernel, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));
	//free(activado);

	//FIN INICIAR KERNEL

	bindear_kernel(kernel, &direccion_kernel, log_kernel);

	escuchar(kernel,log_kernel);

	procesar_entradas_de_consolas(kernel, log_kernel);

	terminar(kernel, direccion_kernel);

	return 0;

	 //CONEXION CON CPU
	 //CONEXION CON MEMORIA
}
void inicializar_direccion_kernel(struct sockaddr_in direccion_kernel) {

	direccion_kernel.sin_family = AF_INET;
	direccion_kernel.sin_addr.s_addr = INADDR_ANY;
	direccion_kernel.sin_port = htons(PUERTO);

}

void bindear_kernel(int kernel, void* direccion_kernel, t_log* log_kernel) {
	if (bind(kernel, (void*) &direccion_kernel, sizeof(direccion_kernel))
			!= 0) {
		perror("fallo el bind");
		log_error(log_kernel, "Fallo el bind\n");
	} else {
		log_info(log_kernel, "El kernel fue bindeado con exito\n");
	}
}

void escuchar(int kernel, t_log* log_kernel) {
	listen(kernel, SOMAXCONN);
	log_info(log_kernel, "El kernel esta escuchando el puerto: %d\n", PUERTO);
	log_info(log_kernel, "Esperando conexiones...\n");
}

void procesar_entradas_de_consolas(int kernel, t_log* log_kernel) {

	while (1) {
		pthread_t hilo;
		struct sockaddr_in *direccion_consola;
		uint32_t *direccion_size;
		int *consola = malloc(sizeof(int));
		*consola = accept(kernel, (void*) *&direccion_consola, *&direccion_size);

		if (*consola == -1) {
			 perror("No se pudo aceptar a la consola");
			 log_error(log_kernel,"No se pudo aceptar a la consola\n");
			 } else if(handshake(*consola)) {
				 log_info(log_kernel,"Se conecto una consola");
				 pthread_create(&hilo, );
				 //&hilo, NULL, *recibir_datos(*consola), *consola
				 pthread_detach(hilo);
			 }

		free(*direccion_size);
		free(*consola);
		log_info(log_kernel, "Esperando nueva conexion...\n");
	}
}

bool handshake(int consola) {
	uint32_t *handshake = malloc(sizeof(uint32_t));
	int aprobado = 0;
	int rechazado = -1;

	recv(consola, &*handshake, sizeof(uint32_t), MSG_WAITALL);
	if(*handshake == 1) {
		send(consola,aprobado,sizeof(aprobado),0);
		return true;
	} else {
		send(consola,rechazado,sizeof(rechazado),0);
		return false;
	}
}

void recibir_datos(int consola) {
	t_paquete *paquete = crear_paquete();
	pcb *pbc_proceso;
	recibir_paquete(consola, &*paquete);
	armar_pcb(paquete);
	enviar_pcb(&pbc_proceso);
	enviar_mensaje_de_finalizacion(consola);
}

void recibir_paquete(int consola, t_paquete *paquete) {
	int tamanio = malloc(sizeof(int));
	free(tamanio);
}


