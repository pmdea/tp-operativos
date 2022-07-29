#ifndef INIT_MEMORIA_H_


#define INIT_MEMORIA_H_

#include <stdint.h>
#include<commons/log.h>
#include<commons/config.h>
#include<stdlib.h>
#include <string.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/string.h>
#include "estructuras.h"
#include "servidor.h"
#include "manejar_swap.h"
#include <pthread.h>
#include <time.h>

typedef struct {
	int puerto_escucha;
	int tam_memoria;
	int tam_pag;
	int entradas_por_tabla;
	int retardo_memoria;
	char* algoritmo_reemplazo;
	int marcos_por_proceso;
	int retardo_swap;
	char* path_swap;
	char* ip;

} t_mem_config; // struct config memoria

/* Datos globales */
#define LOG_FILE "memoria.log"
#define PROC_NAME "MEMORIA"
#define CONF_FILE "./memoria.config"
#define PUERTO_ESCUCHA "PUERTO_ESCUCHA"
#define TAM_MEMORIA "TAM_MEMORIA"
#define TAM_PAGINA "TAM_PAGINA"
#define ENTRADAS_POR_TABLA "ENTRADAS_POR_TABLA"
#define RETARDO_MEMORIA "RETARDO_MEMORIA"
#define ALGORITMO_REEMPLAZO "ALGORITMO_REEMPLAZO"
#define MARCOS_POR_PROCESO "MARCOS_POR_PROCESO"
#define RETARDO_SWAP "RETARDO_SWAP"
#define PATH_SWAP "PATH_SWAP"
#define IP "IP"


uint8_t init(char* path); // inicializa el loger y config
uint8_t cargar_memoria();
uint8_t cargar_config(); // popula el struct de config
void finalizar_programa(); //Limpia para cerrar el programa
t_mem_config* crear_config();
uint8_t cargar_tablas_paginas();
uint8_t cargar_frames_auxiliares();
uint8_t cargar_lista_swaps();
uint8_t init_semaforos(); // inicializar mutex

void finalizar_swap();

#endif /* INIT_MEMORIA_H_ */
