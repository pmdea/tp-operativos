#ifndef SRC_KERNEL_H_
#define SRC_KERNEL_H_

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
#include <pthread.h>

typedef enum {
	HANDSHAKE,
	ENVIO_PROCESO,
	MENSAJE,
	CONFIRMACION,
	FINALIZACION_PROCESO,
	ERROR
} op_code;

typedef struct{
	uint32_t id;
	uint32_t tamanio;
	t_list* instrucciones;
	uint32_t program_counter;
	uint32_t tabla_paginas;
	double estimacion_rafaga;
}PCB;

typedef struct {
    int tamanio_proceso;
    t_queue* instrucciones;
}t_proceso;

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

#define PUERTO "8000"
#define MENSAJE_HANDSHAKE_ESPERADO 9992751
#define IP "127.0.0.1"
#endif /* SRC_KERNEL_H_ */

