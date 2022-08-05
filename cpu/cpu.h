#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <math.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/string.h>
#include <commons/config.h>
#include "pthread.h"

#define IP_CPU "127.0.0.1"

//VAR GLOBAL
int k;
int rafagaEjecutada;
int interrupcionKernel;
int check;
t_list* tlb;

//SOCKETS
int server_cpu;
int socket_memoria;

// Logs y Config
t_log* loggerCpu;
t_config* configCpu;

pthread_mutex_t variableCompartida;

// ENUMS
typedef enum{
	NO_OP,
	IO,
	READ,
	WRITE,
	COPY,
	EXIT,
} ID_INSTRUCCION;

typedef enum{
	CPU = 0,
} id_mod;

typedef enum{
	EXIT_PCB,
	IO_PCB,
	DESALOJO_PCB,
} ID_CPU;

// ESTRUCTURAS
typedef struct{
	uint32_t id;
	uint32_t tamanio;
	t_list* instrucciones;
	uint32_t program_counter;
	uint32_t tabla_paginas;
	double estimacion_rafaga;
}PCB;

typedef struct {
	ID_INSTRUCCION identificador;
	t_queue* parametros;
}t_instruccion;

typedef struct {
	int tamanio_pagina;
	int cantidad_entradas;
}t_config_tabla;

typedef struct {
	int pagina;
	int marco;
}t_entrada_tlb;

typedef struct {
	int entrada_tabla_1er_nivel;
	int entrada_tabla_2do_nivel;
	int desplazamiento;
}t_direccion_logica;

typedef struct {
	uint32_t marco;
	uint32_t id_2do_nivel;
	uint32_t desplazamiento;
	uint32_t direccion;
}t_direccion_fisica;

typedef struct {
    int entradas_tlb;
    char* reemplazo_tlb;
    int retardo_noop;
    char* ip_memoria;
    char* puerto_memoria;
    char* puerto_cpu_dispatch;
    char* puerto_cpu_interrupt;
} CPU_CONFIG;
CPU_CONFIG config_cpu;

//MAIN.C
void dispatch(int escuchaDispatch);
void interrupt(int escuchaInterrupt);

//CICLO_DE_INSTRUCCIONES.C
t_instruccion* fetch(PCB* unPcb);
void decode(t_instruccion* instruccion, PCB* unPCB);
void execute(t_instruccion* instruccion, PCB* proceso, int socketA);
void checkInterrupt(PCB* proceso, int socketA);
int fetchOperands(t_direccion_logica* direccion_logica, PCB unPcb, t_config_tabla config);
void pcb_destroyer(PCB* pcb);
//ENVIO_RECIBO_KERNEL.C
void enviarRespuestaKernel(int socket_receptor, PCB unPCB, uint32_t motivoRegreso, uint32_t rafagaEjecutada, uint32_t tiempoBloqueo, t_log* logger);
PCB* deserializarPCB(int socket_emisor);
t_list* deserializarListaInstruccionesK(int emisor);
uint32_t tamanioParametros(t_list* lista);
int instruccion_a_realizar(ID_INSTRUCCION identificador);
int cantidad_de_parametros(ID_INSTRUCCION identificador);

//OPERACIONES_MEMORIA.C
t_direccion_fisica mmu(t_direccion_logica* direccion_logica, PCB proceso, t_config_tabla config);
int leer(t_direccion_fisica direccion_fisica, uint32_t entrada_2do_nivel);
void escribir(int valor, t_direccion_fisica direccion_fisica, uint32_t entrada_2do_nivel);
t_config_tabla obtener_direccion_logica(int direccion, t_direccion_logica* direccion_logica);
t_config_tabla* obtener_tamanioPag_Entradas();
uint32_t obtener_tabla_2do_nivel(int tabla_paginas_1er_nivel, int entrada_pagina_1er_nivel);
uint32_t obtener_marco(uint32_t tabla_1er_nivel, uint32_t tabla_2do_nivel, uint32_t entrada_2do_nivel);
int comparar_elementos_tlb(t_entrada_tlb* elem, int pag);
int esta_en_tlb(int pag);
int tlb_cache(int pag);
void reemplazo_tlb(t_entrada_tlb* entrada);
void agregar_a_TLB(int pagina, int marco);

//SERIALIZACIONESC.C
void* asignarMemoria(int cantidad);
int recibirMensaje(int socketEmisor, void* buffer, int bytesMaximos);
void enviarMensaje(int socket, void* mensaje, int tamanio);

void concatenarInt32(void* buffer, int* desplazamiento, uint32_t numero);
void concatenarDouble(void* buffer, int* desplazamiento, double numero);
void concatenarString(void* buffer, int* desplazamiento, char* mensaje);
uint32_t deserializarInt32(int emisor);
double deserializarDouble(int emisor);
char* deserializarString(int emisor);

//UTILSCPU.C
int iniciar_servidor_dispatch(void);
int iniciar_servidor_interrupt(void);
int esperar_cliente(int socket_servidor);
void conectar_a_memoria(int socket);
int crear_conexion(char *ip, char* puerto);

// LOG_CONFIGCPU.C
t_log* iniciar_logger_cpu(void);
t_config* iniciar_config_cpu(void);
