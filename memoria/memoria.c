#include "./includeh/memoria.h"
extern t_mem_config* config;
extern t_log* logger;
extern t_list* lista_swaps;
extern t_list* tablas_1er_nivel;
extern t_list* tablas_2do_nivel;
extern t_list* frames_auxiliares;

uint32_t iniciar_proc(uint32_t proc_mem, uint32_t pid){
	log_info(logger, "****INIT PROC size %d pid %d****", proc_mem, pid);
	uint32_t id_tabla = crear_tabla_1er_nivel(proc_mem, pid);
	proc_swap* swap = malloc(sizeof(proc_swap));
	swap->pid = pid;
	swap->size = proc_mem;
	swap->swap = crear_swap(proc_mem, pid);
	list_add(lista_swaps, swap);
	log_info(logger, "****INIT PID %d FINALIZADA!****", pid);
	return id_tabla;
}
void suspender_proc(uint32_t pid){
	tabla_pagina* tp1 = buscar_tabla_por_pid(tablas_1er_nivel, pid);
	log_info(logger, "****INIT SUSPENSION PID %d****", tp1->pid);
	t_list_iterator* tp1_iter = list_iterator_create(tp1->entradas);
	while(list_iterator_has_next(tp1_iter)){
		entrada_tp_1* entrada_1 = list_iterator_next(tp1_iter);
		tabla_pagina* tp2 = buscar_tabla_por_id(tablas_2do_nivel, entrada_1->nro_tp2);
		t_list_iterator* tp2_iter = list_iterator_create(tp2->entradas);
		while(list_iterator_has_next(tp2_iter)){
			entrada_tp_2* pag = list_iterator_next(tp2_iter);
			if(pag->bit_presencia){
				pag_a_swap(pag, tp1->pid, obtener_swap_por_pid(tp1->pid)->swap);
				frame_auxiliar* frame = list_get(frames_auxiliares, pag->frame);
				frame->ocupado = 0; //libero el frame
			}
		}
		list_iterator_destroy(tp2_iter);
	}
	list_iterator_destroy(tp1_iter);
	log_info(logger, "****PID %d SUSPENDIDO!****", tp1->pid);
}
void finalizar_proc(uint32_t pid){
	log_info(logger, "****FINALIZANDO PID %d****", pid);
	uint8_t _is_proc_swap(proc_swap* proc){
		return proc->pid == pid;
	}
	proc_swap* proc_swap = list_remove_by_condition(lista_swaps, (void*) _is_proc_swap);
	eliminar_swap(pid, proc_swap->swap, proc_swap->size);
	borrar_memoria_proceso(pid);
	free(proc_swap);
	log_info(logger, "****PID %d FINALIZADO****", pid);
}
uint32_t get_tabla_2do_lvl(uint32_t id_tabla_1, uint32_t entrada){
	log_info(logger, "****BUSCANDO TABLA 2DO NIVEL PARA %d ENTRADA %d****", id_tabla_1, entrada);
	uint32_t id_tabla_2 = obtener_id_tabla_2do(id_tabla_1, entrada);
	log_info(logger, "****Tabla de 2do nivel encontrada: %d****", id_tabla_2);
	esperar_response_cpu();
	return id_tabla_2;
}
uint32_t get_nro_marco(uint32_t tabla, uint32_t id_tabla_2, uint32_t entrada){
	log_info(logger, "****BUSCANDO MARCO PARA TABLA 1ER %d TABLA 2DO %d ENTRADA %d ****", tabla, id_tabla_2, entrada);
	uint32_t marco = obtener_nro_marco(tabla, id_tabla_2, entrada);
	log_info(logger, "****MARCO %d OBTENIDO****", marco);
	esperar_response_cpu();
	return marco;
}
uint32_t leer_en_memoria(uint32_t id_2do_nivel, uint32_t id_entrada, uint32_t offset){
	uint32_t* data = leer_de_memoria(offset, sizeof(uint32_t));
	log_info(logger, "****LEIDO DE MEMORIA %d ****", *data);
	marcar_pag_mod_uso(id_2do_nivel, id_entrada, 0);
	uint32_t result = *data;
	free(data);
	esperar_response_cpu();
	return result;
}
char* escribir_memoria(uint32_t id_2do_nivel, uint32_t id_entrada, uint32_t offset, void* data){
	log_info(logger, "****ESCRIBIENDO EN MEM %d DATA: %d****", offset, *((uint32_t*)data));
	char* response = escribir_en_memoria(offset, data, sizeof(uint32_t));
	marcar_pag_mod_uso(id_2do_nivel, id_entrada, 1);
	esperar_response_cpu();
	return response;
}

void esperar_response_cpu(){
	usleep(config->retardo_memoria * 1000);
}
