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

#define IP_CPU "127.0.0.1"

//SOCKETS
int server_cpu;
int socket_memoria;

// Logs y Config
t_log* loggerCpu;
t_config* configCpu;

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

//ENVIO_RECIBO_KERNEL.C
void enviarPCB(int socket_receptor, PCB unPCB, t_log* logger);
PCB* deserializarPCB(int socket_emisor);
t_list* deserializarListaInstruccionesK(int emisor);
uint32_t tamanioParametros(t_list* lista);
int cantidad_de_parametros(ID_INSTRUCCION identificador);

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

// LOG_CONFIGCPU.C
t_log* iniciar_logger_cpu(void);
t_config* iniciar_config_cpu(void);
