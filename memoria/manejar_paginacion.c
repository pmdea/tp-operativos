/*
 * manejar_paginacion.c
 *
 *  Created on: Jul 5, 2022
 *      Author: pmdea
 */


#include "includeh/manejar_paginacion.h"

extern t_list* tablas_1er_nivel;
extern t_list* tablas_2do_nivel;
extern t_mem_config* config;
static uint32_t index_tp_1 = 0;
static uint32_t index_tp_2 = 0;

uint32_t crear_tabla_1er_nivel(){
	tabla_pagina* tabla = malloc(sizeof(tabla_pagina));
	tabla->index = index_tp_1;
	tabla->entradas = list_create();
	index_tp_1++;
	list_add(tablas_1er_nivel, tabla);
	for(uint32_t i = 0; i<config->entradas_por_tabla; i++){
		list_add(tabla->entradas, crear_pagina_1er_nivel(i));
	}
	return tabla->index;
}
uint32_t crear_tabla_2do_nivel(){
	tabla_pagina* tabla = malloc(sizeof(tabla_pagina));
	tabla->index = index_tp_2;
	tabla->entradas = list_create();
	index_tp_2++;
	list_add(tablas_2do_nivel, tabla);
	for(uint32_t i = 0; i<config->entradas_por_tabla; i++){
		list_add(tabla->entradas, crear_pagina_2do_nivel(i));
	}
	return tabla->index;
}
entrada_tp_1* crear_pagina_1er_nivel(uint32_t index){
	entrada_tp_1* pag = malloc(sizeof(entrada_tp_1));
	pag->index = index;
	pag->nro_tp2 = crear_tabla_2do_nivel();
	return pag;
}
entrada_tp_2* crear_pagina_2do_nivel(uint32_t index){
	entrada_tp_2* pag = malloc(sizeof(entrada_tp_2));
	pag->bit_modified = 0;
	pag->bit_presencia = 0;
	pag->bit_uso = 0;
	pag->id = index;
	pag->frame = -1; //Marca que no está el marco asignado
	return pag;
}

uint32_t obtener_nro_marco(uint32_t id_table, uint32_t entrada){
	uint8_t _is_entrada(entrada_tp_2* fila){
		return fila->id == entrada;
	}
	uint8_t _is_table(tabla_pagina* tabla){
		return tabla->index == id_table;
	}
	tabla_pagina* tabla = list_find(tablas_2do_nivel, (void*) _is_table);
	entrada_tp_2* entr = list_find(tabla->entradas, (void*) _is_entrada);
	return entr->frame;
}

uint32_t obtener_id_tabla_2do(uint32_t id_tbl, uint32_t entrada){
	uint8_t _is_entrada(entrada_tp_1* fila){
		return fila->index == entrada;
	}
	uint8_t _is_table(tabla_pagina* tabla){
		return tabla->index == id_tbl;
	}
	tabla_pagina* tabla = list_find(tablas_1er_nivel, (void*) _is_table);
	entrada_tp_1* entr = list_find(tabla->entradas, (void*) _is_entrada);
	return entr->nro_tp2;
}

uint8_t se_puede_asignar_marco(tabla_pagina* tbl_proc){
	int marcos_asignados = 0;
	t_list_iterator* iterator = list_iterator_create(tbl_proc->entradas);
	while(list_iterator_has_next(iterator)){
		entrada_tp_1* page = list_iterator_next(iterator);
		//obtengo tabla de pagina
		int id_tbl = page->nro_tp2;
		uint8_t _is_table(tabla_pagina* tabla){
			return tabla->index == id_tbl;
		}
		tabla_pagina* tabla_lvl_2 = list_find(tablas_2do_nivel, (void*) _is_table);
		t_list_iterator* page2_iterator = list_iterator_create(tabla_lvl_2->entradas);

		while(list_iterator_has_next(page2_iterator)){
			entrada_tp_2* entrada =  list_iterator_next(page2_iterator);
			if(entrada->bit_presencia){
				marcos_asignados++;
			}
		}
		list_iterator_destroy(page2_iterator);
	}
	list_iterator_destroy(iterator);

	return marcos_asignados < config->marcos_por_proceso;
}

void pasar_page_a_swap(){

}



