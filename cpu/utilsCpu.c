#include "cpu.h"

int iniciar_servidor_dispatch(void)
{
	int socket_servidor;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(config_cpu.ip, config_cpu.puerto_cpu_dispatch, &hints, &servinfo);

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	int enable = 1;
	setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int));

	// Asociamos el socket a un puerto
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

	// Escuchamos las conexiones entrantes
	listen(socket_servidor, SOMAXCONN);

	freeaddrinfo(servinfo);
	log_info(loggerCpu, "Listo para escuchar a KERNEL - Dispatch");

	return socket_servidor;
}

int iniciar_servidor_interrupt(void)
{
	int socket_servidor;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(IP_CPU, config_cpu.puerto_cpu_interrupt, &hints, &servinfo);

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	int enable = 1;
	setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int));

	// Asociamos el socket a un puerto
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

	// Escuchamos las conexiones entrantes
	listen(socket_servidor, SOMAXCONN);

	freeaddrinfo(servinfo);
	log_info(loggerCpu, "Listo para escuchar a KERNEL - Interrupt");

	return socket_servidor;
}

void conectar_a_memoria(int socket){
	void* buffer = asignarMemoria(sizeof(id_mod));
	int desplazamiento = 0;
	concatenarInt32(buffer, &desplazamiento, CPU);
	enviarMensaje(socket, buffer, sizeof(id_mod));
	free(buffer);
}

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Ahora vamos a crear el socket.
	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype,server_info->ai_protocol);
	int enable = 1;
	setsockopt(socket_cliente, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	setsockopt(socket_cliente, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int));
	// Ahora que tenemos el socket, vamos a conectarlo
	connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);

	freeaddrinfo(server_info);

	return socket_cliente;
}

int esperar_cliente(int socket_servidor)
{
	// Aceptamos un nuevo cliente
	int socket_cliente = accept(socket_servidor, NULL, NULL);
	//log_info(loggerCpu, "Se conecto un cliente!");

	return socket_cliente;
}
