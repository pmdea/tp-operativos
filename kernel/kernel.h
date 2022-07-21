#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<commons/collections/queue.h>
#include<semaphore.h>
#include<pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <netdb.h>


// Varible Global
int ejecutando;

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
pthread_mutex_t mutexReady; // mutex cuando se agrega a ready o se lee
sem_t bloqueoMax; // Binario para saber cuando se bloqueo por mas tiempo del q tendria q estar
pthread_mutex_t mutexExit; // cuando se saca o agrega un proceso a exit mutex
pthread_mutex_t mutexBloqueo; // Adm bloqueos con SRT
pthread_mutex_t mutexBloqueoSuspendido; // Mutex Bloqueos entre C.P y M.P
pthread_mutex_t mutexSuspendido; // M.P y L.P
pthread_mutex_t mutexNew;
pthread_mutex_t variableEjecutando; // Mutex para la variable compartida de SRT
sem_t procesoBloqueado; // hay un sem q es tiene nombre algo pero no me acuerdo para q era
sem_t finalizoProceso; // Corto Plazo avisa a Largo Plazo FIN PROCESO

// DEFINO HILOS
// P.L.P
pthread_t planificadorLargoPlazoHilo;
pthread_t estadoReadyHilo;
pthread_t estadoExitHilo;
// P.M.P
pthread_t planificadorMedianoPlazoHilo;
pthread_t planificadorCortoPlazoHilo;
pthread_t administradorBloqueosHilo;
pthread_t ejecucionAlgoritmoHilo;
pthread_t administradorRespuestaCPUHilo;
pthread_t ejecucionProcesoSRTHilo;

// DEFINO LISTAS
t_list* procesosNew;
t_list* procesosReady;
t_list* procesosExecute;
t_list* procesosBlocked;
t_list* procesosSuspendedBlocked;
t_list* procesosSuspendedReady;
t_list* procesosExit;

t_log* iniciar_logger_kernel(void);
t_config* iniciar_config_kernel(void);
void terminar_programa(t_log*, t_config*);
