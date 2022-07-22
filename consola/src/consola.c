#include "consola.h"


int main(int argc, char** argv) {
		//INICIALIZAR LOG
		log_consola = log_create("consola.log", "CONSOLA", 1, LOG_LEVEL_DEBUG);
		log_info(log_consola,"Hola, soy el log de la consola");
		log_debug(log_consola,"recibi los parametros: %s , %i",argv[1],atoi(argv[2]));
		//FIN INICIALIZAR LOG

		//ABRIR ARCHIVOS Y VALIDACIONES
		if (argc < 3) {
			log_error(log_consola,
					"No se pasaron los parametros necesarios para iniciar la consola");
		} else {
			//FILE* instrucciones = fopen("../consola-v2/identificadores_correctos.txt","r");
			FILE* instrucciones = fopen(argv[1], "r");

			if (instrucciones != NULL) {
				log_info(log_consola,
						"el archivo de instrucciones en %s fue abierto sin problemas",
						argv[1]);
			//FIN ABRIR ARCHIVO Y VALIDACIONES
				t_queue* instrucciones_parseadas = queue_create();
				//VALIDAR PROCESO
				//GUARDAR PROCESO
				if(el_proceso_es_valido(instrucciones, instrucciones_parseadas)) {
					//ABRIR CONFIGURACION
					t_config* consola_config = config_create("consola.config");
					log_debug(log_consola,"EL IP del config es: %s", config_get_string_value(consola_config, CLAVE_IP));
					log_debug(log_consola,"El puerto del config es: %s", config_get_string_value(consola_config, CLAVE_PUERTO));
					// INICIAR SOCKET Y CONEXTAR CON KERNEL
					int socket_consola = crear_conexion_con_kernel(consola_config);
					//HANDSHAKE
					if (socket_consola != -1 && se_pudo_hacer_el_handshake(socket_consola)) {
						t_paquete* paquete = serializar_proceso(instrucciones_parseadas, ENVIO_DATOS, atoi(argv[2]));
						enviar_paquete(paquete, socket_consola);

						if(fue_confirmado_el_envio(socket_consola)) {
							log_info(log_consola,"Se confirmo la entrega del proceso");
						}
					}
				}
				queue_destroy_and_destroy_elements(instrucciones_parseadas,(void*) destruir_instruccion);
			} else {
				log_error(log_consola,"el archivo en la direccion %s no existe.",argv[1]);
			}
		}

		log_info(log_consola,"Consola finalizada");
		log_destroy(log_consola);

		return 0;
}

void destruir_instruccion(t_instruccion* instruccion) {
	queue_destroy(instruccion->parametros);
	free(instruccion);
}

bool el_proceso_es_valido(FILE* instrucciones, t_queue* instrucciones_parseadas) {
	//VALIDAR PROCESO
	char* linea = string_new();
	int lineas_leidas = 0;
	char** instruccion;
	char* ultimo_identificador = string_new();
	//LEER INSTRUCCIONES
	while (fgets(linea, 15, instrucciones) != NULL) {
		lineas_leidas++;
		instruccion = malloc(string_array_size(string_split(linea," "))*sizeof(char*));
		instruccion = string_split(quitar_eol(linea)," ");
		//VALIDAR INSTRUCCION
		if (!es_una_instruccion_valida(instruccion)) {
			log_error(log_consola,
					"La instruccion en la linea %i no es correcta.",
					lineas_leidas);
			log_error(log_consola,"No se puede enviar datos incompletos. Finalizando...");
			return false;
		}
		agregar_instruccion(instrucciones_parseadas, instruccion);
		log_debug(log_consola, "La linea %i fue leida con la instruccion %s de parametro: %sy fue agregada con exito",
				lineas_leidas, instruccion[0],show_parametros(instruccion));
		strcpy(ultimo_identificador, instruccion[0]);
		//log_info(log_consola,"Lei la instruccion %s con los parametros: %s",instruccion[0],show_parametros(instruccion));
	}
	//show_contenido(instrucciones_parseadas);
	//VALIDAR QUE TERMINE CON EXIT
	if (!es_igual_a(ultimo_identificador,"EXIT")) {
		log_error(log_consola,
				"El proceso no termina con una instruccion de EXIT");
		free(ultimo_identificador);
		log_error(log_consola,"No se puede enviar datos incompletos. Finalizando...");
		return false;
	}
		log_info(log_consola,
				"Las %i instrucciones fueron validadas y guardadas",
				lineas_leidas);
		//show_contenido(instrucciones_parseadas);
		string_array_destroy(instruccion);
		return true;
	//FIN VALIDAR QUE TERMINE CON EXIT
}

