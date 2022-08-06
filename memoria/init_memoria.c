#include "./includeh/init_memoria.h"

t_log* logger;
t_mem_config* config;

void* memoria_principal;

//paginacion
t_list* tablas_1er_nivel; //Se dejan como globales para utilizarlas como figura en el issue #2596
t_list* tablas_2do_nivel; //Cada t_list va a tener una t_list como elemento de entrada_tp1 y tp2 correspondientemente, que representará una tabla de paginas
t_list* frames_auxiliares;
t_list* lista_swaps;

// Mutex
pthread_mutex_t mutex_swap;
pthread_mutex_t mutex_memoria;
pthread_mutex_t mutex_frames;
pthread_mutex_t mutex_pagina_1;
pthread_mutex_t mutex_pagina_2;

char* path_config;

uint8_t init(char* path){
	path_config = path;
	config = crear_config();
	logger = log_create("memoria.log", "MEM", true, LOG_LEVEL_INFO);
	log_debug(logger, "Logger creado!");
	return init_semaforos();
}

uint8_t init_semaforos(){
	pthread_mutex_init(&mutex_swap, NULL);
	pthread_mutex_init(&mutex_memoria, NULL);
	pthread_mutex_init(&mutex_frames, NULL);
	pthread_mutex_init(&mutex_pagina_1, NULL);
	pthread_mutex_init(&mutex_pagina_2, NULL);
	return 1;
}

t_mem_config* crear_config(){
	t_mem_config* conf = malloc(sizeof(t_mem_config));
	conf->algoritmo_reemplazo = NULL; //Inicializo char* con null por seguridad
	conf->path_swap = NULL;
	return conf;
}


uint8_t cargar_memoria(){
	memoria_principal = malloc(config->tam_memoria);
	if(memoria_principal == NULL){
		log_error(logger, "Fallo malloc de memoria principal");
		return 0;
	}
	memset(memoria_principal, 0, config->tam_memoria); // le escribo 0s a toda la memoria
	log_info(logger, "****MEMORIA SIZE %d INICIALIZADA****", config->tam_memoria);
	return cargar_tablas_paginas();
}

uint8_t cargar_tablas_paginas(){
	tablas_1er_nivel = list_create(); // representa el espacio de memoria donde estaran todas las paginas de todos los procesos
	tablas_2do_nivel = list_create();
	log_info(logger, "****ESTRUCTURAS TABLAS CREADAS****");
	return cargar_frames_auxiliares();
}

uint8_t cargar_frames_auxiliares(){
	uint32_t nro_frames = config->tam_memoria/config->tam_pag;
	frames_auxiliares = list_create();
	for(uint32_t i = 0; i < nro_frames; i++){
		frame_auxiliar* frame_auxiliar = malloc(sizeof frame_auxiliar);
		frame_auxiliar->id = i;
		frame_auxiliar->ocupado = 0;
		list_add(frames_auxiliares, frame_auxiliar);
	}
	log_info(logger, "****MÓDULO CARGADO CON %d FRAMES****", nro_frames);
	return cargar_lista_swaps();
}

uint8_t cargar_lista_swaps(){
	lista_swaps = list_create();
	struct stat st = {0};
	if (stat(config->path_swap, &st) == -1) {
	    mkdir(config->path_swap, 0777);
	    log_warning(logger, "Path %s no existia. Se creo.", config->path_swap);
	}
	log_info(logger, "****ESTRUCTURA SWAP CREADA****");
	return 1;
}


uint8_t cargar_config(){
	t_config* file = config_create(path_config);
	if(file == NULL){
		log_error(logger, "No se encontro memoria.config");
		return 0;
	}

	config->puerto_escucha = config_get_int_value(file, PUERTO_ESCUCHA);
	config->tam_memoria = config_get_int_value(file, TAM_MEMORIA);
	config->tam_pag = config_get_int_value(file, TAM_PAGINA);
	config->entradas_por_tabla = config_get_int_value(file, ENTRADAS_POR_TABLA);
	config->retardo_memoria = config_get_int_value(file, RETARDO_MEMORIA);
	config->algoritmo_reemplazo = string_duplicate(config_get_string_value(file, ALGORITMO_REEMPLAZO)); // CLOCK/CLOCK-M
	config->marcos_por_proceso = config_get_int_value(file, MARCOS_POR_PROCESO);
	config->retardo_swap = config_get_int_value(file, RETARDO_SWAP);
	config->path_swap = string_duplicate(config_get_string_value(file, PATH_SWAP));
	config->ip = string_duplicate(config_get_string_value(file, IP));

	log_info(logger, "****CONFIG CREADO CORRECTAMENTE****");
	config_destroy(file); // mato el file del config
	return 1;
}

static void tp_destroy(tabla_pagina *self){
	list_destroy_and_destroy_elements(self->entradas, (void*) free);
	free(self);
}

static void frame_destroyer(frame_auxiliar *self){
	free(self);
}


void finalizar_swap(){
	t_list_iterator* iter = list_iterator_create(lista_swaps);
	while(list_iterator_has_next(iter)){
		proc_swap* swap = list_iterator_next(iter);
		eliminar_swap(swap->pid, swap->swap, swap->size);
		free(swap);
	}
	list_iterator_destroy(iter);
	list_destroy(lista_swaps);
}

void finalizar_programa(){
	//Mato espacios swap
	finalizar_swap();

	log_info(logger, "****FINALIZANDO MEMORIA****");
	log_destroy(logger);
	//Mato mutex
	pthread_mutex_destroy(&mutex_swap);
	pthread_mutex_destroy(&mutex_memoria);
	pthread_mutex_destroy(&mutex_frames);
	pthread_mutex_destroy(&mutex_pagina_1);
	pthread_mutex_destroy(&mutex_pagina_2);

	//mato config
	free(config->algoritmo_reemplazo);
	free(config->path_swap);
	free(config->ip);
	free(config);

	//matar frames auxiliares
	list_destroy_and_destroy_elements(frames_auxiliares, (void*) frame_destroyer);
	//matar paginas y frames
	list_destroy_and_destroy_elements(tablas_2do_nivel, (void*) tp_destroy);
	list_destroy_and_destroy_elements(tablas_1er_nivel, (void*) tp_destroy);


	//mato memoria principal
	free(memoria_principal);
	finalizar_servidor();

}
