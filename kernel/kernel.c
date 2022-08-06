#include "kernel.h"

void inicializar_direccion_kernel(struct sockaddr_in *direccion_kernel);
void bindear_kernel(int kernel, struct sockaddr_in direccion_kernel);
void escuchar(int kernel);
void procesar_entradas_de_consolas(int kernel);
void atender_consola(int consola);
bool se_pudo_hacer_el_handshake(int consola);
void enviar_confirmacion(int consola);
bool se_pudo_recibir_el_proceso(int consola,t_proceso* proceso);
bool recibir_proceso(int consola, t_proceso* proceso);
t_proceso* deserializar_proceso(void* contenido,t_proceso* proceso, int tam_max);
int cantidad_de_parametros(ID_INSTRUCCION identificador);
void enviar_confirmacion(int consola);
void mostrar_envio(void* envio, int tam);
void mostrar_proceso(t_proceso* proceso);
void mostrar_instruccion(t_instruccion* instruccion);
void proceso_destroyer(t_proceso* proceso);

int main(int argc, char **argv)
{
	char* path_config;
	if(argc < 2){
		path_config = "kernel.config";
	}else{
		path_config = argv[1];
	}

	loggerKernel = iniciar_logger_kernel();
	configKernel = iniciar_config_kernel(path_config);

	iniciar_settings();

	pthread_mutex_lock(&variableEjecutando);
	ejecutando = 0;
	pthread_mutex_unlock(&variableEjecutando);

	//struct sockaddr_in direccion_kernel;

	//inicializar_direccion_kernel(&direccion_kernel);

//	int kernel = socket(AF_INET, SOCK_STREAM, 0);
//	int enable = 1;
//	setsockopt(kernel, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	//setsockopt(kernel, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int));

	//bindear_kernel(kernel, direccion_kernel);
	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(NULL, config_kernel.puerto_escucha, &hints, &servinfo);
	int server = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	int enable = 1;
	setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
	setsockopt(server, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int));

	bind(server, servinfo->ai_addr, servinfo->ai_addrlen);

	freeaddrinfo(servinfo);
	escuchar(server);

	iniciar_planificadores();

	procesar_entradas_de_consolas(server);

//	finalizar_programa();

	return 0;

}
void finalizar_programa(){

	log_warning(loggerKernel, "Finalizando Kernel...");
	close(socket_memoria);
	close(socket_dispatch);
	close(socket_interrupt);

	pthread_mutex_destroy(&mutexReady);
	pthread_mutex_destroy(&mutexExit);
	pthread_mutex_destroy(&mutexBloqueo);
	pthread_mutex_destroy(&mutexBloqueoSuspendido);
	pthread_mutex_destroy(&mutexSuspendido);
	pthread_mutex_destroy(&mutexNew);

	sem_destroy(&grado_multiprogramacion);
	sem_destroy(&prioridad_SuspendedReady);
	sem_destroy(&nuevoProcesoReady);
	sem_destroy(&enviarInterrupcion);
	sem_destroy(&bloqueoMax);
	sem_destroy(&procesoBloqueado);
	sem_destroy(&finalizoProceso);
	sem_destroy(&hayProcesoAnalizar);

	log_destroy(loggerKernel);
	config_destroy(configKernel);
/*
	t_list* procesosNew;
	t_list* procesosReady;
	t_list* procesosExecute;
	t_list* procesosBlocked;
	t_list* procesosSuspendedReady;
	t_list* procesosExit;
	t_list* conexiones_pcb;

	list_destroy(procesosNew);*/
}


void inicializar_direccion_kernel(struct sockaddr_in *direccion_kernel) {

	direccion_kernel->sin_family = AF_INET;
	direccion_kernel->sin_addr.s_addr = INADDR_ANY;
	direccion_kernel->sin_port = htons(atoi(config_kernel.puerto_escucha));

}