char* quitar_eol(char* string) {
	if(string_ends_with(string,"\n")) {
		return string_substring_until(string,string_length(string) -1);
	}
	return string;
}

bool es_una_instruccion_valida(char** instruccion) {
	//VALIDAR INSTRUCCION
	t_list* identificadores_validos = list_create();
	inicializar_identificadores_validos(identificadores_validos);
	//VALIDAR IDENTIFICADOR
	if (esta_en_la_lista(identificadores_validos, instruccion[0])) {
	//FIN VALIDAR IDENTIFICADOR
		//VALIDAR PARAMETROS
		if (tiene_los_parametros_correctos(instruccion)) {
			list_destroy(identificadores_validos);
			return true;
		} else {
			log_error(log_consola,"Faltan parametros");
		}
	} else {
		log_error(log_consola,"EL identificador %s es desconocido",instruccion[0]);
	}
	//FIN VALIDAR PARAMETROS
	list_destroy(identificadores_validos);
	return false;
	//FIN VALIDAR INSTRUCCION
}

void inicializar_identificadores_validos(t_list* identificadores_validos) {

	list_add(identificadores_validos, "I/O");
	list_add(identificadores_validos, "READ");
	list_add(identificadores_validos, "COPY");
	list_add(identificadores_validos, "WRITE");
	list_add(identificadores_validos, "EXIT");
	list_add(identificadores_validos, "NO_OP");
}

bool esta_en_la_lista(t_list* lista, void* elemento_buscado) {
	bool es_el_elemento(void* elemento) {
		return es_igual_a(elemento, elemento_buscado);
	}
	return list_find(lista, es_el_elemento);
}

bool es_igual_a(void* un_string, void* otro_string) {
	return string_equals_ignore_case((char*) un_string, (char*)otro_string);
}

bool tiene_los_parametros_correctos(char** instruccion) {
	return cantidad_de_parametros(instruccion[0]) != string_array_size(instruccion) - 2;
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
		log_error(log_consola,
				"Se desconoce la cantidad de parametros que espera el identificador %s",
				identificador);
		return -1;
	}
}

void agregar_instruccion(t_queue* instrucciones, char** instruccion) {

	t_instruccion* instruccion_validada = malloc(sizeof(t_instruccion));
	char* identificador = string_duplicate(instruccion[0]);
	//strcat(identificador,"\0");
	instruccion_validada->identificador = identificador;
	instruccion_validada->tamanio_id = string_length(instruccion_validada->identificador);
	instruccion_validada->parametros = queue_create();
	agregar_parametros(instruccion_validada, instruccion);
	queue_push(instrucciones, instruccion_validada);
}

void agregar_parametros(t_instruccion* instruccion_validada, char** instruccion) {

	int contador = 0;
	while (contador <= string_array_size(instruccion)-2
			&& instruccion[++contador] != NULL) {
		log_debug(log_consola,"meti este valor como parametro nro %i: %i",contador,(UNSIGNED_INT) atoi(instruccion[contador]));
		queue_push(instruccion_validada->parametros,
				(void*) atoi(instruccion[contador]));
	}
}

int crear_conexion_con_kernel(t_config* config) {

	if (!config_has_property(config, CLAVE_IP)
			|| !config_has_property(config, CLAVE_PUERTO)) {
		log_error(log_consola,
				"El archivo de configuracion no contiene las claves necesarias. (%s) (%s)",
				CLAVE_IP, CLAVE_PUERTO);
	} else {
		char* ip = config_get_string_value(config, CLAVE_IP);
		char* puerto = config_get_string_value(config, CLAVE_PUERTO);

		log_info(log_consola, "Lei la IP: %i y el PUERTO: %s", ip, puerto);

		int conexion = crear_conexion(ip, puerto);
		log_info(log_consola, "Conexion creada: %i", conexion);
		free(ip);
		free(puerto);
		return conexion;
	}
	return -1;
}

int crear_conexion(char* ip, char* puerto) {
	struct addrinfo hints;
	struct addrinfo* server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_consola = socket(server_info->ai_family,
			server_info->ai_socktype, server_info->ai_protocol);

	connect(socket_consola, server_info->ai_addr, server_info->ai_addrlen);

	freeaddrinfo(server_info);

	return socket_consola;
}

bool se_pudo_hacer_el_handshake(int socket_consola) {

	if(!enviar_mensaje(HANDSHAKE, MENSAJE_HANDSHAKE_ENVIADO, socket_consola)) {
		log_error(log_consola,"No se pudo iniciar el handshake");
		return false;
	}
	return confirmacion_handshake(socket_consola);
}

