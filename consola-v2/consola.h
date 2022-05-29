/*
 * consola.h
 *
 *  Created on: 27 may. 2022
 *      Author: utnso
 */

#ifndef CONSOLA_V2_CONSOLA_H_
#define CONSOLA_V2_CONSOLA_H_

#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/txt.h>
#include <commons/string.h>
#include <commons/collections/queue.h>
#include <commons/config.h>
#include <commons/error.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

typedef struct {
	int tamanio_id;
	char* identificador;
	t_queue* parametros;
} t_instruccion;

typedef enum {
	HANDSHAKE,
	ENVIO_DATOS,
	CONFIRMACION
} op_code;

typedef struct {
	int size;
	void* buffer;
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

#endif /* CONSOLA_V2_CONSOLA_H_ */
