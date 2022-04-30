#include "consola.h"
#include <commons/log.h>

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

void terminar(t_log* logger_consola, t_config* config, int conexion){

	log_destroy(logger_consola);
	config_destroy(config);
	close(conexion);
}

void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * s = malloc(bytes);
	int desplazamiento = 0;

	memcpy(s + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(s + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(s + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return s;
}

t_paquete* crear_paquete(void)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = PAQUETE;
	crear_buffer(paquete);
	return paquete;
}

void enviar_mensaje(char* mensaje, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void agregar_a_paquete(t_paquete* paquete, t_list* lista, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), lista, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void handshake(t_log* logger_consola, int socket_consola){

	uint32_t handshake = 1;
	uint32_t result;

	enviar_mensaje(handshake, socket_consola);
	recv(socket_consola, &result, sizeof(uint32_t), MSG_WAITALL);
	if(result!=0)
	{
		log_error(logger_consola,"ERROR en Handshake");
	}
	else log_info(logger_consola,"Handshake exitoso");
}

void enviar_paquete(t_paquete* paquete, int socket_consola)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* paquete_serializado = serializar_paquete(paquete, bytes);

	send(socket_consola, paquete_serializado, bytes, 0);

	free(paquete_serializado);
}

void paquete(t_list* lista, int conexion)
{
	t_paquete* paquete = crear_paquete();

	agregar_a_paquete(paquete, lista, sizeof(paquete));

	enviar_paquete(paquete, conexion);

	eliminar_paquete(paquete);
}

int crear_conexion(char *ip, int puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, (char*)puerto, &hints, &server_info);

	int socket_consola = socket(server_info->ai_family, server_info->ai_socktype,server_info->ai_protocol);

	connect(socket_consola, server_info->ai_addr, server_info->ai_addrlen);

	freeaddrinfo(server_info);

	return socket_consola;
}

int crear_conexion_con_kernel(t_log* logger_consola, t_config* config){

	char* ip = config_get_string_value(config,"IP_KERNEL");
	int puerto = config_get_int_value(config,"PUERTO");

	log_info(logger_consola, "IP: %s PUERTO: %s", ip, (int)puerto);

	int conexion = crear_conexion(ip, puerto);

	handshake(logger_consola, conexion);

	return conexion;
}

int main(void)
{
	t_log* logger_consola = log_create("consola.log", "CONSOLA", 1, LOG_LEVEL_INFO);
	t_config* config = config_create("consola.config");

	t_list* lista_instrucciones = list_create();
	char* elemento = "Hola";
	list_add(lista_instrucciones, elemento);

	int conexion = crear_conexion_con_kernel(logger_consola, config);
	log_info(logger_consola,"Conexion creada con el Kernel");

	paquete(lista_instrucciones, conexion);
	log_info(logger_consola,"Paquete enviado");

	terminar(logger_consola, config, conexion);
	return 0;
}

