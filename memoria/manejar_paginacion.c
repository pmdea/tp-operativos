#include "includeh/manejar_paginacion.h"

extern t_list* tablas_1er_nivel;
extern t_list* tablas_2do_nivel;
extern t_mem_config* config;
static uint32_t index_tp = 0;
extern t_list* frames_auxiliares;
extern t_log* logger;
extern t_list* lista_swaps;

extern pthread_mutex_t mutex_swap;
extern pthread_mutex_t mutex_frames;
extern pthread_mutex_t mutex_pagina_1;
extern pthread_mutex_t mutex_pagina_2;
uint32_t crear_tabla_1er_nivel(uint32_t proc_mem, uint32_t pid){
	log_info(logger, "****CREANDO TABLAS PID %d****", pid);
	tabla_pagina* tabla = malloc(sizeof(tabla_pagina));
	tabla->id = index_tp;
	tabla->entradas = list_create();
	tabla->pid = pid;
	index_tp++;
	pthread_mutex_lock(&mutex_pagina_1);
	list_add(tablas_1er_nivel, tabla);
	pthread_mutex_unlock(&mutex_pagina_1);
	uint32_t index_swap = 0;
	for(uint32_t i = 0; i<config->entradas_por_tabla; i++){
		list_add(tabla->entradas, crear_pagina_1er_nivel(i, &proc_mem, pid, &index_swap));
		if(proc_mem == 0) // Ya se crearon todas las paginas necesarias para el tamaño del proceso
			break;
	}
	log_info(logger, "****TABLAS CREADAS PID %d****", pid);
	return tabla->id;
}
uint32_t crear_tabla_2do_nivel(uint32_t* proc_mem, uint32_t pid, uint32_t* index_swap){
	tabla_pagina* tabla = malloc(sizeof(tabla_pagina));
	tabla->id= index_tp;
	tabla->entradas = list_create();
	tabla->pid = pid;
	index_tp++;
	pthread_mutex_lock(&mutex_pagina_2);
	list_add(tablas_2do_nivel, tabla);
	pthread_mutex_unlock(&mutex_pagina_2);
	for(uint32_t i = 0; i<config->entradas_por_tabla; i++){
		list_add(tabla->entradas, crear_pagina_2do_nivel(i, *index_swap));
		*proc_mem = *proc_mem - config->tam_pag;
		*index_swap = *index_swap + 1;
		if(*proc_mem == 0)
			break;
	}
	return tabla->id;
}
entrada_tp_1* crear_pagina_1er_nivel(uint32_t index, uint32_t* proc_mem, uint32_t pid, uint32_t* index_swap){
	entrada_tp_1* pag = malloc(sizeof(entrada_tp_1));
	pag->id = index;
	pag->nro_tp2 = crear_tabla_2do_nivel(proc_mem, pid, index_swap);
	return pag;
}
entrada_tp_2* crear_pagina_2do_nivel(uint32_t index, uint32_t index_swap){
	entrada_tp_2* pag = malloc(sizeof(entrada_tp_2));
	pag->bit_modified = 0;
	pag->bit_presencia = 0;
	pag->bit_uso = 0;
	pag->id = index;
	pag->frame = -1; //Marca que no está el marco asignado
	pag->pag_proc_interna = index_swap;
	return pag;
}

uint32_t obtener_nro_marco(uint32_t tabla_1er, uint32_t id_table, uint32_t entrada){

	pthread_mutex_lock(&mutex_pagina_2);
	tabla_pagina* tabla2 = buscar_tabla_por_id(tablas_2do_nivel, id_table);
	pthread_mutex_unlock(&mutex_pagina_2);
	entrada_tp_2* entr = buscar_entrada_2(tabla2->entradas, entrada);
	pthread_mutex_lock(&mutex_pagina_1);
	tabla_pagina* tabla1 = buscar_tabla_por_id(tablas_1er_nivel, tabla_1er);
	pthread_mutex_unlock(&mutex_pagina_1);
	if(!entr->bit_presencia || entr->frame == -1){
		cargar_pag_marco(tabla1, entr);
	}
	return entr->frame;
}

uint32_t obtener_id_tabla_2do(uint32_t id_tbl, uint32_t entrada){
	pthread_mutex_lock(&mutex_pagina_1);
	tabla_pagina* tabla = buscar_tabla_por_id(tablas_1er_nivel, id_tbl);
	pthread_mutex_unlock(&mutex_pagina_1);
	entrada_tp_1* entr = buscar_entrada_1(tabla->entradas, entrada);
	return entr->nro_tp2;
}

