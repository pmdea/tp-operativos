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

void iniciar_settings(){
	iniciar_conexiones();
	iniciar_listas();
	iniciar_semaforos();
	log_info(loggerKernel, "Conexiones, Listas, Semaforos: INICIADOS");
}

void iniciar_conexiones(){
	socket_memoria = crear_conexion(config_kernel.ip_memoria, config_kernel.puerto_memoria);
	conectar_a_memoria(socket_memoria);
	socket_dispatch = crear_conexion(config_kernel.ip_cpu, config_kernel.puerto_cpu_dispatch);
	socket_interrupt = crear_conexion(config_kernel.ip_cpu, config_kernel.puerto_cpu_interrupt);
	log_info(loggerKernel, "CONEXIONES INICIADAS");
}

void conectar_a_memoria(int socket){
	void* buffer = asignarMemoria(sizeof(id_mod));
	int desplazamiento = 0;
	concatenarInt32(buffer, &desplazamiento, KERNEL);
	enviarMensaje(socket_memoria, buffer, sizeof(id_mod));
	free(buffer);
}

void iniciar_listas(){
	procesosNew = list_create();
	procesosReady = list_create();
	procesosExecute = list_create();
	procesosBlocked = list_create();
	procesosSuspendedReady = list_create();
	procesosExit = list_create();
	conexiones_pcb = list_create();
}

void iniciar_semaforos(){
	pthread_mutex_init(&mutexNew, NULL);
	pthread_mutex_init(&mutexReady, NULL);
	pthread_mutex_init(&mutexExit, NULL);
	pthread_mutex_init(&mutexBloqueo, NULL);
	pthread_mutex_init(&mutexBloqueoSuspendido, NULL);
	pthread_mutex_init(&mutexSuspendido, NULL);
	pthread_mutex_init(&variableEjecutando, NULL);
	sem_init(&grado_multiprogramacion, 0, config_kernel.grado_multiprogramacion);
    sem_init(&hayProcesoAnalizar, 0, 0);
	sem_init(&prioridad_SuspendedReady, 0, 0);
    sem_init(&enviarInterrupcion, 0, 0);
    sem_init(&finalizoProceso, 0, 0);
    sem_init(&nuevoProcesoReady, 0, 0);
    sem_init(&bloqueoMax, 0, 0);
    sem_init(&procesoBloqueado, 0, 0);
}

void iniciar_planificadores(){
	pthread_create(&gestionBloqueoHilo, NULL, (void*) gestionBloqueo_Suspension, NULL);
    pthread_create(&estadoExitHilo, NULL, gestionExit, NULL);
    pthread_create(&estadoReadyHilo, NULL, gestionNewSuspended, NULL);


    if(string_contains("FIFO", config_kernel.algoritmo_planificacion)){
    	log_info(loggerKernel, "INICIANDO ALGORITMO FIFO");
    	pthread_create(&algoritmoHilo, NULL, algoritmo_FIFO, NULL);
    }else{
    	log_info(loggerKernel, "INICIANDO ALGORITMO SRT");
    	pthread_create(&algoritmoHilo, NULL, algoritmo_SRT, NULL);
    	pthread_create(&desalojoSRTHilo, NULL, administradorInterrupcionCPU, NULL);
    }

    if(string_contains("SRT", config_kernel.algoritmo_planificacion)){
    	pthread_detach(desalojoSRTHilo);
    }

	pthread_detach(gestionBloqueoHilo);
	pthread_detach(algoritmoHilo);

    pthread_detach(estadoReadyHilo);
    pthread_detach(estadoExitHilo);
}

void avisar_a_consola(PCB* pcbFinalizado){
    int socket_consola = devolverID_CONSOLA(pcbFinalizado);
    void* buffer = asignarMemoria(sizeof(uint32_t));

    op_code mensaje = FINALIZACION_PROCESO;
    int desp = 0;

    concatenarInt32(buffer,&desp, mensaje);

    enviarMensaje(socket_consola, buffer, sizeof(uint32_t));
    free(buffer);
    log_debug(loggerKernel, "Ya le avise");
}
