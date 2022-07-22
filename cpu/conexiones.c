#include "cpu.h"

void init_cpu(){
	log_info(loggerCpu, "Initializing CPU process...");
	// Realizar handshake con memoria
	//socket_memoria = crear_conexion(config_cpu.ip_memoria, string_itoa(config_cpu.puerto_memoria));
	//log_info(loggerCpu, "Memory socket created! %d", socket_memoria);
	//handshake_memoria(socket_memoria);
	//Escuchar en dispatch e interrupt
	kernel_int_socket = create_socket(string_itoa(config_cpu.puerto_cpu_interrupt));
	kernel_disp_socket = create_socket(string_itoa(config_cpu.puerto_cpu_dispatch));
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
	//TODO: handshake
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
		log_info(loggerCpu, "ESPERANDO PROCESO");
		//pcb* proceso = malloc(sizeof(pcb));
		//proceso = deserializarPCB(socket_cliente);

		pcb* proceso = asignarMemoria(sizeof(pcb));
		log_info(loggerCpu, "ESTOY EN DESERIALIZAR1");
		t_list* instrucciones = list_create();
		proceso -> id = deserializarInt(socket_cliente);
		proceso -> tamanio = deserializarInt(socket_cliente);
		proceso -> program_counter = deserializarInt(socket_cliente);
		proceso -> tabla_paginas = deserializarInt(socket_cliente);
		proceso -> estimacion_rafaga = deserializarDouble(socket_cliente);
		//proceso -> instrucciones = list_create();
		//instrucciones = deserializarListaInst(socket_cliente);
		//list_add_all(proceso -> instrucciones, instrucciones );

		log_info(loggerCpu, "66PCB ID %i a Kernel....", proceso -> id);
		log_info(loggerCpu, "66PCB EST %f de CPU ", proceso -> estimacion_rafaga);
		log_info(loggerCpu, "66PCB TAB %i de CPU ", proceso -> tabla_paginas);

		int* rafaga = 0;
		int tamanio = list_size(proceso->instrucciones);
		log_info(loggerCpu, " TAMANIO INST %i", tamanio);


		for(j = 0; j < tamanio; j++){

		// obtiene la instruccion del pcb
		t_instruccion* instruccion = fetch(proceso);
		log_info(loggerCpu, "INSTRUCCION %s", instruccion -> identificador);
		// se fija si tiene que buscar operandos en memoria
		decode(instruccion, proceso);

		//ejecuta la instruccion
		execute(instruccion, proceso, rafaga, socket_cliente);

		checkInterrupt(rafaga, proceso, j, socket_cliente);

		}
	}

	log_info(loggerCpu, "Dispatch thread finish!");
	return EXIT_SUCCESS;
}
