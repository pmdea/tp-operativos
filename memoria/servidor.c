#include "includeh/servidor.h"
extern t_mem_config* config;
extern t_log* logger;
static int socket_mem = 0;

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(ip, puerto, &hints, &servinfo);
	int server = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

	bind(server, servinfo->ai_addr, servinfo->ai_addrlen);

	listen(server, SOMAXCONN);

	freeaddrinfo(servinfo);
	return server;
}


uint8_t ini_servidor(){
	log_info(logger, "Inicializando servidor con ip %s y puerto %d... ", config->ip, config->puerto_escucha);
	char* puerto_string = string_itoa(config->puerto_escucha);
	socket_mem = crear_conexion(config->ip, puerto_string);
	log_info(logger, "Socket creado y escuchando!");
	free(puerto_string);
	return 1;
}

int escuchar_server(){
	int cliente = esperar_cliente();
	if(cliente == -1){
		return 0;
	}
	pthread_t thread;
	id_mod modulo; // codigo para distinguir si es cpu o kernel
	if(recv(cliente, &modulo, sizeof(modulo), 0) != sizeof(modulo)){
		return 0;
	}
	args_thread* args = malloc(sizeof(args_thread));
	args->socket_cliente = cliente;
	switch(modulo){
		case KERNEL:
			log_info(logger, "Recibido mensaje de KERNEL, create y detach thread correspondiente");
			pthread_create(&thread, NULL, (void*) escuchar_kernel, (void*) args);
			pthread_detach(thread);
			return 1;
		break;
		case CPU:
			log_info(logger, "Recibido mensaje de CPU, create y detach thread correspondiente");
			pthread_create(&thread, NULL, (void*) escuchar_cpu, (void*) args);
			pthread_detach(thread);
			return 1;
		break;
	}
	return 1;
}

int esperar_cliente(){
	int socket_cliente = accept(socket_mem, NULL, NULL);
	if(socket_cliente == -1){
		log_error(logger, "Ha ocurrido un error con la conexión");
	}
	return socket_cliente;
}

void* escuchar_kernel(void* arg){
	//TODO: generar métodos para escuchar_kernel
	log_info(logger, "Escuchando mensaje de KERNEL");
	args_thread* args = (args_thread*) arg;
	int cliente = args->socket_cliente;
	free(args);
	while (cliente != -1) {
		message_kernel* request = parsear_message_kernel(cliente);
		uint32_t response = 1;
		switch(request->estado){
			case 0: //Inicializar proceso
				log_info(logger, "Inicializando proceso con pcb id %d y size %d...", request->id_pcb, request->tamanio_pcb);
				uint32_t id_tabla = iniciar_proc(request->tamanio_pcb, request->id_pcb);
				uint32_t ok = 1;
				enviar_mensaje_cliente(cliente, &id_tabla, sizeof(id_tabla));
			break;
			case 1 : //SUSP_PROC
				log_info(logger, "Suspendiendo proceso con pcb id %d...", request->id_pcb);
				suspender_proc(request->id_pcb);
				enviar_mensaje_cliente(cliente, &ok, sizeof(ok));
			break;
			case 2: //EXIT
				log_info(logger, "Finalizando proceso con pcb id %d...", request->id_pcb);
				finalizar_proc(request->id_pcb);
				enviar_mensaje_cliente(cliente, &ok, sizeof(ok));
			break;

		}
	}
	log_warning(logger, "El cliente se desconecto de memoria");
	return EXIT_SUCCESS;
}
void* escuchar_cpu(void* arg){
	log_info(logger, "Escuchando mensaje de CPU");
	args_thread* args = (args_thread*) arg;
	int cliente = args->socket_cliente;
	free(args);
	while (cliente != -1) {
		message_cpu* request = parsear_message_cpu(cliente);
		switch(request->operacion){
			case HANDSHAKE:{
				log_info(logger, "Realizando handshake con CPU");
				uint32_t size_pag = config->tam_pag;
				uint32_t nro_entradas = config->entradas_por_tabla;
				log_info(logger, "Modulo de memoria configurado con paginas de size %d y entradas x tabla %d", size_pag, nro_entradas);
				int size = sizeof(size_pag) + sizeof(nro_entradas);
				void* buffer = malloc(size);
				memcpy(buffer, &size_pag, sizeof(size_pag));
				memcpy(buffer+ sizeof(nro_entradas), &nro_entradas, sizeof(nro_entradas));
				enviar_mensaje_cliente(cliente, buffer, size);
				free(buffer);
				free(request);
			}
			break;
			case READ:{
				log_info(logger, "Recibido request de READ");
				uint32_t valor = leer_en_memoria(request->datos[0], request->datos[1], request->datos[2]);
				enviar_mensaje_cliente(cliente, &valor, sizeof(uint32_t));
				free(request);
			}
			break;
			case WRITE:{
				log_info(logger, "Recibido request de WRITE");
				char* response = escribir_memoria(request->datos[0], request->datos[1], request->datos[2], &(request->datos[3]));
				enviar_mensaje_cliente(cliente, response, sizeof(uint32_t));
				free(request);
			}
			break;
			case COPY:
				log_info(logger, "Recibido request de COPY");
				log_error(logger, "No se deberia llamar. Hacer copy haciendo read y write");
				free(request);
			break;
			case GET_PAG_NVL_2:{
				log_info(logger, "Recibido request de GET_PAG_NVL_2");
				uint32_t id_tabla_2 = get_tabla_2do_lvl(request->datos[0], request->datos[1]);
				enviar_mensaje_cliente(cliente, &id_tabla_2, sizeof(uint32_t));
				free(request);
			}
			break;
			case GET_MARCO:{
				log_info(logger, "Recibido request de GET_MARCO");
				uint32_t valor_en_mem = get_nro_marco(request->datos[0], request->datos[1], request->datos[2]);
				enviar_mensaje_cliente(cliente, &valor_en_mem, sizeof(uint32_t));
				free(request);
			}
			break;
		}

	}
	log_warning(logger, "El cliente se desconecto de memoria");
	return EXIT_SUCCESS;
}

int enviar_mensaje_cliente(int cliente, void* data, int size){
	return send(cliente, data, size, 0);
}

message_kernel* parsear_message_kernel(int cliente){
	message_kernel* request = malloc(sizeof(message_kernel));
    void* stream = malloc(100);
    recv(cliente, stream, 100, 0);
    //Copio estado del stream al struct
    memcpy(&(request->estado), stream, sizeof(uint32_t));
    //Copio estado del stream al struct
    memcpy(&(request->tamanio_pcb), stream + sizeof(uint32_t), sizeof(uint32_t));
    memcpy(&(request->id_pcb), stream + sizeof(uint32_t) + sizeof(uint32_t), sizeof(uint32_t));
    free(stream);
    return request;
}

message_cpu* parsear_message_cpu(int cliente){
	message_cpu* request = malloc(sizeof(message_cpu));
	int size = sizeof(uint32_t);
    recv(cliente, &(request->operacion), size, 0);
    recv(cliente, &(request->size_data), size, 0);
    int cantidad_datos = request->size_data/size;
    for(int i = 0; i < cantidad_datos; i++){
    	uint32_t dato;
    	recv(cliente, &dato, size, 0);
    	request->datos[i] = dato;
    }
	return request;
}

void finalizar_servidor(){
	close(socket_mem);
}
