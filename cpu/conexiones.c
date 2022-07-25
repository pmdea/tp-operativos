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
/**
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
}*/

int create_socket(char* port)
{
	int socket_servidor;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(IP_CPU, port, &hints, &servinfo);

	// Creamos el socket de escucha del servidor
	socket_servidor = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

	// Asociamos el socket a un puerto
	bind(socket_servidor, servinfo->ai_addr, servinfo->ai_addrlen);

	// Escuchamos las conexiones entrantes
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
//	while(true){

//		int esperarValor = deserializarInt(socket_cliente);

//		pthread_mutex_lock(&interrupcionVariable);
//		interrupcion = esperarValor;
//		pthread_mutex_unlock(&interrupcionVariable);

//	}

	log_info(loggerCpu, "Interrupt thread finish!");
	return EXIT_SUCCESS;
}

void *dispatch(void *arg){
	PCB* unPCB;
	log_info(loggerCpu, "Dispatch listening for calls...");
	int socket_cliente = esperar_cliente(kernel_disp_socket);
	log_info(loggerCpu, "Kernel Dispatch connected %d", socket_cliente);
	while(true){
		log_info(loggerCpu, "Esperando PCB.....");
		unPCB = deserializarPCB(socket_cliente);


		int* rafaga = malloc(sizeof(int));
		rafaga = 0;
		int tamanio = list_size(unPCB->instrucciones);

//		mostrarDatosPCB(*unPCB, loggerCpu);

//		enviarRespuestaKernel(socket_cliente, *unPCB, IO, 10, 15, loggerCpu);

		for(j = 0; j < tamanio; j++){
		// obtiene la instruccion del pcb
		log_warning(loggerCpu, "PC %i", unPCB->program_counter);
		t_instruccion* unaInstruccion = fetch(*unPCB);

		// se fija si tiene que buscar operandos en memoria
		decode(unaInstruccion, *unPCB);

		//ejecuta la instruccion
		execute(unaInstruccion, unPCB, rafaga, socket_cliente);

		checkInterrupt(rafaga, *unPCB, socket_cliente);

		}
		free(rafaga);
	}

	log_info(loggerCpu, "Dispatch thread finish!");
	return EXIT_SUCCESS;
}
