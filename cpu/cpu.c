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

void handshake_memoria(int socket){
	log_info(logger, "Realizing handshake with memory module...");
	log_info(logger, "Handshake completed successfully!");
}

void end_process(t_config* config){
	config_destroy(config);
	log_destroy(logger);
	close(kernel_disp_socket);
	close(kernel_int_socket);
	close(mem_socket);
}

t_log* init_logger(void){
	return log_create(ARCHIVO_LOG, PROCESS_NAME, true, LOG_LEVEL_DEBUG);
}

t_config* load_config(void){
	return config_create(ARCHIVO_CONFIG);
}

int create_socket(char* port)
{
	int socket_servidor;
	struct addrinfo hints, *servinfo;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(IP_CPU, port, &hints, &servinfo);
	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);
	listen(socket_servidor, SOMAXCONN);
	freeaddrinfo(servinfo);
	log_trace(logger, "Listening to connections...");
	return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{
	int socket_cliente = accept(socket_servidor, NULL, NULL);
	log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

void *interruption(void *arg){
	log_info(logger, "Interrupt listening for calls...");
	int socket_cliente = esperar_cliente(kernel_int_socket);
	log_info(logger, "Client connected %d", socket_cliente);
	while(true){
		int cod = recibir_operacion(socket_cliente);
		log_info(logger, "Client connected %d", cod);
		//Todavia no se sabe que hacer asi que se deja así.
		if(cod < 0){
			return EXIT_FAILURE;
		}
	}

	log_info(logger, "Interrupt thread finish!");
	return EXIT_SUCCESS;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void *dispatch(void *arg){
	log_info(logger, "Dispatch listening for calls...");
	int socket_cliente = esperar_cliente(kernel_disp_socket);
	log_info(logger, "Client connected %d", socket_cliente);
	while(true){
		int cod = recibir_operacion(socket_cliente);
		log_info(logger, "Client connected %d", cod);
		//Todavia no se sabe que hacer asi que se deja así.
		if(cod < 0){
			return EXIT_FAILURE;
		}
	}

	log_info(logger, "Dispatch thread finish!");
	return EXIT_SUCCESS;
}

void init_cpu(t_config* config){
	log_info(logger, "Initializing CPU process...");
	// Realizar handshake con memoria
	mem_socket = crear_conexion(config_get_string_value(config, IP_MEMORIA), config_get_string_value(config, PUERTO_MEMORIA));
	log_info(logger, "Memory socket created! %d", mem_socket);
	handshake_memoria(mem_socket);
	//Escuchar en dispatch e interrupt
	kernel_int_socket = create_socket(config_get_string_value(config, PUERTO_ESCUCHA_INTERRUPT));
	kernel_disp_socket = create_socket(config_get_string_value(config, PUERTO_ESCUCHA_DISPATCH));
	log_info(logger, "CPU initialized correctly");
}

int main(void)
{
	logger = init_logger();
	t_config* config = load_config();
	init_cpu(config);

	pthread_t thread_dispath, thread_interrupt;
	log_info(logger, "Starting dispatch thread...");
	pthread_create(&thread_dispath, NULL, dispatch, NULL);
	log_info(logger, "Starting interrupt thread...");
	pthread_create(&thread_interrupt, NULL, interruption, NULL);

	pthread_join(thread_interrupt, NULL);
	pthread_join(thread_dispath, NULL);

	end_process(config);
	return EXIT_SUCCESS;
}
