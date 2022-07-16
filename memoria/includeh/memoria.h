/*
 * memoria.h
 *
 *      Author: pmdea
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<stdio.h>
#include<inttypes.h>
#include<unistd.h>
#include "init_memoria.h"
#include "manejar_memoria.h"
#include "manejar_paginacion.h"
#include "manejar_swap.h"

uint32_t iniciar_proc();
void suspender_proc();
void finalizar_proc();
uint32_t get_tabla_2do_lvl(uint32_t id_tabla_1, uint32_t entrada);
uint32_t get_nro_marco(uint32_t pid, uint32_t id_tabla_2, uint32_t entrada);
uint32_t leer_en_memoria(uint32_t id_2do_nivel, uint32_t id_entrada, uint32_t offset);
char* escribir_memoria(uint32_t id_2do_nivel, uint32_t id_entrada, uint32_t offset, void* data);
void esperar_response_cpu();


#endif /* MEMORIA_H_ */