bool confirmacion_handshake(int socket_consola) {

	t_mensaje* recibido = malloc(sizeof(t_mensaje));
	op_code *operacion = malloc(sizeof(op_code));
	recv(socket_consola, operacion, sizeof(op_code),0);
	if (*operacion != HANDSHAKE) {
		log_error(log_consola,"La consola no implementa el mismo protocolo handshake que el kernel");
		free(recibido);
		return false;
	}
	deserializar_handshake(recibido,socket_consola);
	if(!se_recibio_el_mensaje_correcto(recibido,socket_consola)) {
		log_error(log_consola,"No se pudo deserializar el paquete");
		free(recibido);
		return false;
	}
	return true;
}

void deserializar_handshake(t_mensaje* recibido,int socket_consola) {
	recv(socket_consola, &(recibido->tamanio_mensaje),sizeof(int),0);
	uint32_t *size_string = malloc(sizeof(uint32_t));
	recv(socket_consola, size_string,sizeof(uint32_t),0);
	void* data = malloc(*size_string);
	recv(socket_consola, data, *size_string,0);
	recibido->mensaje = data;
}

bool se_recibio_el_mensaje_correcto(t_mensaje* recibido, int socket_consola) {
	if(!es_igual_a(recibido->mensaje, MENSAJE_HANDSHAKE_ESPERADO)) {
		log_error(log_consola,"No se recibio el mensaje esperado, se recibio: %s y se esperaba %s",recibido->mensaje,MENSAJE_HANDSHAKE_ESPERADO);
		free(recibido);
		return false;
	}
	free(recibido);
	log_info(log_consola,"Se recibio el mensaje esperado del handshake");
	return true;
}

