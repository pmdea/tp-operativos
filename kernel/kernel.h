#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<commons/collections/queue.h>
#include<semaphore.h>
#include<pthread.h>

// Defino variable global
int tiemposBlockedSuspended;

// Defino Sockets
int socket_memoria;
int socket_cpu_dispatch;
int socket_cpu_interrupt;

// DEFINO LOG Y CONFIG
t_log* loggerKernel;
t_config* configKernel;

// DEFINO LOS SEMAFOROS
sem_t grado_multiprogramacion; // Tiene en cuenta el numero obtenido de la config.
sem_t prioridad_SuspendedReady; // Binario con P.M.P
sem_t nuevoProcesoReady; // Binario con P.C.P
sem_t mutexReady; // mutex cuando se agrega a ready o se lee
sem_t bloqueoMax; // Binario para saber cuando se bloqueo por mas tiempo del q tendria q estar
sem_t mutexExit; // cuando se saca o agrega un proceso a exit mutex
sem_t mutexBloqueo; // Adm bloqueos con SRT
sem_t procesoBloqueado; // hay un sem q es tiene nombre algo pero no me acuerdo para q era

// DEFINO HILOS
pthread_t planificadorLargoPlazoHilo;
pthread_t planificadorMedianoPlazoHilo;
pthread_t planificadorCortoPlazoHilo;
pthread_t administradorBloqueosHilo;
pthread_t ejecucionAlgoritmoHilo;

// DEFINO LISTAS
t_list* procesosNew;
t_list* procesosReady;
t_list* procesosExecute;
t_list* procesosBlocked;
t_list* procesosSuspendedBlocked;
t_list* procesosSuspendedReady;
t_list* procesosExit;
t_list* conexiones_pcb;
t_list* tiemposBlocked;

// DEFINO ESTRUCTURAS
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

typedef struct {
    int id;
    int tamanio;
    t_list* instrucciones; // LISTA
    int program_counter;
    int tabla_paginas; // LISTA
    double estimacion_rafaga;
} pcb;

typedef struct {
	int tamanio_id;
	char* identificador;
	t_queue* parametros;
}t_instruccion;

typedef struct {
    int tamanio_proceso;
    t_queue* instrucciones;
}t_proceso;

typedef struct {
    int socket_consola;
    int pcbVinculado;
} consola_pcb;

//log_config.c
t_log* iniciar_logger_kernel(void);
t_config* iniciar_config_kernel(void);
void terminar_programa(t_log* logger, t_config* config);
//funciones_pcb.c
void generar_PCB(int idPCB, t_proceso* proceso);

//Planificadores
void planificador_LargoPlazo();
void planificador_CortoPlazo();
void administrar_bloqueos();

//Algoritmos
void algoritmo_FIFO();
void algoritmo_SRT();
void avisar_a_planificador_LP(pcb* pcbFinalizado);
