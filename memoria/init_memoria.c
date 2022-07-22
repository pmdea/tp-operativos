#include "./includeh/init_memoria.h"

t_log* logger;
t_mem_config* config;

void* memoria_principal;

//paginacion
t_list* tablas_1er_nivel; //Se dejan como globales para utilizarlas como figura en el issue #2596
t_list* tablas_2do_nivel; //Cada t_list va a tener una t_list como elemento de entrada_tp1 y tp2 correspondientemente, que representará una tabla de paginas
t_list* frames_auxiliares;
t_queue* cola_reemplazo;
t_list* lista_swaps;

// Mutex
pthread_mutex_t mutex_swap;
pthread_mutex_t mutex_memoria;
pthread_mutex_t mutex_frames;
pthread_mutex_t mutex_pagina_1;
pthread_mutex_t mutex_pagina_2;
pthread_mutex_t mutex_cola_reemplazo;

uint8_t init(){
	config = crear_config();
	logger = log_create("memoria.log", "MEM", true, LOG_LEVEL_INFO);
	log_info(logger, "Logger creado!");
	return init_semaforos();
}

uint8_t init_semaforos(){
	pthread_mutex_init(&mutex_swap, NULL);
	pthread_mutex_init(&mutex_memoria, NULL);
	pthread_mutex_init(&mutex_frames, NULL);
	pthread_mutex_init(&mutex_pagina_1, NULL);
	pthread_mutex_init(&mutex_pagina_2, NULL);
	pthread_mutex_init(&mutex_cola_reemplazo, NULL);
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
	log_info(logger, "Memoria cargada correctamente!");
	return cargar_tablas_paginas();
}

uint8_t cargar_tablas_paginas(){
	log_info(logger, "Generando estructuras globales de tablas de paginas...");
	tablas_1er_nivel = list_create(); // representa el espacio de memoria donde estaran todas las paginas de todos los procesos
	tablas_2do_nivel = list_create();
	log_info(logger, "Estructuras globales de tablas de paginas creadas!");
	return cargar_frames_auxiliares();
}

uint8_t cargar_frames_auxiliares(){
	log_info(logger, "Generando estructura auxiliar para manejo de frames...");
	uint32_t nro_frames = config->tam_memoria/config->tam_pag;
	frames_auxiliares = list_create();
	for(uint32_t i = 0; i < nro_frames; i++){
		frame_auxiliar* frame_auxiliar = malloc(sizeof frame_auxiliar);
		frame_auxiliar->id = i;
		frame_auxiliar->ocupado = 0;
		list_add(frames_auxiliares, frame_auxiliar);
	}
	log_info(logger, "El módulo tiene %d frames disponibles!", nro_frames);
	log_info(logger, "Estructura auxiliar para manejo de frames creada!");
	return cargar_queue_pags();
}

uint8_t cargar_queue_pags(){
	log_info(logger, "Generando estructura para swapping...");
	cola_reemplazo = queue_create();
	return cargar_lista_swaps();
}

uint8_t cargar_lista_swaps(){
	log_info(logger, "Generando lista de archivos swap...");
	lista_swaps = list_create();
	return 1;
}


uint8_t cargar_config(){
	t_config* file = config_create("memoria.config");
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

	log_info(logger, "Configuracion cargada correctamente");
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

void finalizar_programa(){
	log_info(logger, "Finalizando programa...");
	log_destroy(logger);
	//Mato mutex
	pthread_mutex_destroy(&mutex_swap);
	pthread_mutex_destroy(&mutex_memoria);
	pthread_mutex_destroy(&mutex_frames);
	pthread_mutex_destroy(&mutex_pagina_1);
	pthread_mutex_destroy(&mutex_pagina_2);
	pthread_mutex_destroy(&mutex_cola_reemplazo);

	//mato config
	free(config->algoritmo_reemplazo);
	free(config->path_swap);
	free(config);

	//matar cola_auxiliar
	queue_destroy(cola_reemplazo);// Las páginas las libero cuando libero las tablas, no antes para evitar double free
	//matar frames auxiliares
	list_destroy_and_destroy_elements(frames_auxiliares, (void*) frame_destroyer);
	//matar paginas y frames
	list_destroy_and_destroy_elements(tablas_2do_nivel, (void*) tp_destroy);
	list_destroy_and_destroy_elements(tablas_1er_nivel, (void*) tp_destroy);

	//mato memoria principal
	free(memoria_principal);

}
