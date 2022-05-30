/*
 * kernel.h
 *
 *  Created on: 13 may. 2022
 *      Author: utnso
 */

#ifndef SRC_KERNEL_H_
#define SRC_KERNEL_H_


#include <commons/string.h>
#include <commons/config.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <commons/log.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <stdio.h>
#include <stdlib.h>

/*
*/

typedef enum
{
	HANDSHAKE,
	ENVIO_DATOS,
	MENSAJE,
	CONFIRMACION
}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

typedef struct {
	int tamanio_id;
	char* identificador;
	t_queue* parametros;
}t_instruccion;

typedef struct {
	int tamanio_proceso;
	t_queue* instrucciones;
}t_proceso;

typedef struct {
	int tamanio_mensaje;
	char* mensaje;
}t_mensaje;

typedef struct
{
	char *id;
	int tamanio;
	t_list *instrucciones;
	t_list *pc;
	char *tabla_paginas;
	int estimacion_rafaga;
} pcb;

#endif /* SRC_KERNEL_H_ */


