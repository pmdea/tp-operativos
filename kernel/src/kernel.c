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
void atender_consola(int consola);
bool protocolo_handshake(int consola);
bool es_igual_a(char* un_string, char* otro_string);
bool enviar_mensaje(op_code codigo, char* mensaje, int socket_consola);
bool se_pudo_recibir_el_proceso(t_proceso* proceso,int consola);
void recibir_instrucciones(t_proceso* proceso,int consola);
bool se_pueden_recibir_instrucciones(t_proceso* proceso, int consola);
void recibir_parametros(t_instruccion* instruccion, int consola);
int cantidad_de_parametros(char* identificador);
void enviar_confirmacion(int consola);
bool enviar_paquete(t_paquete* paquete, int socket_consola);
void eliminar_paquete(t_paquete* paquete);

#define CONFIRM_ENVIADA "RECIBIDO :)"
#define MENSAJE_HANDSHAKE_ESPERADO "HOLA DON PEPITO"
#define MENSAJE_HANDSHAKE_ENVIADO "HOLA DON JOSE"

t_log* log_kernel;

int main(void) {
	// INICIAR KERNEL
	log_kernel = log_create("log_kernel.log", "KERNEL", 1, LOG_LEVEL_DEBUG);
	struct sockaddr_in direccion_kernel;
	inicializar_direccion_kernel(direccion_kernel);

	int kernel = socket(AF_INET, SOCK_STREAM, 0);
	int activado = 1;
	setsockopt(kernel, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	//LEER ARCHIVO CONFIG
	t_config* config = config_create("./kernel/kernel.config");

	config_destroy(config);
	//ESPERAR CONEXION CONSOLA

	bindear_kernel(kernel, &direccion_kernel);

	escuchar(kernel);

	//CREAR HILOS

	procesar_entradas_de_consolas(kernel);

	//FINALIZAR KERNEL

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
		int *consola = malloc(sizeof(int));
		*consola = accept(kernel, NULL,NULL);

		if (*consola == -1) {
			log_error(log_kernel,"No se pudo aceptar a la consola\n");
		} else {
			pthread_create(&hilo, NULL, (void*) atender_consola,(void*) *consola);
			pthread_detach(hilo);
			free(consola);
		}

		log_info(log_kernel, "Esperando nueva conexion...\n");
	}

}

bool protocolo_handshake(int consola) {
	op_code *operacion = malloc(sizeof(op_code));
	recv(consola,operacion,sizeof(op_code),0);
	if(*operacion != HANDSHAKE) {
		log_error(log_kernel,"La consola no implementa el mismo protocolo handshake del kernel");
		free(operacion);
		return false;
	}
	free(operacion);
	t_mensaje* recibido = malloc(sizeof(t_mensaje));
	recv(consola,&(recibido->tamanio_mensaje), sizeof(int),0);
	recv(consola,recibido->mensaje,recibido->tamanio_mensaje,0);
	strcat(recibido->mensaje,"\0");
	if(!es_igual_a(recibido->mensaje,MENSAJE_HANDSHAKE_ESPERADO)) {
		log_error(log_kernel,"EL mensaje recibido no era el esperado: recibido: %s, esperado: %s",recibido->mensaje,MENSAJE_HANDSHAKE_ESPERADO);
	} else if (!enviar_mensaje(HANDSHAKE,MENSAJE_HANDSHAKE_ENVIADO,consola)) {
		log_error(log_kernel,"No se pudo enviar el mensaje de handshake");
	} else {
		log_info(log_kernel,"El protocolo handshake fue aprobado");
		free(recibido);
		return true;
	}
	free(recibido);
	return false;
}

bool es_igual_a(char* un_string, char* otro_string) {
	return strcmp(un_string, otro_string) == 0;
}


void atender_consola(int consola) {
	log_info(log_kernel, "preparado para recibir datos de la consola...\n");
	log_info(log_kernel,"Iniciando protocolo handshake...");
	t_proceso* proceso = malloc(sizeof(t_proceso));
	proceso->instrucciones = queue_create();
	if(!protocolo_handshake(consola)) {
		log_error(log_kernel,"La consola no implementa el mismo protocolo habdshake que el kernel");
	} else if(!se_pudo_recibir_el_proceso(proceso, consola)) {
		log_error(log_kernel,"No se pudo recibir el proceso");
	} else {
		enviar_confirmacion(consola);
	}

}

