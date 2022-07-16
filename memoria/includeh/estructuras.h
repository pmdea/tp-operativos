/*
 * estructuras.h
 *
 *      Author: pmdea
 */

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

#endif /* INCLUDEH_ESTRUCTURAS_H_ */