uint8_t se_puede_asignar_marco(tabla_pagina* tbl_proc){
	int marcos_asignados = 0;
	t_list_iterator* iterator = list_iterator_create(tbl_proc->entradas);
	while(list_iterator_has_next(iterator)){
		entrada_tp_1* page = list_iterator_next(iterator);
		pthread_mutex_lock(&mutex_pagina_2);
		tabla_pagina* tabla_lvl_2 = buscar_tabla_por_id(tablas_2do_nivel, page->nro_tp2);
		t_list_iterator* page2_iterator = list_iterator_create(tabla_lvl_2->entradas);

		while(list_iterator_has_next(page2_iterator)){
			entrada_tp_2* entrada =  list_iterator_next(page2_iterator);
			if(entrada->bit_presencia){
				marcos_asignados++;
			}
		}
		list_iterator_destroy(page2_iterator);
		pthread_mutex_unlock(&mutex_pagina_2);
	}
	list_iterator_destroy(iterator);

	return marcos_asignados < config->marcos_por_proceso;
}

entrada_tp_2* algoritmo_reemplazo(t_list* paginas){ // Devuelve la página elegida para reemplazar
	int algoritmo = string_equals_ignore_case(config->algoritmo_reemplazo, "CLOCK") ? 0 : 1;
	log_info(logger, "****ALGORITMO DE REEMPLAZO %s****", config->algoritmo_reemplazo);
	switch(algoritmo){
		case 0: //CLOCK
			while(1){
				t_list_iterator* iterator = list_iterator_create(paginas);
				while(list_iterator_has_next(iterator)){
					entrada_tp_2* pag = list_iterator_next(iterator);
					if(pag->bit_uso)
						pag->bit_uso = 0;
					else{
						list_iterator_destroy(iterator);
						return pag;
					}
				}
				list_iterator_destroy(iterator);
			}
			break;
		case 1: ;//CLOCK-M
			uint8_t firstRun = true;
			log_debug(logger, "Primera corrida CLOCK-M");
			while(1){
				t_list_iterator* iterator = list_iterator_create(paginas);
				while(list_iterator_has_next(iterator)){
					entrada_tp_2* pag = list_iterator_next(iterator);
					if(firstRun){
						if(!pag->bit_uso && !pag->bit_modified)
							return pag;
					}
					else{
						if(!pag->bit_uso && pag->bit_modified)
							return pag;
						pag->bit_uso = 0;
					}
				}
				list_iterator_destroy(iterator);
				firstRun = !firstRun;
			}
			break;
		default:
			return NULL;
			break;
	}
}

