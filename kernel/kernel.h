#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<commons/collections/queue.h>
#include<commons/config.h>
#include "pthread.h"
#include "semaphore.h"
#include <errno.h>

#define MENSAJE_HANDSHAKE_ESPERADO 9992751


//VAR GLOBAL
int ejecutando;

//SOCKETS
int socket_memoria;
int socket_dispatch;
int socket_interrupt;

// Logs y Config
t_log* loggerKernel;
t_config* configKernel;

// LISTAS
t_list* procesosNew;
t_list* procesosReady;
t_list* procesosExecute;
t_list* procesosBlocked;
t_list* procesosSuspendedReady;
t_list* procesosExit;
t_list* conexiones_pcb;

// SEMAFOROS
sem_t grado_multiprogramacion; // Tiene en cuenta el numero obtenido de la config.
sem_t prioridad_SuspendedReady; // Binario con P.M.P
sem_t nuevoProcesoReady; // Binario con P.C.P
sem_t enviarInterrupcion;
sem_t bloqueoMax; // Binario para saber cuando se bloqueo por mas tiempo del q tendria q estar
sem_t procesoBloqueado; // hay un sem q es tiene nombre algo pero no me acuerdo para q era
sem_t finalizoProceso; // Corto Plazo avisa a Largo Plazo FIN PROCESO
sem_t hayProcesoAnalizar; // Consola - L - Exit - Bloqueos
pthread_mutex_t mutexReady; // mutex cuando se agrega a ready o se lee
pthread_mutex_t mutexExit; // cuando se saca o agrega un proceso a exit mutex
pthread_mutex_t mutexBloqueo; // Adm bloqueos con SRT
pthread_mutex_t mutexBloqueoSuspendido; // Mutex Bloqueos entre C.P y M.P
pthread_mutex_t mutexSuspendido; // M.P y L.P
pthread_mutex_t mutexNew;
pthread_mutex_t variableEjecutando; // Mutex para la variable compartida de SRT

// HILOS (L - M - C)
pthread_t estadoReadyHilo; // L
pthread_t estadoExitHilo; // L

pthread_t gestionBloqueoHilo; // C
pthread_t algoritmoHilo; // C
pthread_t desalojoSRTHilo; // C



// ENUMS

typedef enum {
	HANDSHAKE,
	ENVIO_PROCESO,
	MENSAJE,
	CONFIRMACION,
	FINALIZACION_PROCESO,
	ERROR
} op_code;

typedef enum{
	NO_OP,
	IO,
	READ,
	WRITE,
	COPY,
	EXIT,
} ID_INSTRUCCION;

typedef enum{
	EXIT_PCB,
	IO_PCB,
	DESALOJO_PCB,
} ID_CPU;

typedef enum {
    INICIALIZA,
    SUSPENDE,
    FINALIZA,
}OP_MEMORIA;

typedef enum{
	KERNEL = 1,
} id_mod;

// ESTRUCTURAS
typedef struct{
	uint32_t id;
	uint32_t tamanio;
	t_list* instrucciones;
	uint32_t program_counter;
	uint32_t tabla_paginas;
	double estimacion_rafaga;
}PCB;

typedef struct{
	 PCB* unPCB;
	 uint32_t tiempo;
	 uint32_t aux;
	 uint32_t suspendido;
}blockedPCB;

typedef struct {
	ID_INSTRUCCION identificador;
	t_queue* parametros;
}t_instruccion;

typedef struct {
    int tamanio_proceso;
    t_queue* instrucciones;
}t_proceso;

typedef struct {
    char* ip_memoria;
    char* puerto_memoria;
    char* ip_cpu;
    char* puerto_cpu_dispatch;
    char* puerto_cpu_interrupt;
    char* puerto_escucha;
    char* algoritmo_planificacion;
    int estimacion_inicial;
    double alfa;
    int grado_multiprogramacion;
    int tiempo_maximo_bloqueado;

} KERNEL_CONFIG;
KERNEL_CONFIG config_kernel;

typedef struct {
    int socket_consola;
    int pcbVinculado;
} consola_pcb;

// UTILS.C
int crear_conexion(char *ip, char* puerto);
void iniciar_settings();
void iniciar_conexiones();
void iniciar_listas();
void iniciar_semaforos();
void iniciar_planificadores();
void conectar_a_memoria(int socket);
void pcb_destroyer(PCB* pcb);

//PLANIFICADORES
void gestionNewSuspended(); // L
void gestionExit(); // L
void gestionBloqueo_Suspension(); // C
void algoritmo_FIFO(); // C
void algoritmo_SRT();
void administradorInterrupcionCPU(); // C SRT
void avisar_a_cpu_interrupt();
void avisar_a_consola(PCB* pcbFinalizado);
int devolverID_PCB(int socket);
int devolverID_CONSOLA(PCB* unPCB);
//FUNCIONESPCB.C
PCB crearPCB(int idPCB, t_proceso* proceso);
void agregarEstadoNew(PCB* unPCB );
void generarEstructuraPCB(int idPCB, t_proceso* proceso);
void estimador(PCB* unPCB, double alfa, int rafaga_ejecutada);
bool ordenarSRT(PCB* unPCB,PCB* otroPCB);

//AVISOS_A_MEMORIA.C
void avisar_a_memoria(OP_MEMORIA operacion, PCB unPCB, t_log* logger);

//ENVIO_RECIBO_CPU.C
void enviarPCB(int socket_receptor, PCB unPCB, t_log* logger);
PCB* deserializarPCB(int socket_emisor);
t_list* recibirRespuestaCPU(int socket_emisor);
t_list* deserializarListaInstruccionesK(int emisor);
uint32_t tamanioParametros(t_list* lista);
int cantidad_de_parametros(ID_INSTRUCCION identificador);

//SERIALIZACIONESK.C
void* asignarMemoria(int cantidad);
int recibirMensaje(int socketEmisor, void* buffer, int bytesMaximos);
void enviarMensaje(int socket, void* mensaje, int tamanio);

void concatenarInt32(void* buffer, int* desplazamiento, uint32_t numero);
void concatenarDouble(void* buffer, int* desplazamiento, double numero);
void concatenarString(void* buffer, int* desplazamiento, char* mensaje);
uint32_t deserializarInt32(int emisor);
double deserializarDouble(int emisor);
char* deserializarString(int emisor);

// LOG_CONFIG.C
t_log* iniciar_logger_kernel(void);
t_config* iniciar_config_kernel(char* path);

void finalizar_programa();
