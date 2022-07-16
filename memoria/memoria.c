/*
 * main.c
 *
 *      Author: pmdea
 */
#include "./includeh/memoria.h"
extern t_mem_config* config;
extern t_log* logger;
extern t_list* lista_swaps;
extern t_list* tablas_1er_nivel;
extern t_list* tablas_2do_nivel;

uint32_t iniciar_proc(uint32_t proc_mem, uint32_t pid){
	log_info(logger, "Inicializando proceso size %d pid %d", proc_mem, pid);
	uint32_t id_tabla = crear_tabla_1er_nivel(proc_mem, pid);
	proc_swap* swap = malloc(sizeof(proc_swap));
	swap->pid = pid;
	swap->size = proc_mem;
	swap->swap = crear_swap(proc_mem, pid);
	list_add(lista_swaps, swap);
	esperar_response_cpu();
	log_info(logger, "Inicializacion finalizada para proceso pid %d!", pid);
	return id_tabla;
}
void suspender_proc(uint32_t id_tabla){
	tabla_pagina* tp1 = buscar_tabla_por_id(tablas_1er_nivel, id_tabla);
	log_info(logger, "Incializando suspension del pid %d", tp1->pid);
	t_list_iterator* tp1_iter = list_iterator_create(tp1->entradas);
	while(list_iterator_has_next(tp1_iter)){
		entrada_tp_1* entrada_1 = list_iterator_next(tp1_iter);
		tabla_pagina* tp2 = buscar_tabla_por_id(tablas_2do_nivel, entrada_1->nro_tp2);
		t_list_iterator* tp2_iter = list_iterator_create(tp2->entradas);
		while(list_iterator_has_next(tp2_iter)){
			entrada_tp_2* pag = list_iterator_next(tp2_iter);
			if(pag->bit_presencia){
				pag_a_swap(pag, tp1->pid, obtener_swap_por_pid(tp1->pid));
			}
		}
	}
	log_info(logger, "Se ha suspendido el pid %d correctamente!", tp1->pid);

	esperar_response_cpu();
}
void finalizar_proc(uint32_t pid){
	log_info(logger, "Finalizando proceso pid %d...", pid);
	uint8_t _is_proc_swap(proc_swap* proc){
		return proc->pid == pid;
	}
	proc_swap* proc_swap = list_remove_by_condition(lista_swaps, (void*) _is_proc_swap);
	eliminar_swap(pid, proc_swap->swap, proc_swap->size);
	borrar_memoria_proceso(pid);
	log_info(logger, "Proceso pid %d finalizado!", pid);
	esperar_response_cpu();
}
uint32_t get_tabla_2do_lvl(uint32_t id_tabla_1, uint32_t entrada){

	uint32_t id_tabla_2 = 0;
	esperar_response_cpu();
	return id_tabla_2;
}
uint32_t get_nro_marco(uint32_t pid, uint32_t id_tabla_2, uint32_t entrada){
	log_info(logger, "Buscando nro de marco para pid %d en tabla de 2nivel %d y entrada %d", pid, id_tabla_2, entrada);
	uint32_t marco = obtener_nro_marco(pid, id_tabla_2, entrada);
	log_info(logger, "Marco %d obtenido!", marco);
	esperar_response_cpu();
	return marco;
}
uint32_t leer_en_memoria(uint32_t id_2do_nivel, uint32_t id_entrada, uint32_t offset){
	uint32_t data = *((uint32_t*)leer_de_memoria(offset, sizeof(uint32_t)));
	log_info(logger, "Leido de memoria: %d", data);
	marcar_pag_mod_uso(id_2do_nivel, id_entrada);
	esperar_response_cpu();
	return data;
}
char* escribir_memoria(uint32_t id_2do_nivel, uint32_t id_entrada, uint32_t offset, void* data){
	log_info(logger, "Escribiendo en dirección física %d data: %d", offset, *((uint32_t*)data));
	char* response = escribir_en_memoria(offset, data, sizeof(uint32_t));
	marcar_pag_mod_uso(id_2do_nivel, id_entrada);
	esperar_response_cpu();
	return response;
}

void esperar_response_cpu(){
	usleep(config->retardo_memoria* 1000);
}
