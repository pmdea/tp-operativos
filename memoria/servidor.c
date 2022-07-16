/*
 * servidor.c
 *
 *      Author: pmdea
 */


#include "includeh/servidor.h"
extern t_mem_config* config;
extern t_log* logger;
static int socket_mem = 0;

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(ip, puerto, &hints, &server_info);
	int socket_cliente = socket(server_info->ai_family,
            server_info->ai_socktype,
            server_info->ai_protocol);
	connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);
	bind(socket_mem, server_info->ai_addr, server_info->ai_addrlen);
	listen(socket_mem, SOMAXCONN);
	freeaddrinfo(server_info);
	return socket_cliente;
}


uint8_t ini_servidor(){
 //TODO: generar parámetros de inicio del servidor
	char* ip = "127.0.0.1";
	log_info(logger, "Inicializando servidor con ip %s y puerto %d... ", ip, config->puerto_escucha);
	socket_mem = crear_conexion(ip, string_itoa(config->puerto_escucha));
	log_info(logger, "Socket creado y escuchando: %d!", socket);
	free(ip);
	return 1;
}

int escuchar_server(){
	int cliente = esperar_cliente();
	if(cliente != -1){
		pthread_t thread;
		//TODO: distinguir kernel de CPU -> hablar con el grupo para ver el protocolo

	}
}

int esperar_cliente(){

}

void* escuchar_kernel(void* arg){
	//TODO: generar métodos para escuchar_kernel
	return EXIT_SUCCESS;
}
void* escuchar_cpu(void* arg){
	//TODO: generar funcionalidad escuchar cpu
	return EXIT_SUCCESS;
}

void finalizar_servidor(){
	//TODO: limpiar memoria
	close(socket);
}
