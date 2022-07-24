#include "kernel.h"

void inicializar_direccion_kernel(struct sockaddr_in *direccion_kernel) {

	direccion_kernel->sin_family = AF_INET;
	direccion_kernel->sin_addr.s_addr = INADDR_ANY;
	direccion_kernel->sin_port = htons(atoi(PUERTO));

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
		log_error(loggerKernel, "No se pudo escuchar el puerto %s",PUERTO);
	} else {
		log_info(loggerKernel, "El kernel esta escuchando el puerto: %s\n", PUERTO);
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

            consola_pcb * nuevaConexion = malloc(sizeof(consola_pcb));
            nuevaConexion -> socket_consola = *consola;
            nuevaConexion -> pcbVinculado = id_ultimo_pcb;
            list_add(conexiones_pcb, nuevaConexion);
            //TODO: devolver a kernel que termino
            pthread_create(&hilo, NULL, (void*) atender_consola,(void*) *consola);
            pthread_detach(hilo);
            free(consola);
        }

        log_info(loggerKernel, "Esperando nueva conexion...\n");
        id_ultimo_pcb++;
    }

}


bool protocolo_handshake(int consola) {
	op_code *operacion = malloc(sizeof(op_code));
	recv(consola,operacion,sizeof(op_code),0);
	if(*operacion != HANDSHAKE) {
		log_error(loggerKernel,"La consola no implementa el mismo protocolo handshake del kernel");
		free(operacion);
		return false;
	}
	free(operacion);
	t_mensaje* recibido = malloc(sizeof(t_mensaje));
	recv(consola,&(recibido->tamanio_mensaje), sizeof(int),0);
	int* sizeMensaje = malloc(sizeof(int));
	int* charTest = malloc(sizeof(int));
	recv(consola,sizeMensaje, sizeof(int),0);
	void* tst = malloc(*sizeMensaje);
	recv(consola,tst,*sizeMensaje,0);
	recibido->mensaje = tst;
	string_append(&recibido->mensaje, "\0");
	free(charTest);
	free(sizeMensaje);
	if(!es_igual_a(recibido->mensaje, "HOLA DON PEPITO")) {
		log_error(loggerKernel,"EL mensaje recibido no era el esperado: recibido: %s, esperado: %s",recibido->mensaje,MENSAJE_HANDSHAKE_ESPERADO);
	} else if (!enviar_mensaje(HANDSHAKE,MENSAJE_HANDSHAKE_ENVIADO,consola)) {
		log_error(loggerKernel,"No se pudo enviar el mensaje de handshake");
	} else {
		log_info(loggerKernel,"El protocolo handshake fue aprobado");
		free(recibido->mensaje);
		free(recibido);
		return true;
	}
	free(recibido->mensaje);
	free(recibido);
	return false;
}

bool es_igual_a(char* un_string, char* otro_string) {
	return string_equals_ignore_case(un_string, otro_string);
}

void atender_consola(int consola) {
    log_info(loggerKernel, "preparado para recibir datos de la consola...\n");
    log_info(loggerKernel,"Iniciando protocolo handshake...");
    t_proceso* proceso = malloc(sizeof(t_proceso));
    proceso->instrucciones = queue_create();
    if(!protocolo_handshake(consola)) {
        log_error(loggerKernel,"La consola no implementa el mismo protocolo habdshake que el kernel");
    } else if(!se_pudo_recibir_el_proceso(proceso, consola)) {
        log_error(loggerKernel,"No se pudo recibir el proceso");
    } else {
        int idPCB = devolverID_PCB(consola);
        generar_PCB(idPCB, proceso);
        enviar_confirmacion(consola);
    }

}


bool se_pudo_recibir_el_proceso(t_proceso* proceso,int consola) {
	int *tamanio_recibido = malloc(sizeof(int));
	if(recv(consola,tamanio_recibido,sizeof(int),0) == 0) {
		log_error(loggerKernel,"La consola se desconecto.");
		free(tamanio_recibido);
		return false;
	} else {
		proceso->tamanio_proceso = *tamanio_recibido;
		recibir_instrucciones(proceso,consola);
		free(tamanio_recibido);
		return true;
	}
}