void bindear_kernel(int kernel, struct sockaddr_in direccion_kernel) {
	if (bind(kernel, (void*) &(direccion_kernel), sizeof(direccion_kernel))
			== -1) {
		log_error(loggerKernel, "Fallo el bind\n");
	} else {
		log_info(loggerKernel, "El kernel fue bindeado con exito\n");
	}
}

void escuchar(int kernel) {
	if(listen(kernel, SOMAXCONN) != 0) {
		log_error(loggerKernel, "No se pudo escuchar el puerto %s",config_kernel.puerto_escucha);
	} else {
		log_info(loggerKernel, "El kernel esta escuchando el puerto: %s\n", config_kernel.puerto_escucha);
		log_info(loggerKernel, "Esperando conexiones...\n");
	}

}


void procesar_entradas_de_consolas(int kernel) {
    int id_ultimo_pcb = 0;
    while (1) {
        pthread_t hilo;
        int *consola = malloc(sizeof(int));
        *consola = accept(kernel, NULL,NULL);

        if (*consola == -1) {
            log_error(loggerKernel,"No se pudo aceptar a la consola\n");
        } else {
            log_info(loggerKernel, "SOCKET CONSOLA ACTUAL: %i", *consola);
            log_info(loggerKernel, "ID PCB ACTUAL: %i", id_ultimo_pcb);

            consola_pcb* nuevaConexion = malloc(sizeof(consola_pcb));
            nuevaConexion->socket_consola = *consola;
            nuevaConexion->pcbVinculado = id_ultimo_pcb;
            list_add(conexiones_pcb, nuevaConexion);
            //free(nuevaConexion);
            pthread_create(&hilo, NULL, (void*) atender_consola,(void*) *consola);
            pthread_detach(hilo);
            //free(consola);
        }

        log_info(loggerKernel, "Esperando nueva conexion...\n");
        id_ultimo_pcb++;
        free(consola);
    }

}

void atender_consola(int consola) {
	log_info(loggerKernel, "preparado para recibir datos de la consola...\n");
	log_info(loggerKernel,"Estoy atendiendo a la consola %i",consola);

	t_proceso* proceso = malloc(sizeof(t_proceso));
	proceso->instrucciones = queue_create();
	log_info(loggerKernel,"Iniciando protocolo handshake...");
	if(se_pudo_hacer_el_handshake(consola)) {
		log_info(loggerKernel,"Se pudo hacer el handshake :D");

		//DENTRO DE RECIBIR_PROCESO SE TIENE QUE ARMAR EL PCB
		if (!se_pudo_recibir_el_proceso(consola,proceso)) {
			log_error(loggerKernel,"No se pudo recibir el proceso :C");
		} else {
			//mostrar_proceso(proceso);
			log_debug(loggerKernel,"Generando PCB...");
			int idPCB = devolverID_PCB(consola);
			generarEstructuraPCB(idPCB, proceso);
		}
	} else {
		log_error(loggerKernel,"No se pudo completar el handshake :C");
		op_code cod = ERROR;
		send(consola,&cod,sizeof(op_code),MSG_NOSIGNAL);
	}
	proceso_destroyer(proceso);
}

void proceso_destroyer(t_proceso* proceso){
    queue_destroy(proceso -> instrucciones);
	free(proceso);
}

bool se_pudo_recibir_el_proceso(int consola, t_proceso* proceso) {

	if (recibir_proceso(consola,proceso)) {
		enviar_confirmacion(consola);
		return true;
	}

	return false;
}

void enviar_confirmacion(int consola) {

	op_code cod = CONFIRMACION;
	send(consola,&cod,sizeof(op_code),MSG_NOSIGNAL);
	log_info(loggerKernel,"Envie confirmacion :D");
}

