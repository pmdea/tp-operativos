#include "conexion.h"
#include "utlis.h"
#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <semaphore.h>
#include <pthread.h>


// DEFINO LOS SEMAFOROS
sem_t grado_multiprogramacion; // Tiene en cuenta el numero obtenido de la config.
sem_t prioridad_SuspendedReady; // Binario con P.M.P
sem_t nuevoProcesoReady; // Binario con P.C.P
sem_t agregarAReady; // mutex cuando se agrega a ready o se lee
sem_t bloqueoMax; // Binario para saber cuando se bloqueo por mas tiempo del q tendria q estar
sem_t procesoExit ; // cuando se saca o agrega un proceso a exit mutex
sem_t respuestaCpu : // binario con el cpu para esperar mensaje
sen_t algo; // hay un sem q es tiene nombre algo pero no me acuerdo para q era
// DEFINO HILOS
pthread_t planificadorLargoPlazo;
pthread_t planificadorMedianoPlazo;
pthread_t planificadorCortoPlazo;

typedef struct {
	int id;
	int tamanio;
	t_list* instrucciones; // LISTA
	int program_counter;
	int tabla_paginas; // LISTA
	double estimacion_rafaga;
} pcb;

typedef struct {
	int tamanio_proceso;
	t_queue* instrucciones;
}t_proceso;

typedef struct {
	int socket_consola;
	int pcbVinculado;
} consola_pcb;

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
t_list* conexiones_pcb;

t_log* iniciar_logger_kernel(void);
t_config* iniciar_config_kernel(void);
void terminar_programa(t_log*, t_config*);