void recibir_instrucciones(t_proceso* proceso, int consola) {
	while(se_pueden_recibir_instrucciones(proceso,consola));
}

bool se_pueden_recibir_instrucciones(t_proceso* proceso, int consola) {
	t_instruccion* instruccion = malloc(sizeof(t_instruccion));
	instruccion->parametros = queue_create();
	recv(consola,&(instruccion->tamanio_id),sizeof(int),0);
	void* string = malloc(instruccion->tamanio_id);
	recv(consola,string,instruccion->tamanio_id,0);
	instruccion->identificador = string;
	recibir_parametros(instruccion, consola);
	queue_push(proceso->instrucciones,instruccion);
	return es_igual_a(instruccion->identificador,"EXIT");
}

void recibir_parametros(t_instruccion* instruccion, int consola) {
	int contador = 0;
	while(contador < cantidad_de_parametros(instruccion->identificador)){
		int *parametro = malloc(sizeof(int));
		recv(consola,parametro,sizeof(int),0);
		queue_push(instruccion->parametros,(void*) *parametro);
	}
}

int cantidad_de_parametros(char* identificador) {

	if (es_igual_a(identificador, "I/O") || es_igual_a(identificador, "READ")
			|| es_igual_a(identificador, "NO_OP")) {
		return 1;
	} else if (es_igual_a(identificador, "COPY")
			|| es_igual_a(identificador, "WRITE")) {
		return 2;
	} else if (es_igual_a(identificador, "EXIT")) {
		return 0;
	} else {
		log_error(loggerKernel,
				"Se desconoce la cantidad de parametros que espera el identificador %s",
				identificador);
		return -1;
	}
}

void enviar_confirmacion(int consola) {
	if(!enviar_mensaje(CONFIRMACION,CONFIRM_ENVIADA ,consola)) {
		log_error(loggerKernel,"No se pudo enviar la confirmacion");
	}
}

bool enviar_mensaje(op_code codigo, char* mensaje, int socket_consola) {
	t_paquete* envio = malloc(sizeof(t_paquete));
	envio->codigo_operacion = codigo;
	envio->buffer = malloc(sizeof(t_buffer));
	envio->buffer->size = string_length(mensaje) +1  + sizeof(int);
	envio->buffer->stream = malloc(envio->buffer->size);
	int desplazamiento = 0;
	int *tamanio = malloc(sizeof(int));
	*tamanio = strlen(mensaje)+1;
	memcpy(envio->buffer->stream + desplazamiento,tamanio,sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(envio->buffer->stream + desplazamiento,mensaje, *tamanio);
	free(tamanio);
	return enviar_paquete(envio,socket_consola);
}

bool enviar_paquete(t_paquete* paquete, int socket_consola) {
	int *tamanio_datos = malloc(sizeof(int));
	*tamanio_datos = sizeof(op_code) + paquete->buffer->size + sizeof(int);
	void* datos_a_enviar = malloc(*tamanio_datos);
	int desplazamiento = 0;
	memcpy(datos_a_enviar + desplazamiento,&(paquete->codigo_operacion),sizeof(op_code));
	desplazamiento += sizeof(op_code);
	memcpy(datos_a_enviar + desplazamiento,&(paquete->buffer->size),sizeof(int));
	desplazamiento += sizeof(int);
	int sizeChar = *tamanio_datos - sizeof(op_code) - sizeof(int);
	memcpy(datos_a_enviar + desplazamiento,paquete->buffer->stream, sizeChar);

	if(send(socket_consola,datos_a_enviar,*tamanio_datos,0) == -1) {
		eliminar_paquete(paquete);
		free(datos_a_enviar);
		free(tamanio_datos);
		log_error(loggerKernel,"No se pudo enviar el paquete");
		return false;
	}
	eliminar_paquete(paquete);
	free(datos_a_enviar);
	free(tamanio_datos);
	log_info(loggerKernel,"EL paquete fue enviado con exito");
	return true;
}

void eliminar_paquete(t_paquete* paquete) {
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}