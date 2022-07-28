#ifndef INCLUDEH_ESTRUCTURAS_H_
#define INCLUDEH_ESTRUCTURAS_H_

/* PAGINACION */
typedef struct{
	uint32_t nro_tp2; // referencia a la tabla de 2do nivel para la estructura
	uint32_t id;
} entrada_tp_1; //TP == Tabla Pagina

typedef struct{
	uint32_t frame; // frame al que apunta la pagina
	uint32_t id;
	uint8_t bit_presencia;
	uint8_t bit_uso;
	uint8_t bit_modified;
	uint32_t pag_proc_interna;
	uint64_t t_assigned;
} entrada_tp_2;

typedef struct{
	t_list* entradas;
	uint32_t pid;
	uint32_t id;
} tabla_pagina;

typedef struct{
	uint32_t id;
	uint8_t ocupado;
} frame_auxiliar;

typedef struct{
	uint32_t pid;
	void* swap;
	uint32_t size;
} proc_swap;

typedef enum{
	KERNEL = 1,
	CPU = 0
} id_mod;

typedef enum{
	HANDSHAKE = 1,
	READ = 2,
	WRITE = 3,
	COPY = 4,
	GET_PAG_NVL_2 = 5,
	GET_MARCO = 6
} oper_cpu;
typedef struct{
	int socket_cliente;
} args_thread;

typedef struct{
	uint32_t estado;
	uint32_t tamanio_pcb;
	uint32_t id_pcb;
} message_kernel;

typedef struct{
	uint32_t operacion;
	uint32_t size_data;
	uint32_t datos[4];
} message_cpu;

#endif /* INCLUDEH_ESTRUCTURAS_H_ */
