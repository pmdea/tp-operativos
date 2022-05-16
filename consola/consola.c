#include "consola.h"

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

void terminar(t_config* config, int conexion){

	log_destroy(logger_consola);
	config_destroy(config);
	close(conexion);
}

void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void* s = malloc(bytes);
	int desplazamiento = 0;

	memcpy(s + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(s + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(s + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return s;
}

void* serializar_cola(t_paquete* paquete, int bytes)
{
	void* s = malloc(bytes);
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

void enviar_cola(t_queue* cola, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = COLA;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = sizeof(cola);
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, cola, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_cola(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void agregar_a_paquete(t_paquete* paquete, t_queue* cola, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(tamanio));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), cola, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete* paquete, int socket_consola)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* paquete_serializado = serializar_paquete(paquete, bytes);

	send(socket_consola, paquete_serializado, bytes, 0);

	free(paquete_serializado);
}

void paquete(t_queue* cola, int conexion)
{
	t_paquete* paquete = crear_paquete();

	agregar_a_paquete(paquete, cola, sizeof(cola));

	enviar_paquete(paquete, conexion);

	eliminar_paquete(paquete);
}

int crear_conexion(char* ip, char* puerto)
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

int crear_conexion_con_kernel(t_config* config){

	char* ip = config_get_string_value(config,"IP_KERNEL");
	char* puerto = config_get_string_value(config,"PUERTO_KERNEL");
	log_info(logger_consola, "Leí archivo");

	log_info(logger_consola, "IP: %s PUERTO: %s", ip, puerto);

	int conexion = crear_conexion(ip, puerto);
	log_info(logger_consola, "Conexion creada");

	enviar_mensaje("Handshake", conexion);

	return conexion;
}

int main(void)
{
	logger_consola = log_create("consola.log", "CONSOLA", 1, LOG_LEVEL_INFO);
	t_config* config = config_create("consola.config");

	t_list* instruccion = list_create();
	list_add(instruccion, identificador);
	list_add(instruccion, paramA);
	list_add(instruccion, paramB);

	t_queue* lista_instrucciones = queue_create();

	queue_push(lista_instrucciones, instruccion);

	int conexion = crear_conexion_con_kernel(config);
	log_info(logger_consola,"Conexion creada con el Kernel");

	enviar_cola(lista_instrucciones, conexion);
	log_info(logger_consola,"Paquete enviado");

	terminar(config, conexion);
	return EXIT_SUCCESS;
}

