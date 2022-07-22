#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

#define IP_CPU "127.0.0.1"

// Sockets para conservar referencia en todo el programa
int kernel_int_socket;
int kernel_disp_socket;
int socket_memoria;

// ServiceNames
#define SRV_KERNEL_DISPATCH "KERNEL_DISPATCH"
#define SRV_KERNEL_INTERRUPT "KERNEL_INTERRUPT"
#define SRV_MEMORY "SRV_MEMORY"

// Var globales
int* interrupcion;

pthread_mutex_t interrupcionVariable;

// DEFINO LOG Y CONFIG
t_log* loggerCpu;
t_config* configCpu;
t_list* tlb;

// DEFINO ESTRUCTURAS
typedef struct {
    int entradas_tlb;
    char* reemplazo_tlb;
    int retardo_noop;
    char* ip_memoria;
    int puerto_memoria;
    int puerto_cpu_dispatch;
    int puerto_cpu_interrupt;
} CPU_CONFIG;
CPU_CONFIG config_cpu;


typedef enum {
    EXIT,
    IO,
    DESALOJO,
}OP_CPU;

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
	int entrada_tabla_1er_nivel;
	int entrada_tabla_2do_nivel;
	int desplazamiento;
} direccion_logica;

typedef struct {
	uint32_t marco;
	uint32_t desplazamiento;
} direccion_fisica;

typedef struct {
	int pagina;
	int marco;
} entrada_tlb;

//Conexiones
void init_cpu();
int crear_conexion(char *ip, char* puerto);
void handshake_memoria(int socket);
int create_socket(char* port);
int esperar_cliente(int socket_servidor);
void *interruption(void *arg);
void *dispatch(void *arg);

// Log y Config
t_log* iniciar_logger_cpu(void);
t_config* iniciar_config_cpu(void);

// Ciclo de instrucciones
t_instruccion* fetch(pcb* unPcb);
void decode(t_instruccion* instruccion, pcb* unPCB);
int fetchOperands(direccion_logica* direccion_logica, pcb* unPcb);
void execute(t_instruccion* instruccion, pcb* proceso, int raf);
void checkInterrupt(int rafaga, pcb* proceso, int cortarEjecucion);

int TLB(int pag);
int estaEnTLB(int pag);
direccion_fisica* MMU(direccion_logica* direccion_logica, pcb* proceso);

// Operaciones memoria
int leer(direccion_fisica* direccion_fisica);
void escribir(int valor, direccion_fisica* direccion_fisica);

void obtener_direccion_logica(int direccion, direccion_logica* direccion_logica);
void obtener_tamanioPag_Entradas(int tamanio_pagina, int cant_entradas_por_tabla);
uint32_t obtener_marco(uint32_t tabla_1er_nivel, uint32_t tabla_2do_nivel, uint32_t entrada_2do_nivel);
int asignarNumero(char* ident);

int comparar_elementos_tlb(entrada_tlb* elem, int pag);
int esta_en_tlb(int pag);
int tlb_cache(int pag);
void reemplazo_tlb(entrada_tlb* entrada);
void agregar_a_TLB(int pagina, int marco);
direccion_fisica* mmu(direccion_logica* direccion_logica, pcb* proceso);
// Utils
void* asignarMemoria(int cantidad);
int recibirMensaje(int socketEmisor, void* buffer, int bytesMaximos);
void enviarMensaje(int socket, void* mensaje, int tamanio);

void enviarStringSerializado(char* mensaje, int socket);
void enviarIntSerializado(int numero, int socket_memoria);
void enviar_respuesta_kernel(int socket, pcb* unPCB, int rafagaCPU , int motivoRetorno, int tiempoBloqueo, t_log* logger);

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
int tamanio_listaInst(t_list* listaInst);