bool recibir_proceso(int consola,t_proceso* proceso) {


	op_code cod;
	recv(consola,&cod,sizeof(op_code),0);

	log_info(loggerKernel,"%i",cod);

	int tam_tot;
	recv(consola,&tam_tot,sizeof(int),0);
	log_info(loggerKernel,"%i",tam_tot);

	int tam_proceso;
	recv(consola,&tam_proceso,sizeof(int),0);

	void* contenido = malloc(tam_tot);
	recv(consola,contenido,tam_tot,0);

	log_debug(loggerKernel,"Cosa %i",tam_proceso);
	//mostrar_envio(contenido, tam_tot);

	if (cod == ENVIO_PROCESO) {
		log_info(loggerKernel,"Recibi el proceso :D");
		proceso->tamanio_proceso = tam_proceso;
		deserializar_proceso(contenido,proceso,tam_tot);
		free(contenido);
		return true;
	}

	log_error(loggerKernel,"Todo mal");
	free(contenido);
	return true;
}

void mostrar_envio(void* envio, int tam) {

	ID_INSTRUCCION *id = malloc(sizeof(ID_INSTRUCCION));
	int *parametro = malloc(sizeof(int));
	int desp = 0;

	while(desp < tam) {

		memcpy(id,envio + desp,sizeof(ID_INSTRUCCION));
		log_debug(loggerKernel,"ID: %i",*id);
		desp += sizeof(ID_INSTRUCCION);

		for(int i = 0; i < cantidad_de_parametros(*id); i++) {

			memcpy(parametro,envio + desp,sizeof(int));
			desp += sizeof(int);
			log_debug(loggerKernel,"PARAMETRO: %i",*parametro);
		}
	}
	free(id);
	free(parametro);


}

t_proceso* deserializar_proceso(void* contenido,t_proceso* proceso, int tam_max) {

	log_debug(loggerKernel,"Empezando a deserializar");
	int desp = 0;

	ID_INSTRUCCION *id = malloc(sizeof(ID_INSTRUCCION));
	int *parametro = malloc(sizeof(int));
	t_queue* parametros;
	while(desp < tam_max) {

		t_instruccion* instruccion = malloc(sizeof(t_instruccion));

		parametros = queue_create();
		memcpy(id,contenido + desp, sizeof(ID_INSTRUCCION));
		desp += sizeof(ID_INSTRUCCION);

		//log_debug(loggerKernel,"ID: %i",*id);

		for(int i = 0; i < cantidad_de_parametros(*id); i++) {

			memcpy(parametro,contenido + desp, sizeof(int));
			desp += sizeof(int);
			queue_push(parametros,(void*) *parametro);
			//log_debug(loggerKernel,"PARAMETRO: %i",*parametro);
		}

		instruccion->identificador = *id;
		instruccion->parametros = parametros;
		//mostrar_instruccion(instruccion);
		queue_push(proceso->instrucciones,instruccion);

	}
	free(id);
	free(parametro);
	//mostrar_proceso(proceso);
	return proceso;

}

bool se_pudo_hacer_el_handshake(int consola) {

	op_code codigo;
	int mensaje;

	recv(consola,&codigo,sizeof(op_code),0);

	recv(consola,&mensaje,15,0);

	log_debug(loggerKernel,"MENSAJE HANDSHAKE RECIBIDO: %i",mensaje);
	if ( MENSAJE_HANDSHAKE_ESPERADO == mensaje && codigo == HANDSHAKE) {
		log_info(loggerKernel,"Se recibió el mensaje correcto");
		enviar_confirmacion(consola);
		return true;
	}
	log_info(loggerKernel,"No se recibió el mensaje correcto");
	return false;

}


void mostrar_proceso(t_proceso* proceso) {

	log_debug(loggerKernel,"TAMAÑO: %i",proceso->tamanio_proceso);

	list_map(proceso->instrucciones->elements,(void*) mostrar_instruccion);
}

void mostrar_instruccion(t_instruccion* instruccion) {

	log_debug(loggerKernel,"ID: %i",(instruccion->identificador));
	for(int i = 0; i < cantidad_de_parametros(instruccion->identificador); i++) {
		log_debug(loggerKernel,"PARAMETRO: %i",(int) list_get(instruccion->parametros->elements, i));
	}
}
