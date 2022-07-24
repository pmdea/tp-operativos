/*
 * kernel.c
 *
 *  Created on: 23 jul. 2022
 *      Author: utnso
 */

#include "kernel.h"

#define PUERTO "8000"
#define MENSAJE_HANDSHAKE_ESPERADO "HOLA DON PEPITO"

void inicializar_direccion_kernel(struct sockaddr_in *direccion_kernel);
void bindear_kernel(int kernel, struct sockaddr_in direccion_kernel);
void escuchar(int kernel);
void procesar_consola(int kernel);
void atender_consola(int consola);
bool se_pudo_hacer_el_handshake(int consola);
void recibir_mensaje(int consola, op_code* codigo, void* mensaje);
void enviar_confirmacion(int consola);
void recibir_proceso(int consola);
t_proceso* deserializar_proceso(void* contenido, int tam_max);
int cantidad_de_parametros(ID_INSTRUCCION identificador);
void mostrar_proceso(t_proceso* proceso);
void enviar_confirmacion(int consola);
void mostrar_instruccion(t_instruccion* instruccion);

t_log* loggerKernel;

int main() {

	struct sockaddr_in direccion_kernel;

	loggerKernel = log_create("./log_kernel", "kernel", 1, LOG_LEVEL_DEBUG);

	inicializar_direccion_kernel(&direccion_kernel);

	int kernel = socket(AF_INET, SOCK_STREAM, 0);
	int activado = 1;
	setsockopt(kernel, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	bindear_kernel(kernel, direccion_kernel);

	escuchar(kernel);

	while(1) {
		procesar_consola(kernel);
	}

	return 0;
}

void inicializar_direccion_kernel(struct sockaddr_in *direccion_kernel) {

	direccion_kernel->sin_family = AF_INET;
	direccion_kernel->sin_addr.s_addr = INADDR_ANY;
	direccion_kernel->sin_port = htons(atoi(PUERTO));

}

void bindear_kernel(int kernel, struct sockaddr_in direccion_kernel) {
	if (bind(kernel, (void*) &(direccion_kernel), sizeof(direccion_kernel))
			== -1) {
		log_error(loggerKernel, "Fallo el bind\n");
	} else {
		log_info(loggerKernel, "El kernel fue bindeado con exito\n");
	}
}

void escuchar(int kernel) {
	if(listen(kernel, SOMAXCONN) != 0) {
		log_error(loggerKernel, "No se pudo escuchar el puerto %s",PUERTO);
	} else {
		log_info(loggerKernel, "El kernel esta escuchando el puerto: %s\n", PUERTO);
		log_info(loggerKernel, "Esperando conexiones...\n");
	}

}

void procesar_consola(int kernel) {
		pthread_t hilo;
		int *consola = malloc(sizeof(int));
		*consola = accept(kernel, NULL,NULL);

		if (consola == -1) {
		       log_error(loggerKernel,"No se pudo aceptar a la consola\n");
		        } else {
		            log_info(loggerKernel, "SOCKET CONSOLA ACTUAL: %i", consola);
		            //log_info(loggerKernel, "ID PCB ACTUAL: %i", id_ultimo_pcb);
		        }

		 pthread_create(&hilo, NULL, (void*) atender_consola,(void*) consola);
		 pthread_detach(hilo);
}

void atender_consola(int consola) {
	log_info(loggerKernel,"Estoy atendiendo a la consola %i",consola);

	//handshake()
	if(se_pudo_hacer_el_handshake(consola)) {
		log_info(loggerKernel,"Se pudo hacer el handshake :D");
	} else {
		log_error(loggerKernel,"No se pudo completar el handshake :C");
	}

	//DENTRO DE RECIBIR_PROCESO SE TIENE QUE ARMAR EL PCB
	recibir_proceso(consola);

	enviar_confirmacion(consola);

}

void enviar_confirmacion(int consola) {

	op_code *cod = malloc(sizeof(op_code));
	*cod = CONFIRMACION;
	send(consola,cod,sizeof(op_code),0);
	free(cod);
}

void recibir_proceso(int consola) {

	op_code *cod = malloc(sizeof(op_code));
	recv(consola,cod,sizeof(op_code),0);

	int *tam_tot = malloc(sizeof(int));
	recv(consola,tam_tot,sizeof(int),0);

	void* contenido = malloc(*tam_tot);
	recv(consola,contenido,*tam_tot,0);

	t_proceso* proceso = deserializar_proceso(contenido, *tam_tot - sizeof(int));
	memcpy(&(proceso->tamanio_proceso),contenido + (*tam_tot - sizeof(int)),sizeof(int));

	mostrar_proceso(proceso);

	free(cod);
	free(tam_tot);
	free(contenido);
}

void mostrar_proceso(t_proceso* proceso) {

	log_debug(loggerKernel,"TAMAÑO: %i",proceso->tamanio_proceso);
	list_map(proceso->instrucciones->elements,(void*) mostrar_instruccion);
}

void mostrar_instruccion(t_instruccion* instruccion) {

	log_debug(loggerKernel,"ID: %i",(instruccion->identificador));
	for(int i = 0; i < cantidad_de_parametros(instruccion->identificador); i++) {
		log_debug(loggerKernel,"PARAMETRO: %i",(int)queue_peek(instruccion->parametros));
		queue_pop(instruccion->parametros);
	}
}

t_proceso* deserializar_proceso(void* contenido, int tam_max) {

	t_proceso* proceso = malloc(sizeof(t_proceso));
	proceso->instrucciones =  queue_create();
	int *desp = malloc(sizeof(int));
	*desp = 0;
	while(*desp < tam_max) {

		t_instruccion* instruccion = malloc(sizeof(t_instruccion));
		ID_INSTRUCCION *id = malloc(sizeof(ID_INSTRUCCION));
		t_queue* parametros = queue_create();
		memcpy(id,contenido + *desp, sizeof(ID_INSTRUCCION));
		*desp += sizeof(ID_INSTRUCCION);
		for(int i = 0; i < cantidad_de_parametros(*id); i++) {
			int *parametro = malloc(sizeof(int));
			memcpy(parametro,contenido + *desp, sizeof(int));
			*desp += sizeof(int);
			queue_push(parametros,(void*) *parametro);
		}
		instruccion->identificador = *id;
		instruccion->parametros = parametros;
		queue_push(proceso->instrucciones,instruccion);
	}

	return proceso;
}

bool se_pudo_hacer_el_handshake(int consola) {

	op_code *codigo = malloc(sizeof(op_code));
	void* mensaje = malloc(string_length(MENSAJE_HANDSHAKE_ESPERADO));

	recibir_mensaje(consola,codigo,mensaje);

	if (*codigo != HANDSHAKE ) {
		log_info(loggerKernel,"No se recibió el mensaje correcto");
		free(codigo);
		free(mensaje);
		return false;
	}

	if ( strcmp(MENSAJE_HANDSHAKE_ESPERADO,(char*) mensaje) == 0 ) {
		log_info(loggerKernel,"Se recibió el mensaje correcto");
		enviar_confirmacion(consola);
		free(codigo);
		free(mensaje);
		return true;
	}
	log_info(loggerKernel,"No se recibió el mensaje correcto");
	free(codigo);
	free(mensaje);
	return false;

}

void recibir_mensaje(int consola, op_code* codigo, void* mensaje) {

	int *tamanio = malloc(sizeof(int));

	recv(consola,tamanio,sizeof(int),0);

	void* recibido = malloc(*tamanio);
	recv(consola,recibido,*tamanio,0);

	memcpy(codigo,recibido,sizeof(int));
	memcpy(mensaje,recibido + sizeof(int), string_length(MENSAJE_HANDSHAKE_ESPERADO));

	free(recibido);
	free(tamanio);

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
