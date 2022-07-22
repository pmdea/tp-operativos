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

#define CONFIRM_ENVIADA "RECIBIDO :)"
#define MENSAJE_HANDSHAKE_ESPERADO "HOLA DON PEPITO"
#define MENSAJE_HANDSHAKE_ENVIADO "HOLA DON JOSE"

typedef struct {
	int tamanio_mensaje;
	char* mensaje;
}t_mensaje;

typedef enum
{
	HANDSHAKE,
	ENVIO_DATOS,
	MENSAJE,
	CONFIRMACION
}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

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
t_list* conexiones_pcb;
t_list* tiemposBlocked;
t_list* tiemposBlockedSuspendMax;

// DEFINO ESTRUCTURAS
typedef enum {
    INICIALIZA,
    SUSPENDE,
    FINALIZA,
}OP_MEMORIA;

typedef enum {
    EXIT,
    IO,
    DESALOJO,
}OP_CPU;

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

// Conexiones
int crear_conexion(char *ip, char* puerto);
void iniciar_conexiones();


//log_config.c
t_log* iniciar_logger_kernel(void);
t_config* iniciar_config_kernel(void);
void terminar_programa(t_log* logger, t_config* config);
//funciones_pcb.c
void generar_PCB(int idPCB, t_proceso* proceso);
void estimador(pcb* unPCB, double alfa, int rafaga_ejecutada);
bool ordenarSRT(pcb* unPCB,pcb* otroPCB);
int devolverID_CONSOLA(pcb* unPCB);
int devolverID_PCB(int socket);
//Planificadores
void planificador_LargoPlazo();
void planificador_CortoPlazo();
void planificador_MedianoPlazo();
void estadoReady();
void estadoExit();
void administrar_bloqueos();
void administradorRespuestaCPU();
void ejecucionProcesoSRT();
//Algoritmos
void algoritmo_FIFO();
void algoritmo_SRT();
void avisar_a_planificador_LP(pcb* pcbFinalizado);

// Utils
void* asignarMemoria(int cantidad);
int recibirMensaje(int socketEmisor, void* buffer, int bytesMaximos);
void enviarMensaje(int socket, void* mensaje, int tamanio);

void avisar_a_consola(pcb* pcbFinalizado);
void avisar_a_memoria(int socket, uint32_t estado, pcb* unProceso, t_log* logger);
void avisar_a_cpu_interrupt();

void serilizar_enviar_pcb(int socket, pcb* unPCB , t_log* logger);
void enviarStringSerializado(char* mensaje, int socket);
void enviarIntSerializado(int numero, int socket_memoria);
void enviar_respuesta_kernel(int socket, pcb* unPCB, int rafagaCPU , char* motivoRetorno, int tiempoBloqueo, t_log* logger);

void concatenarInstruccion(void* buffer, int* desplazamiento, t_instruccion* unaInstruccion);
void concatenarInt(void* buffer, int* desplazamiento, int numero);
void concatenarInt32(void* buffer, int* desplazamiento, uint32_t numero);
void concatenarDouble(void* buffer, int* desplazamiento, double numero);
void concatenarString(void* buffer, int* desplazamiento, char* mensaje);
void concatenarListaInt(void* buffer, int* desplazamiento, t_list* listaArchivos);

char* deserializarString(int emisor);
int deserializarInt(int emisor);
char deserializarChar(int emisor);
double deserializarDouble(int emisor);
t_list* deserializarListaInt(int emisor);
t_list* deserializarListaInstrucciones(int emisor);
t_list* deserializarListaInst(int emisor);
t_instruccion* deserializarInst(int emisor);
t_list* recibir_devolucion_cpu(int socket);
int tamanio_listaInst(t_list* listaInst);

// conexionConsola
#define PUERTO "8080"
#define IP "127.0.0.1"

void inicializar_direccion_kernel(struct sockaddr_in *direccion_kernel);
void bindear_kernel(int kernel, struct sockaddr_in direccion_kernel);
void escuchar(int kernel);
void procesar_entradas_de_consolas(int kernel);
void atender_consola(int consola);
bool protocolo_handshake(int consola);
bool es_igual_a(char* un_string, char* otro_string);
bool enviar_mensaje(op_code codigo, char* mensaje, int socket_consola);
bool se_pudo_recibir_el_proceso(t_proceso* proceso,int consola);
void recibir_instrucciones(t_proceso* proceso,int consola);
bool se_pueden_recibir_instrucciones(t_proceso* proceso, int consola);
void recibir_parametros(t_instruccion* instruccion, int consola);
int cantidad_de_parametros(char* identificador);
void enviar_confirmacion(int consola);
bool enviar_paquete(t_paquete* paquete, int socket_consola);
void eliminar_paquete(t_paquete* paquete);


