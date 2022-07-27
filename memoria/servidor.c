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
	char* ip = "127.0.0.1";
	log_info(logger, "Inicializando servidor con ip %s y puerto %d... ", ip, config->puerto_escucha);
	char* puerto_string = string_itoa(config->puerto_escucha);
	socket_mem = crear_conexion(ip, puerto_string);
	log_info(logger, "Socket creado y escuchando: %d!", socket_mem);
	free(puerto_string);
	return 1;
}

int escuchar_server(){
	pthread_t t_cpu, t_kernel;
	for(int i = 0; i<2; i++){
		int cliente = esperar_cliente();
		if(cliente == -1){
			return 0;
		}

		id_mod modulo; // codigo para distinguir si es cpu o kernel
		if(recv(cliente, &modulo, sizeof(modulo), 0) != sizeof(modulo)){
			return 0;
		}
		args_thread* args = malloc(sizeof(args_thread));
		args->socket_cliente = cliente;
		switch(modulo){
			case KERNEL:
				log_info(logger, "Recibido mensaje de KERNEL, create y detach thread correspondiente");
				pthread_create(&t_kernel, NULL, (void*) escuchar_kernel, (void*) args);
			break;
			case CPU:
				log_info(logger, "Recibido mensaje de CPU, create y detach thread correspondiente");
				pthread_create(&t_cpu, NULL, (void*) escuchar_cpu, (void*) args);
			break;
		}
	}
	pthread_join(t_cpu, NULL);
	pthread_join(t_kernel, NULL);
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
		if(request == NULL)
			break;
		switch(request->estado){
			case 0: //Inicializar proceso
				log_info(logger, "Inicializando proceso con pcb id %d y size %d...", request->id_pcb, request->tamanio_pcb);
				uint32_t id_tabla = iniciar_proc(request->tamanio_pcb, request->id_pcb);
				enviar_mensaje_cliente(cliente, &id_tabla, sizeof(id_tabla));
				free(request);
			break;
			case 1 : //SUSP_PROC
				log_info(logger, "Suspendiendo proceso con pcb id %d...", request->id_pcb);
				suspender_proc(request->id_pcb);
				free(request);
				//enviar_mensaje_cliente(cliente, 1, sizeof(uint32_t));
			break;
			case 2: //EXIT
				log_info(logger, "Finalizando proceso con pcb id %d...", request->id_pcb);
				finalizar_proc(request->id_pcb);
				free(request);
				//enviar_mensaje_cliente(cliente, 1, sizeof(uint32_t));
			break;

		}
	}
	log_warning(logger, "El cliente se desconecto de memoria");
	close(cliente);
	return EXIT_SUCCESS;
}
void* escuchar_cpu(void* arg){
	//TODO: generar funcionalidad escuchar cpu
	log_info(logger, "Escuchando mensaje de CPU");
	args_thread* args = (args_thread*) arg;
	int cliente = args->socket_cliente;
	free(args);
	while (cliente != -1) {
		message_cpu* request = parsear_message_cpu(cliente);
		if(request == NULL)
			break;
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
				uint32_t direc_fisica, id_2do_nivel, id_entrada;
				get_values_from_data(request->data, &id_2do_nivel, &id_entrada, &direc_fisica, NULL);
				leer_en_memoria(id_2do_nivel, id_entrada, direc_fisica);
				uint32_t valor;
				enviar_mensaje_cliente(cliente, &valor, sizeof(uint32_t));
				free(request->data);
				free(request);
			}
			break;
			case WRITE:{
				log_info(logger, "Recibido request de WRITE");
				uint32_t direc_fisica, valor, id_2do_nivel, id_entrada;
				get_values_from_data(request->data, &id_2do_nivel, &id_entrada, &direc_fisica, &valor);
				char* response = escribir_memoria(id_2do_nivel, id_entrada, direc_fisica, &valor);
				enviar_mensaje_cliente(cliente, response, sizeof(uint32_t));
				free(request->data);
				free(request);
			}
			break;
			case COPY:
				log_info(logger, "Recibido request de COPY");
				log_error(logger, "No se deberia llamar. Hacer copy haciendo read y write");
				uint32_t direc_fisica1, direc_fisica2;
				get_values_from_data(request->data, &direc_fisica1, &direc_fisica2, NULL, NULL);
				free(request->data);
				free(request);
			break;
			case GET_PAG_NVL_2:{
				log_info(logger, "Recibido request de GET_PAG_NVL_2");
				uint32_t id_tabla_1, entrada;
				get_values_from_data(request->data, &id_tabla_1, &entrada, NULL, NULL);
				uint32_t id_tabla_2 = get_tabla_2do_lvl(id_tabla_1, entrada);
				enviar_mensaje_cliente(cliente, &id_tabla_2, sizeof(uint32_t));
				free(request->data);
				free(request);
			}
			break;
			case GET_MARCO:{
				log_info(logger, "Recibido request de GET_MARCO");
				uint32_t id_tabla_1, id_tabla_2, entrada;
				get_values_from_data(request->data, &id_tabla_1, &id_tabla_2, &entrada, NULL);
				uint32_t valor_en_mem = get_nro_marco(id_tabla_1, id_tabla_2, entrada);
				enviar_mensaje_cliente(cliente, &valor_en_mem, sizeof(uint32_t));
				free(request->data);
				free(request);
			}
			break;
		}

	}
	log_warning(logger, "El cliente se desconecto de memoria");
	close(cliente);
	return EXIT_SUCCESS;
}

int enviar_mensaje_cliente(int cliente, void* data, int size){
	return send(cliente, data, size, 0);
}

message_kernel* parsear_message_kernel(int cliente){
	message_kernel* request = malloc(sizeof(message_kernel));
    void* stream = malloc(100);
    int bytes_recibidos = recv(cliente, stream, 100, 0);
    if(bytes_recibidos < 1){
    	log_info(logger, "Cliente desconectado.");
        free(stream);
        free(request);
    	return NULL;
    }
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
    void* stream = malloc(100);
    int bytes_recibidos = recv(cliente, stream, 100, 0);
    log_info(logger, "Bytes recibidos de CPU: %d", bytes_recibidos);
    if(bytes_recibidos < 1){
    	log_info(logger, "Cliente desconectado.");
        free(stream);
        free(request);
    	return NULL;
    }
    memcpy(&(request->operacion), stream, sizeof(request->operacion));
    memcpy(&(request->data), stream + sizeof(request->operacion), 100 - sizeof(request->operacion));
    free(stream);
	return request;
}

void get_values_from_data(void* data, uint32_t* primer, uint32_t* segundo, uint32_t* tercero, uint32_t* cuarto){
	int size = sizeof(uint32_t);
	memcpy(primer, data, size);
	if(segundo != NULL)
		memcpy(segundo, data + size, size);
	if(tercero != NULL)
		memcpy(tercero, data + size*2, size);
	if(cuarto != NULL)
		memcpy(cuarto, data + size*3, size);
}

void finalizar_servidor(){
	close(socket_mem);
}