bool enviar_mensaje(op_code codigo, char* mensaje, int socket_consola) {
	t_paquete* envio = malloc(sizeof(t_paquete));
	envio->operacion = codigo;
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

t_paquete* serializar_proceso(t_queue* contenido, op_code codigo,
		int tamanio_proceso) {
	t_paquete* paquete_serializado = malloc(sizeof(t_paquete));
	paquete_serializado->operacion = codigo;
	paquete_serializado->buffer = malloc(sizeof(t_buffer));
	log_debug(log_consola,"codigo: %i",paquete_serializado->operacion);
	paquete_serializado->buffer->size = sizeof(int) //Codigo
			+ sizeof(tamanio_proceso) // TAMAÑO PROCESO
			+ queue_size(contenido)*sizeof(int)
			+ tamanio_identificadores(contenido)*sizeof(char)
			+ parametros_totales(contenido)*sizeof(UNSIGNED_INT);
	void* buffer = malloc(paquete_serializado->buffer->size);
	agregar_contenido(contenido,buffer,tamanio_proceso);
	paquete_serializado->buffer->stream = buffer;
	return paquete_serializado;
}

int tamanio_identificadores(t_queue* instrucciones) {

	t_list* tamanios = list_map(instrucciones->elements,(void*) tamanio_identificador);
	log_debug(log_consola,"total identificadores: %i",(int) list_fold1(tamanios,(void*) sumar));
	return (int) list_fold1(tamanios,(void*) sumar);
}

int tamanio_identificador(t_instruccion* instruccion) {
	//log_debug(log_consola,"Instruccion leida: %s",instruccion->identificador);
	//log_debug(log_consola,"cantidad param: %i",cantidad_de_parametros(instruccion->identificador));
	return instruccion->tamanio_id;
}

int sumar(int num1, int num2) {
	return num1 + num2;
}

int parametros_totales(t_queue* contenido) {
	t_list* tamanios = list_map(contenido->elements,(void*) parametros);
	log_debug(log_consola,"Parametros totales: %i",(int) list_fold1(tamanios,(void*) sumar));
	return (int) list_fold1(tamanios,(void*) sumar);
}

int parametros(t_instruccion* nodo) {
	return cantidad_de_parametros(nodo->identificador);
}

void agregar_contenido(t_queue* contenido, void* buffer, int tamanio) {
	int desplazamiento = 0;
	log_debug(log_consola,"Desplazamiento actual: %i",desplazamiento);
	t_instruccion* instruccion = malloc(sizeof(t_instruccion));
	while (!queue_is_empty(contenido)) {
		instruccion = queue_peek(contenido);
		//string_append(instruccion->identificador,"\0");
		log_debug(log_consola,"Desplazamiento actual: %i",desplazamiento);
		//log_debug(log_consola,"tamanio del id: %i",sizeof(instruccion->tamanio_id + 1));
		memcpy(buffer + desplazamiento, &(instruccion->tamanio_id), sizeof(int));
		desplazamiento += sizeof(int);
		log_debug(log_consola,"Desplazamiento actual: %i",desplazamiento);
		log_debug(log_consola,"identificador: %s",instruccion->identificador);
		memcpy(buffer + desplazamiento, &(instruccion->identificador),
				sizeof(char) * instruccion->tamanio_id);
		desplazamiento += sizeof(char) * instruccion->tamanio_id;
		log_debug(log_consola,"Desplazamiento actual: %i",desplazamiento);
		//show_contenido(instruccion->parametros);
		copiar_parametros(buffer, &desplazamiento, instruccion->parametros);
		queue_pop(contenido);

	}
	memcpy(buffer, &tamanio, sizeof(int));
	//desplazamiento += sizeof(char);
	//log_debug(log_consola,"datos grabados (a lo guaso): %i",(int*) buffer);
	free(instruccion);
}

void copiar_parametros(void* buffer, int *desplazamiento, t_queue* parametros) {
	if(queue_size(parametros) > 0) {
		UNSIGNED_INT parametro_actual;
			while (!queue_is_empty(parametros)) {
				parametro_actual = (UNSIGNED_INT) queue_peek(parametros);
				log_debug(log_consola,"param actual: %i",parametro_actual);
				memcpy(buffer + *desplazamiento, &parametro_actual,
						sizeof(UNSIGNED_INT));
				*desplazamiento += sizeof(UNSIGNED_INT);
				queue_pop(parametros);
			}
	}
}

bool enviar_paquete(t_paquete* paquete, int socket_consola) {
	int *tamanio_datos = malloc(sizeof(int));
	*tamanio_datos = sizeof(op_code) + paquete->buffer->size + sizeof(int);
	void* datos_a_enviar = malloc(*tamanio_datos);
	int desplazamiento = 0;
	memcpy(datos_a_enviar + desplazamiento,&(paquete->operacion),sizeof(op_code));
	desplazamiento += sizeof(op_code);
	memcpy(datos_a_enviar + desplazamiento,&(paquete->buffer->size),sizeof(int));
	desplazamiento += sizeof(int);
	int sizeChar = *tamanio_datos - sizeof(op_code) - sizeof(int);
	memcpy(datos_a_enviar + desplazamiento,paquete->buffer->stream, sizeChar);

	if(send(socket_consola,datos_a_enviar,*tamanio_datos,0) == -1) {
		eliminar_paquete(paquete);
		free(datos_a_enviar);
		free(tamanio_datos);
		log_error(log_consola,"No se pudo enviar el paquete");
		return false;
	}
	eliminar_paquete(paquete);
	free(datos_a_enviar);
	free(tamanio_datos);
	log_info(log_consola,"EL paquete fue enviado con exito");
	return true;
}

void eliminar_paquete(t_paquete* paquete) {
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

bool fue_confirmado_el_envio(int socket_consola) {
	// ESPERO RECIBIR "RECIBIDO :)", CUALQUIER OTRA COSA IMPLICA FALLO
	t_mensaje* recibido = malloc(sizeof(t_mensaje));
	char* mensaje_recibido = string_new();

	if(recv(socket_consola, &(recibido->operacion),sizeof(op_code),0) == -1) {
		log_error(log_consola,"Conexion perdida, no se puede confirmar envio");
	} else if(recibido->operacion != CONFIRMACION) {
		log_error(log_consola,"No se recibio el codigo de operacion esperado");
	} else if (recv(socket_consola, &(recibido->tamanio_mensaje), sizeof(int), 0)
			== -1) {
		log_error(log_consola,"Conexion perdida, no se puede confirmar envio");
	} else {
		if(recv(socket_consola,recibido->mensaje,recibido->tamanio_mensaje,0) == -1) {
			log_error(log_consola,"Conexion perdida, no se puede confirmar envio");
		} else {
			strcat(recibido->mensaje,"\0");
			mensaje_recibido = string_duplicate(recibido->mensaje);
		}
	}
	free(recibido);
	return es_igual_a(mensaje_recibido,CONFIRM_ESPERADA);
}


//PARA TESTEO
char* show_parametros(char** instruccion) {
	char* parametros = string_new();
	for(int i = 1; i <= string_array_size(instruccion) -1; i++) {
		strcat(parametros,instruccion[i]);
		strcat(parametros,", ");
	}
	return parametros;
}

void show_contenido(t_queue* contenido) {
	t_instruccion* instruccion = malloc(sizeof(t_instruccion));
	int contador = 1;
	t_queue* aux = malloc(sizeof(t_queue));
	aux->elements = list_duplicate(contenido->elements);
	aux->elements->elements_count = contenido->elements->elements_count;
	log_debug(log_consola,"mostrando elementos");
	while(!queue_is_empty(aux)){
		instruccion = queue_peek(aux);
		log_debug(log_consola,"contenido %i: %s, tamaño id: %i, cant_params: %i",contador,instruccion->identificador,instruccion->tamanio_id,queue_size(instruccion->parametros));
		contador++;
		queue_pop(aux);
	}
	free(aux);
}

void deserializar_datos(t_proceso* proceso, t_paquete* stream) {
	int desplazamiento = 0;
	t_instruccion* instruccion_actual = malloc(sizeof(t_instruccion));
	memcpy(&(proceso->tamanio_proceso),&(stream->buffer->stream) + desplazamiento,sizeof(int));
	log_debug(log_consola,"Tamaño del proceso: %i",proceso->tamanio_proceso);
	desplazamiento += sizeof(int);
	log_debug(log_consola,"Deserializando las instrucciones...");
	while(desplazamiento < stream->buffer->size) {

		memcpy(&(instruccion_actual->tamanio_id),&(stream->buffer->stream) + desplazamiento,sizeof(int));
		log_debug(log_consola,"grabe tamaño id: %i",instruccion_actual->tamanio_id);
		desplazamiento += sizeof(int);
		memcpy(&(instruccion_actual->identificador),&(stream->buffer->stream) + desplazamiento, instruccion_actual->tamanio_id);
		desplazamiento += instruccion_actual->tamanio_id;
		log_debug(log_consola,"grabe id: %s",instruccion_actual->identificador);
		log_debug(log_consola,"leyendo parametros...");
		int parametro_actual;
		instruccion_actual->parametros = queue_create();
		for(int i = 1; i <= cantidad_de_parametros(instruccion_actual->identificador); i++){
			memcpy(&parametro_actual,stream->buffer->stream + desplazamiento,sizeof(int));
			log_debug(log_consola,"grabe param %i: %i",i,parametro_actual);
			desplazamiento += sizeof(int);
			queue_push(instruccion_actual->parametros,(void*) parametro_actual);
		}
		t_instruccion* instruccion_lista = malloc(sizeof(t_instruccion));
		instruccion_lista->identificador = string_duplicate(instruccion_actual->identificador);
		instruccion_lista->tamanio_id = instruccion_actual->tamanio_id;
		instruccion_lista->parametros = instruccion_actual->parametros;
		//queue_destroy(instruccion_actual->parametros);
		instruccion_actual->parametros = queue_create();
		queue_push(proceso->instrucciones,instruccion_lista);
	}

}

void deserializar_mensaje(char* mensaje,t_paquete* paquete) {
	memcpy(mensaje,&(paquete->buffer->stream),paquete->buffer->size);

}

void show_proceso(t_proceso* proceso) {
	log_info(log_consola,"MOSTRANDO PROCESO DESERIALIZADO");
	log_info(log_consola,"Tamaño del proceso: %i",proceso->tamanio_proceso);
	show_instrucciones(proceso->instrucciones);
}

void show_instrucciones(t_queue* instrucciones) {
	t_queue* aux = malloc(sizeof(t_queue));
	t_instruccion* instruccion_actual = malloc(sizeof(t_instruccion));
	aux->elements = list_duplicate(instrucciones->elements);
	while(!queue_is_empty(aux)) {
		instruccion_actual = queue_peek(aux);
		log_info(log_consola,"Tamaño id: %i",instruccion_actual->tamanio_id);
		log_info(log_consola,"Identificador: %s",instruccion_actual->identificador);
		show_parametros_leidos(list_duplicate(instruccion_actual->parametros->elements));
		queue_pop(aux);
	}
	queue_destroy(aux);
}

void show_parametros_leidos(t_list* parametros) {
	t_queue* aux = malloc(sizeof(t_queue));
	aux->elements = parametros;
	int parametro = 0;
	while(!queue_is_empty(aux)){
		parametro = (int) queue_peek(aux);
		log_info(log_consola,"Parametro: %i",parametro);
		queue_pop(aux);
	}
}

void show_paquete(t_paquete* paquete){
	log_debug(log_consola,"Mostrando paquete...");
	log_debug(log_consola,"operacion: %i",paquete->operacion);
	log_debug(log_consola,"tam datos a enviar: %i",paquete->buffer->size);
	char* algo = paquete->buffer->stream;
	log_debug(log_consola,"stream: %s", algo);
}
