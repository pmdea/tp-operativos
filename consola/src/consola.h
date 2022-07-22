#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/txt.h>
#include <commons/string.h>
#include <commons/collections/queue.h>
#include <commons/config.h>
#include <commons/error.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

typedef struct {
	int tamanio_id;
	char* identificador;
	t_queue* parametros;
} t_instruccion;

typedef enum {
	HANDSHAKE,
	ENVIO_DATOS,
	MENSAJE,
	CONFIRMACION
} op_code;

typedef struct {
	int size;
	void* stream;
} __attribute__((packed))
t_buffer;

typedef struct {
	op_code operacion;
	t_buffer* buffer;
}  __attribute__((packed))
t_paquete;

typedef struct {
	op_code operacion;
	int tamanio_mensaje;
	char* mensaje;
} __attribute__((packed))
t_mensaje;

typedef struct {
	t_queue* instrucciones;
	int tamanio_proceso;
}t_proceso;

bool el_proceso_es_valido(FILE* instrucciones, t_queue* instrucciones_parseadas);
char* quitar_eol(char* identificador);
bool es_una_instruccion_valida(char** instruccion);
void inicializar_identificadores_validos(t_list* identificadores_validos);
bool esta_en_la_lista(t_list* lista, void* elemento);
bool tiene_los_parametros_correctos(char** instruccion);
bool es_igual_a(void* un_string, void* otro_string);
int cantidad_de_parametros(char* identificador);
void agregar_instruccion(t_queue* instrucciones, char** instruccion);
void agregar_parametros(t_instruccion* instruccion_validada, char** instruccion);
int crear_conexion_con_kernel(t_config* config);
int crear_conexion(char* ip, char* puerto);
bool se_pudo_hacer_el_handshake(int socket_consola);
bool confirmacion_handshake(int socket_consola);
void deserializar_handshake(t_mensaje* recibido,int socket_consola);
bool se_recibio_el_mensaje_correcto(t_mensaje* recibido, int socket_consola);
bool enviar_mensaje(op_code codigo, char* mensaje, int socket_consola);
bool enviar_paquete(t_paquete* paquete, int socket_consola);
void eliminar_paquete(t_paquete* paquete);
t_paquete* serializar_proceso(t_queue* contenido, op_code codigo,
		int tamanio_proceso);
int tamanio_identificadores(t_queue* instrucciones);
int tamanio_identificador(t_instruccion* instruccion);
int sumar(int un_numero, int otro_numero);
int parametros_totales(t_queue* contenido);
int parametros(t_instruccion* instruccion);
void agregar_contenido(t_queue* contenido, void* buffer, int tamanio);
void copiar_parametros(void* buffer, int *desplazamiento, t_queue* parametros);


bool fue_confirmado_el_envio(int socket_consola);

void destruir_instruccion(t_instruccion* instruccion);

char* show_parametros(char** instruccion);
void show_contenido(t_queue* contenido);
void show_paquete(t_paquete* paquete);
void deserializar_datos(t_proceso* proceso, t_paquete* stream);
void show_proceso(t_proceso* proceso);
void show_instrucciones(t_queue* instrucciones);
void show_parametros_leidos(t_list* parametros);
void deserializar_mensaje(char* mensaje,t_paquete* paquete);

#define CLAVE_IP "IP_KERNEL"
#define CLAVE_PUERTO "PUERTO_KERNEL"
#define UNSIGNED_INT uint32_t
#define CONFIRM_ESPERADA "RECIBIDO :)"
#define MENSAJE_HANDSHAKE_ENVIADO "HOLA DON PEPITO"
#define MENSAJE_HANDSHAKE_ESPERADO "HOLA DON JOSE"

typedef enum {
	NO_OP = 0,
	IO = 1,
	READ = 2,
	WRITE = 3,
	COPY = 4,
	EXIT = 5
} OPERACIONES;

t_log* log_consola;

#endif /* CONSOLA_H_ */

