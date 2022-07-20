//#include <commons/log.h>
//#include <commons/string.h>
//#include <commons/config.h>
//#include <commons/collections/list.h>
//#include <commons/collections/queue.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <readline/readline.h>
//#include <signal.h>
//#include <unistd.h>
//#include <sys/socket.h>
//#include <netdb.h>
//#include <string.h>
//#include <math.h>
//
//typedef struct {
//	int id;
//	int tamanio;
//	t_list* instrucciones; // LISTA
//	int program_counter;
//	int tabla_paginas; // LISTA
//	double estimacion_rafaga;
//} Pcb;

typedef struct {
	int pagina;
	int marco;
} Entrada_tlb;

//typedef struct {
//	int tamanio_id;
//	char* identificador;
//	t_queue* parametros;
//} Instruccion;
//
//typedef struct {
//	int size;
//	void* stream;
//} __attribute__((packed))
//t_buffer;
//
//typedef enum {
//	IO,
//	EXIT,
//	INTERRUPT,
//} op_code;
//
//typedef struct {
//	op_code operacion;
//	t_buffer* buffer;
//}  __attribute__((packed))
//t_paquete;
//
//typedef struct {
//	int entrada_tabla_1er_nivel;
//	int entrada_tabla_2do_nivel;
//	int desplazamiento;
//} __attribute__((packed))
//Direccion_logica
//;
//
//typedef struct {
//	int marco;
//	int desplazamiento;
//} __attribute__((packed))
//Direccion_fisica
//;
//
//typedef struct {
//	int tam_pag;
//	int nro_entradas;
//}__attribute__((packed))
//Config
//;
//
//int conexion;
//int socket_memoria;
