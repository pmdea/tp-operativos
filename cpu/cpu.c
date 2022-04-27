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

int await_call(void){

}

void kill_cpu(t_config* config){

	close(cpu_socket);
}

void connect_to_service(t_config* config, char* service){
	int* socket;
	char* ip;
	char* port;

	log_info(logger, "Connecting to %s service...", service);

	if(!strcmp(service, SRV_KERNEL_DISPATCH)){
		ip = config_get_string_value(config, IP_KERNEL);
		port = config_get_string_value(config, PUERTO_ESCUCHA_DISPATCH);
		socket = &kernel_disp_socket;
	}
	else if(!strcmp(service, SRV_KERNEL_INTERRUPT)){
		ip = config_get_string_value(config, IP_KERNEL);
		port = config_get_string_value(config, PUERTO_ESCUCHA_INTERRUPT);
		socket = &kernel_int_socket;
	}
	else if(!strcmp(service, SRV_MEMORY)){
		ip = config_get_string_value(config, IP_KERNEL);
		port = config_get_string_value(config, PUERTO_ESCUCHA_INTERRUPT);
		socket = &mem_socket;
	}
	*socket = crear_conexion(ip, port);
	log_info(logger, "Connected to %s service successfully!", service);
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

	connect_to_service(config, SRV_KERNEL_DISPATCH);
	connect_to_service(config, SRV_KERNEL_INTERRUPT);
	connect_to_service(config, SRV_MEMORY);
	iniciar_servidor();
	log_info(logger, "CPU initialized correctly");
}

int main(void)
{
	logger = init_logger();
	t_config* config = load_config();
	init_cpu(config);
	return 0;
}
