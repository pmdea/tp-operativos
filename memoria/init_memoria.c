#include "./includeh/init_memoria.h"

t_log* logger;
t_mem_config* config;

void* memoria_principal;

//paginacion
t_list* tablas_1er_nivel; //Se dejan como globales para utilizarlas como figura en el issue #2596
t_list* tablas_2do_nivel; //Cada t_list va a tener una t_list como elemento de entrada_tp1 y tp2 correspondientemente, que representará una tabla de paginas


uint8_t init(){
	config = crear_config();
	logger = log_create("memoria.log", "MEM", true, LOG_LEVEL_INFO);
	log_info(logger, "Logger creado!");
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
	tablas_1er_nivel = list_create(); // representa el espacio de memoria donde estaran todas las paginas de todos los procesos
	tablas_2do_nivel = list_create();
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
	config->algoritmo_reemplazo = config_get_string_value(file, ALGORITMO_REEMPLAZO); // CLOCK/CLOCK-M
	config->marcos_por_proceso = config_get_int_value(file, MARCOS_POR_PROCESO);
	config->retardo_swap = config_get_int_value(file, RETARDO_SWAP);
	config->path_swap = config_get_string_value(file, PATH_SWAP);

	log_info(logger, "Configuracion cargada correctamente");
	config_destroy(file); // mato el file del config
	return 1;
}

void finalizar_programa(){
	log_info(logger, "Finalizando programa...");
	log_destroy(logger);

	//mato config
	free(config->algoritmo_reemplazo);
	free(config->path_swap);
	free(config);

	//matar paginas y frames
	list_destroy(tablas_1er_nivel);
	list_destroy(tablas_2do_nivel);

	//mato memoria principal
	free(memoria_principal);

}
