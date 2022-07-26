#include "kernel.h"

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

	freeaddrinfo(server_info);
	return socket_cliente;
}

void iniciar_conexiones(){
	//socket_memoria = crear_conexion(config_kernel.ip_memoria, string_itoa(config_kernel.puerto_memoria));
	socket_cpu_dispatch = crear_conexion(config_kernel.ip_cpu, string_itoa(config_kernel.puerto_cpu_dispatch));
	socket_cpu_interrupt = crear_conexion(config_kernel.ip_cpu, string_itoa(config_kernel.puerto_cpu_interrupt));
}