void cargar_pag_marco(tabla_pagina* tabla, entrada_tp_2* pag){
	log_info(logger, "****CARGANDO PAG %d DE PID %d****", pag->id, tabla->pid);
	t_list* pags_con_presencia = list_create();
	log_debug(logger, "Generando lista con paginas con p=1");
	t_list_iterator* iterator = list_iterator_create(tabla->entradas);
	log_debug(logger, "Obteniendo ids paginas del proceso con presencia.");
	while(list_iterator_has_next(iterator)){
		entrada_tp_1* page = list_iterator_next(iterator);
		pthread_mutex_lock(&mutex_pagina_2);
		tabla_pagina* tabla_lvl_2 = buscar_tabla_por_id(tablas_2do_nivel, page->nro_tp2);
		t_list_iterator* page2_iterator = list_iterator_create(tabla_lvl_2->entradas);
		while(list_iterator_has_next(page2_iterator)){
			entrada_tp_2* entrada =  list_iterator_next(page2_iterator);
			if(entrada->bit_presencia)
				list_add(pags_con_presencia, entrada);
		}
		list_iterator_destroy(page2_iterator);
		pthread_mutex_unlock(&mutex_pagina_2);
	}
	list_iterator_destroy(iterator);

	int cant_pags_ocupadas = list_size(pags_con_presencia);
	if(cant_pags_ocupadas<config->marcos_por_proceso){
		log_debug(logger, "Buscando marco disponible...");
		t_list_iterator* frame_iterator = list_iterator_create(frames_auxiliares);
		pthread_mutex_lock(&mutex_frames);
		while(list_iterator_has_next(frame_iterator)){
			frame_auxiliar* frame = list_iterator_next(frame_iterator);
			if(!frame->ocupado){
				log_debug(logger, "Frame libre encontrado en id %d.", frame->id);
				pag->bit_presencia = 1;
				pag->bit_uso = 1;
				pag->frame = frame->id;
				pag->t_assigned = GetTimeStamp();
				frame->ocupado = 1;
				pthread_mutex_lock(&mutex_swap);
				proc_swap* swap = obtener_swap_por_pid(tabla->pid);
				swap_a_pag(pag, swap->swap);
				retardo_swap();
				pthread_mutex_unlock(&mutex_swap);
				break;
			}
		}
		list_iterator_destroy(frame_iterator);
		pthread_mutex_unlock(&mutex_frames);

		if(pag->frame == -1 || !pag->bit_presencia){
			sort_lista_por_tiempo(pags_con_presencia);
			log_info(logger, "****PAGE FAULT: No hay marco disponible para pag %d****", pag->id);
			entrada_tp_2* pag_reemplazar = algoritmo_reemplazo(pags_con_presencia);
			pag->bit_presencia = 1;
			pag->bit_uso = 1;
			pag->frame = pag_reemplazar->frame;
			pag->t_assigned = GetTimeStamp();
			pthread_mutex_lock(&mutex_swap);
			proc_swap* swap = obtener_swap_por_pid(tabla->pid);
			swap_a_pag(pag, swap->swap);
			pag_a_swap(pag_reemplazar, tabla->pid, swap->swap);
			retardo_swap();
			pthread_mutex_unlock(&mutex_swap);
			log_info(logger, "****FRAME %d CARGADA CON PAG %d ---- TENIA PAG %d****", pag->frame, pag->id, pag_reemplazar->id);
		}
	}else{
		sort_lista_por_tiempo(pags_con_presencia);
		log_info(logger, "****PAGE FAULT: No hay marco disponible para pag %d****", pag->id);
		entrada_tp_2* pag_reemplazar = algoritmo_reemplazo(pags_con_presencia);
		pag->bit_presencia = 1;
		pag->bit_uso = 1;
		pag->frame = pag_reemplazar->frame;
		pag->t_assigned = GetTimeStamp();
		pthread_mutex_lock(&mutex_swap);
		proc_swap* swap = obtener_swap_por_pid(tabla->pid);
		swap_a_pag(pag, swap->swap);
		pag_a_swap(pag_reemplazar, tabla->pid, swap->swap);
		retardo_swap();
		pthread_mutex_unlock(&mutex_swap);
		log_info(logger, "****FRAME %d CARGADA CON PAG %d ---- TENIA PAG %d****", pag->frame, pag->id, pag_reemplazar->id);
	}

	list_destroy(pags_con_presencia);
}


	void pag_a_swap(entrada_tp_2* pag, uint32_t pid, void* swap){
		log_info(logger, "****SWAP PAG->SWAP %d PID %d****", pag->id, pid);
		//seteo la pag con presencia 0 y uso 0
		pag->bit_presencia = 0;
		pag->bit_uso = 0;
		uint32_t direc_fisica = config->tam_pag * pag->frame;
		void* data = leer_de_memoria(direc_fisica, (uint32_t)config->tam_pag);
		uint32_t posicion = pag->pag_proc_interna * (uint32_t)config->tam_pag;
		if(pag->bit_modified)
			escribir_swap(swap, data, config->tam_pag, posicion);
		free(data);
		pthread_mutex_lock(&mutex_frames);
		frame_auxiliar* frame = list_get(frames_auxiliares, pag->frame);
		frame->ocupado = 0; //libero el frame
		pthread_mutex_unlock(&mutex_frames);
		log_info(logger, "****SWAP PAG->SWAP %d PID %d EXITOSO****", pag->id, pid);
	}

	void swap_a_pag(entrada_tp_2* pag, void* swap){
		log_info(logger, "****SWAP->PAG %d****", pag->id);
		void* data_swap = leer_swap(swap, config->tam_pag, pag->pag_proc_interna * config->tam_pag);
		escribir_en_memoria(pag->frame * config->tam_pag, data_swap, config->tam_pag);
		free(data_swap);
		pag->bit_presencia = 1;
		pag->bit_uso = 1;
		log_info(logger, "****SWAP SWAP->PAG %d EXITOSO****", pag->id);
	}

	void borrar_memoria_proceso(uint32_t pid){
		log_info(logger, "****LIBERANDO FRAMES PARA PID %d***", pid);
		pthread_mutex_lock(&mutex_pagina_1);
		tabla_pagina* tabla = buscar_tabla_por_pid(tablas_1er_nivel, pid);
		pthread_mutex_unlock(&mutex_pagina_1);
		log_debug(logger, "Encontrada tabla id %d para pid %d!", tabla->id, pid);
		t_list_iterator* iterator1 = list_iterator_create(tabla->entradas);
		log_debug(logger, "Buscando paginas id %d para pid %d!", tabla->id, pid);
		while(list_iterator_has_next(iterator1)){
			entrada_tp_1* page = list_iterator_next(iterator1);
			pthread_mutex_lock(&mutex_pagina_2);
			tabla_pagina* tabla_lvl_2 = buscar_tabla_por_id(tablas_2do_nivel, page->nro_tp2);
			t_list_iterator* page2_iterator = list_iterator_create(tabla_lvl_2->entradas);
			while(list_iterator_has_next(page2_iterator)){
				entrada_tp_2* entrada =  list_iterator_next(page2_iterator);
				if(entrada->bit_presencia){
					log_debug(logger, "Liberando memoria de pag %d frame %d", entrada->id, entrada->frame);
					liberar_memoria(entrada->frame, config->tam_pag);
					frame_auxiliar* frame = list_get(frames_auxiliares, entrada->frame);
					frame->ocupado = 0; //libero el frame
				}
			}
			pthread_mutex_unlock(&mutex_pagina_2);
			list_iterator_destroy(page2_iterator);
		}
		list_iterator_destroy(iterator1);
		log_info(logger, "****MEMORIA DE PID %d LIBERADA****", pid);
	}

	// FUNCIONES DE BÚSQUEDA GENERAL
	tabla_pagina* buscar_tabla_por_id(t_list* tablas, uint32_t id){
		uint8_t _is_table(tabla_pagina* tabla){
			return tabla->id == id;
		}
		return list_find(tablas, (void*) _is_table);
	}

	tabla_pagina* buscar_tabla_por_pid(t_list* tablas, uint32_t pid){
		uint8_t _is_table(tabla_pagina* tabla){
			return tabla->pid == pid;
		}
		return list_find(tablas, (void*) _is_table);
	}

	entrada_tp_1* buscar_entrada_1(t_list* entradas, uint32_t id){
		uint8_t _is_entrada(entrada_tp_1* fila){
			return fila->id == id;
		}
		return list_find(entradas, (void*) _is_entrada);
	}
	entrada_tp_2* buscar_entrada_2(t_list* entradas, uint32_t id){
		uint8_t _is_entrada(entrada_tp_2* fila){
			return fila->id == id;
		}
		return list_find(entradas, (void*) _is_entrada);
	}

	void marcar_pag_mod_uso(uint32_t id_2do_nivel, uint32_t id_entrada, uint8_t modificada){
		log_info(logger, "Marcando pag %d de tabla 2do nivel %d como en uso", id_2do_nivel, id_entrada);
		pthread_mutex_lock(&mutex_pagina_2);
		tabla_pagina* segundo = buscar_tabla_por_id(tablas_2do_nivel, id_2do_nivel);
		entrada_tp_2* pag = buscar_entrada_2(segundo->entradas, id_entrada);
		pag->bit_uso = 1;
		if(modificada){
			log_info(logger, "Marcando pag %d de tabla 2do nivel %d como modificada", id_2do_nivel, id_entrada);
			pag->bit_modified = (modificada)? modificada : pag->bit_modified;
		}

		pthread_mutex_unlock(&mutex_pagina_2);
	}

	proc_swap* obtener_swap_por_pid(uint32_t pid){
		uint8_t _is_proc_swap(proc_swap* proc){
			return proc->pid == pid;
		}
		return list_find(lista_swaps, (void*) _is_proc_swap);
	}

	void sort_lista_por_tiempo(t_list* list){
	    bool pag_antes_de_pag(void* e1, void* e2) {
	        entrada_tp_2* pag1 = e1;
	        entrada_tp_2* pag2 = e2;
	        return pag1->t_assigned < pag2->t_assigned;
	    }
	    list_sort(list, &pag_antes_de_pag);
	}

	uint64_t GetTimeStamp() {
	    struct timeval tv;
	    gettimeofday(&tv,NULL);
	    return tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
	}