bool se_pudo_recibir_el_proceso(t_proceso* proceso,int consola) {
	int *tamanio_recibido = malloc(sizeof(int));
	if(recv(consola,tamanio_recibido,sizeof(int),0) == 0) {
		log_error(log_kernel,"La consola se desconexto.");
		free(tamanio_recibido);
		return false;
	} else {
		recibir_instrucciones(proceso,consola);
		recv(consola,&(proceso->tamanio_proceso),sizeof(int),0);
		free(tamanio_recibido);
		return true;
	}
}

void recibir_instrucciones(t_proceso* proceso, int consola) {
	while(se_pueden_recibir_instrucciones(proceso,consola));
}

bool se_pueden_recibir_instrucciones(t_proceso* proceso, int consola) {
	t_instruccion* instruccion = malloc(sizeof(t_instruccion));
	instruccion->parametros = queue_create();
	recv(consola,&(instruccion->tamanio_id),sizeof(int),0);
	recv(consola,&(instruccion->identificador),instruccion->tamanio_id,0);
	recibir_parametros(instruccion, consola);
	queue_push(proceso->instrucciones,instruccion);
	return es_igual_a(instruccion->identificador,"EXIT");
}

void recibir_parametros(t_instruccion* instruccion, int consola) {
	int contador = 0;
	while(contador < cantidad_de_parametros(instruccion->identificador)){
		int *parametro = malloc(sizeof(int));
		recv(consola,parametro,sizeof(int),0);
		queue_push(instruccion->parametros,(void*) *parametro);
		free(parametro);
	}
}

int cantidad_de_parametros(char* identificador) {

	if (es_igual_a(identificador, "I/O") || es_igual_a(identificador, "READ")
			|| es_igual_a(identificador, "NO_OP")) {
		return 1;
	} else if (es_igual_a(identificador, "COPY")
			|| es_igual_a(identificador, "WRITE")) {
		return 2;
	} else if (es_igual_a(identificador, "EXIT")) {
		return 0;
	} else {
		log_error(log_kernel,
				"Se desconoce la cantidad de parametros que espera el identificador %s",
				identificador);
		return -1;
	}
}

void enviar_confirmacion(int consola) {
	if(!enviar_mensaje(CONFIRMACION,CONFIRM_ENVIADA ,consola)) {
		log_error(log_kernel,"No se pudo enviar la confirmacion");
	}
}

bool enviar_mensaje(op_code codigo, char* mensaje, int socket_consola) {
	t_paquete* envio = malloc(sizeof(t_paquete));
	envio->codigo_operacion = codigo;
	envio->buffer->size = string_length(mensaje) + sizeof(int);
	envio->buffer->stream = malloc(envio->buffer->size);
	int desplazamiento = 0;
	int *tamanio_mensaje = malloc(sizeof(int));
	*tamanio_mensaje = string_length(mensaje);
	memcpy(envio->buffer->stream + desplazamiento,tamanio_mensaje,sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(envio->buffer->stream + desplazamiento,mensaje,sizeof(int));
	return enviar_paquete(envio,socket_consola);
}

bool enviar_paquete(t_paquete* paquete, int socket_consola) {
	int *tamanio_datos = malloc(sizeof(int));
	*tamanio_datos = sizeof(op_code) + paquete->buffer->size + sizeof(int);
	void* datos_a_enviar = malloc(*tamanio_datos);
	int desplazamiento = 0;
	memcpy(datos_a_enviar + desplazamiento,&(paquete->codigo_operacion),sizeof(op_code));
	desplazamiento += sizeof(op_code);
	memcpy(datos_a_enviar + desplazamiento,&(paquete->buffer->size),sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(datos_a_enviar + desplazamiento,&(paquete->buffer->stream),paquete->buffer->size);

	if(send(socket_consola,datos_a_enviar,*tamanio_datos,0) != -1) {
		eliminar_paquete(paquete);
		free(datos_a_enviar);
		free(tamanio_datos);
		log_error(log_kernel,"No se pudo enviar el paquete");
		return false;
	}
	eliminar_paquete(paquete);
	free(datos_a_enviar);
	free(tamanio_datos);
	log_info(log_kernel,"El paquete fue enviado con exito");
	return true;
}

void eliminar_paquete(t_paquete* paquete) {
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}
