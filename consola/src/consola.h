/*
 * consola.h
 *
 *  Created on: 22 jul. 2022
 *      Author: utnso
 */

#ifndef SRC_CONSOLA_H_
#define SRC_CONSOLA_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include <commons/string.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

typedef enum {
	IO = 0,
	READ = 1,
	WRITE = 2,
	COPY = 3,
	EXIT = 4,
	NO_OP = 5
} ID_INSTRUCCION;

typedef struct {
	ID_INSTRUCCION identificador;
	t_queue* parametros;
} t_instruccion;

typedef enum {
	HANDSHAKE,
	ENVIO_PROCESO,
	RECHAZO,
	CONFIRMACION,
	FINALIZACION_PROCESO
} op_code;

typedef struct {
	int size;
	void* stream;
} __attribute__((packed))
t_buffer;

typedef struct {
	op_code operacion;
	t_buffer* buffer;
}  __attribute__((packed))
t_paquete;

typedef struct {
	op_code operacion;
	int tamanio_mensaje;
	char* mensaje;
} __attribute__((packed))
t_mensaje;

typedef struct {
	t_queue* instrucciones;
	int tamanio_proceso;
}t_proceso;



#endif /* SRC_CONSOLA_H_ */
