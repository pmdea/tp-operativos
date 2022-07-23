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
int j;
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

typedef struct{
	uint32_t id;
	uint32_t tamanio;
	t_list* instrucciones;
	uint32_t program_counter;
	uint32_t tabla_paginas;
	double estimacion_rafaga;
}PCB;

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
t_instruccion* fetch(PCB unPcb);
void decode(t_instruccion* instruccion, PCB unPCB);
int fetchOperands(direccion_logica* direccion_logica, PCB unPcb);
void execute(t_instruccion* instruccion, PCB proceso, int raf, int socketA);
void checkInterrupt(int rafaga, PCB proceso, int cortarEjecucion, int socketA);
int TLB(int pag);
int estaEnTLB(int pag);

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
direccion_fisica* mmu(direccion_logica* direccion_logica, PCB* proceso);
// Utils
void enviarRespuestaKernel(int socket_receptor, PCB unPCB, uint32_t motivoRegreso, uint32_t rafagaEjecutada, uint32_t tiempoBloqueo, t_log* logger);
void mostrarDatosPCB(PCB unPCB, t_log* log);
void* asignarMemoria(int cantidad);
int recibirMensaje(int socketEmisor, void* buffer, int bytesMaximos);
void enviarMensaje(int socket, void* mensaje, int tamanio);

void concatenarInt32(void* buffer, int* desplazamiento, uint32_t numero);
void concatenarDouble(void* buffer, int* desplazamiento, double numero);
void concatenarString(void* buffer, int* desplazamiento, char* mensaje);
void concatenarListaInt32(void* buffer, int* desplazamiento, t_list* listaArchivos);
uint32_t tamanio_listaInst(t_list* listaInst);
PCB* deserializarPCB(int socket_emisor, t_log* logger);
uint32_t deserializarInt32(int emisor);
t_list* deserializarListaInt32(int emisor);
double deserializarDouble(int emisor);
char* deserializarString(int emisor);
/*
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
*/
