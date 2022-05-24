/*
 * kernel.h
 *
 *  Created on: 13 may. 2022
 *      Author: utnso
 */

#ifndef SRC_KERNEL_H_
#define SRC_KERNEL_H_


#include <commons/string.h>
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
	MENSAJE,
	PAQUETE
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
