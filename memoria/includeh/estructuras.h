/*
 * estructuras.h
 *
 *  Created on: Jul 3, 2022
 *      Author: pmdea
 */

#ifndef INCLUDEH_ESTRUCTURAS_H_
#define INCLUDEH_ESTRUCTURAS_H_

/* PAGINACION */
typedef struct{
	uint32_t nro_tp2; // referencia a la tabla de 2do nivel para la estructura
	uint32_t index;
} entrada_tp_1; //TP == Tabla Pagina

typedef struct{
	uint32_t frame; // frame al que apunta la pagina
	uint32_t id;
	uint8_t bit_presencia;
	uint8_t bit_uso;
	uint8_t bit_modified;
} entrada_tp_2;

typedef struct{
	t_list* entradas;
	uint32_t index;
} tabla_pagina;

#endif /* INCLUDEH_ESTRUCTURAS_H_ */
