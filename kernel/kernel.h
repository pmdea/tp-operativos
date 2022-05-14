#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<commons/collections/list.h>
#include <semaphore.h>
#include <pthread.h>

typedef struct {
	int id;
	int tamanio;
	char* instrucciones; // LISTA
	int program_counter;
	char* tabla_paginas; // LISTA
	int estimacion_rafaga;
} pcb;

typedef struct {
	char* ip_memoria;
	int puerto_memoria;
	char* ip_cpu;
	int puerto_cpu_dispatch;
	int puerto_cpu_interrupt;
	int puerto_escucha;
	char* algoritmo_planificacion;
	int estimacion_inicial;
	double alfa;
	int grado_multiprogramacion;
	int tiempo_maximo_bloqueado;

} KERNEL_CONFIG;
KERNEL_CONFIG config_kernel;

t_list* procesosNew;
t_list* procesosReady;
t_list* procesosExecute;
t_list* procesosBlocked;
t_list* procesosSuspendedBlocked;
t_list* procesosSuspendedReady;
t_list* procesosExit;

t_log* iniciar_logger_kernel(void);
void terminar_programa(t_log*);
