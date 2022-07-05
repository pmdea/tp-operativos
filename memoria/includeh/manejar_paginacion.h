/*
 * manejar_paginacion.h
 *
 *  Created on: Jul 5, 2022
 *      Author: pmdea
 */

#ifndef INCLUDEH_MANEJAR_PAGINACION_H_
#define INCLUDEH_MANEJAR_PAGINACION_H_

#include "init_memoria.h"
#include "estructuras.h"

uint32_t crear_tabla_1er_nivel();
uint32_t crear_tabla_2do_nivel();
entrada_tp_1* crear_pagina_1er_nivel(uint32_t index);
entrada_tp_2* crear_pagina_2do_nivel(uint32_t index);
uint32_t obtener_nro_marco(uint32_t index_t, uint32_t entrada);
#endif /* INCLUDEH_MANEJAR_PAGINACION_H_ */
