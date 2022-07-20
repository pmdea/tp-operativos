/*
 * manejar_paginacion.h
 *
 *      Author: pmdea
 */

#ifndef INCLUDEH_MANEJAR_PAGINACION_H_
#define INCLUDEH_MANEJAR_PAGINACION_H_

#include "init_memoria.h"
#include "estructuras.h"
#include "manejar_memoria.h"
#include "manejar_swap.h"

uint32_t crear_tabla_1er_nivel(uint32_t proc_mem, uint32_t pid);
uint32_t crear_tabla_2do_nivel(uint32_t* proc_mem, uint32_t pid, uint32_t* index_swap);
entrada_tp_1* crear_pagina_1er_nivel(uint32_t index, uint32_t* proc_mem, uint32_t pid, uint32_t* index_swap);
entrada_tp_2* crear_pagina_2do_nivel(uint32_t index, uint32_t index_swap);
uint32_t obtener_nro_marco(uint32_t pid, uint32_t index_t, uint32_t entrada);
uint32_t obtener_id_tabla_2do(uint32_t id_tbl, uint32_t entrada);
uint8_t se_puede_asignar_marco(tabla_pagina* tbl_proc);
entrada_tp_2* algoritmo_reemplazo();
void pag_a_swap(entrada_tp_2* pag, uint32_t pid, void* swap);
void swap_a_pag(entrada_tp_2* pag, void* swap);
uint32_t ini_proceso(uint32_t p_size);
void borrar_memoria_proceso(uint32_t pid);
void cargar_pag_marco(tabla_pagina* tabla, entrada_tp_2* pag);
tabla_pagina* buscar_tabla_por_id(t_list* tablas, uint32_t id);
tabla_pagina* buscar_tabla_por_pid(t_list* tablas, uint32_t pid);
entrada_tp_2* buscar_entrada_2(t_list* entradas, uint32_t id);
entrada_tp_1* buscar_entrada_1(t_list* entradas, uint32_t id);
void marcar_pag_mod_uso(uint32_t id_2do_nivel, uint32_t id_entrada, uint8_t modificada);
proc_swap* obtener_swap_por_pid(uint32_t pid);
#endif /* INCLUDEH_MANEJAR_PAGINACION_H_ */
