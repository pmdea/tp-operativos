#include "cpu.h"

void init_cpu(){
	log_info(loggerCpu, "Initializing CPU process...");
	// Realizar handshake con memoria
	socket_memoria = crear_conexion(config_cpu.ip_memoria), config_cpu.puerto_memoria);
	log_info(loggerCpu, "Memory socket created! %d", socket_memoria);
	handshake_memoria(socket_memoria);
	//Escuchar en dispatch e interrupt
	kernel_int_socket = create_socket(config_cpu.puerto_cpu_interrupt);
	kernel_disp_socket = create_socket(config_cpu.puerto_cpu_dispatch);
	log_info(loggerCpu, "CPU initialized correctly");
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
	int socket_cliente = socket(server_info->ai_family,
            server_info->ai_socktype,
            server_info->ai_protocol);
	connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);

	freeaddrinfo(server_info);
	return socket_cliente;
}

void handshake_memoria(int socket){
	log_info(loggerCpu, "Realizing handshake with memory module...");
	log_info(loggerCpu, "Handshake completed successfully!");
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
	log_trace(loggerCpu, "Listening to connections...");
	return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{
	int socket_cliente = accept(socket_servidor, NULL, NULL);
	//log_info(loggerCpu, "Se conecto un cliente!");

	return socket_cliente;
}

void *interruption(void *arg){
	log_info(loggerCpu, "Interrupt listening for calls...");
	int socket_cliente = esperar_cliente(kernel_int_socket);
	log_info(loggerCpu, "Kernel Interrupt connected %d", socket_cliente);
	while(true){
		int esperarValor = deserializarInt(socket_cliente);

		pthread_mutex_lock(&interrupcionVariable);
		interrupcion = esperarValor;
		pthread_mutex_unlock(&interrupcionVariable);
	}

	log_info(loggerCpu, "Interrupt thread finish!");
	return EXIT_SUCCESS;
}

void *dispatch(void *arg){
	log_info(loggerCpu, "Dispatch listening for calls...");
	int socket_cliente = esperar_cliente(kernel_disp_socket);
	log_info(loggerCpu, "Kernel Dispatch connected %d", socket_cliente);
	while(true){

		pcb* proceso = deserializarPCB(socket_cliente);
		log_info(loggerCpu, "Ingreso un proceso");
		int* rafaga = 0;
		int* i = 0;
		int tamanio = list_size(proceso->instrucciones);

		while(i < tamanio){

		// obtiene la instruccion del pcb
		t_instruccion* instruccion = fetch(proceso);

		// se fija si tiene que buscar operandos en memoria
		decode(instruccion, proceso);

		//ejecuta la instruccion
		execute(instruccion, proceso, rafaga);

		checkInterrupt(rafaga, proceso);

		i++;

		}

	}

	log_info(loggerCpu, "Dispatch thread finish!");
	return EXIT_SUCCESS;
}

