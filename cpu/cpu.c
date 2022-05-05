#include "cpu.h"

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

void handshake_memoria(){
	log_info(logger, "Realizing handshake with memory module...");
	log_info(logger, "Handshake completed successfully!");
}

void end_process(t_config* config){

	close(cpu_socket);
	close(mem_socket);
}

t_log* init_logger(void){
	return log_create(ARCHIVO_LOG, PROCESS_NAME, true, LOG_LEVEL_DEBUG);
}

t_config* load_config(void){
	return config_create(ARCHIVO_CONFIG);
}

void iniciar_servidor(void)
{
	int socket_servidor;
	struct addrinfo hints, *servinfo, *p;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(IP_CPU, PUERTO_CPU, &hints, &servinfo);
	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
	listen(socket_servidor, SOMAXCONN);
	freeaddrinfo(servinfo);
	log_trace(logger, "Listening to connections...");
	int* socket = &cpu_socket;
	*socket = socket_servidor;
}

void init_cpu(t_config* config){
	log_info(logger, "Initializing CPU process...");
	// Realizar handshake con memoria
	int* socketMemoria = &mem_socket;
	*socketMemoria= crear_conexion(config_get_string_value(config, IP_MEMORIA), config_get_string_value(config, PUERTO_MEMORIA));
	log_info(logger, "Memory socket created! %d", mem_socket);

	//Escuchar en dispatch e interrupt
	iniciar_servidor();
	log_info(logger, "CPU initialized correctly");
}

int main(void)
{
	logger = init_logger();
	t_config* config = load_config();


	return 0;
}
