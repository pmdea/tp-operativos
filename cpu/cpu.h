#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

typedef struct {
	int id;
	int tamanio;
	t_list* instrucciones; // LISTA
	int program_counter;
	char* tabla_paginas; // LISTA
	double estimacion_rafaga;
} Pcb;

typedef struct {
	int tamanio_id;
	char* identificador;
	t_queue* parametros;
} Instruccion;

typedef struct {
	int size;
	void* stream;
} __attribute__((packed))
t_buffer;

typedef enum {
	IO,
	EXIT,
	INTERRUPT,
} op_code;

typedef struct {
	op_code operacion;
	t_buffer* buffer;
}  __attribute__((packed))
t_paquete;

int conexion;
