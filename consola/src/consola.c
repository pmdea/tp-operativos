#include "../src/consola.h"

void inicializar_log_consola();
void destruir_log_consola();
void print_mensaje_info(char* mensaje);
void print_mensaje_error(char* mensaje);
bool el_proceso_es_valido(FILE* instrucciones);
bool es_una_instruccion_valida(char** instruccion);
bool tiene_los_parametros_correctos(char** instruccion);
bool es_valido(char* identificador);
int parametros_segun_id(char* identificador);
int cantidad_de_parametros(ID_INSTRUCCION identificador);
ID_INSTRUCCION get_id(char* identificador);
int crear_conexion_con_kernel(t_config* config);
int crear_conexion(char* ip, char* puerto);
bool se_pudo_hacer_el_handshake(int socket);
void recibir_confirmacion(op_code* cod_respuesta, int socket);
bool fue_confirmado_el_envio(int socket_consola);
void enviar_paquete(int socket, FILE* archivo, int tam);
void esperar_finalizacion_proceso(int socket_consola);
void finalizar(FILE* archivo_instrucciones,t_config* consola_config);
void mostrar_envio(void* envio, int tam);

t_log* log_consola;
int cant_parametros_leidos;
int cant_instrucciones;

int main(int argc, char** argv) {

	inicializar_log_consola();

	FILE* archivo_instrucciones = fopen(argv[1], "r");
	int tam_proceso = atoi(argv[2]);

	char* path_config = argv[3];

	t_config* consola_config = config_create(path_config);

	if (argc < 3) {
		print_mensaje_error("No se pasaron los parámetros necesarios");
		finalizar(archivo_instrucciones, consola_config);
		return -1;
	}

	if (archivo_instrucciones == NULL) {
		print_mensaje_error("El archivo en la ruta especificada no existe");
		finalizar(archivo_instrucciones, consola_config);
		return -1;
	}

	log_info(log_consola,
			"El archivo en la ruta \"%s\" fue abierto correctamente", argv[1]);

	if (!el_proceso_es_valido(archivo_instrucciones)) {
		print_mensaje_error("El proceso no es válido, FINALIZANDO...");
		finalizar(archivo_instrucciones, consola_config);
		return -1;
	}

	int socket_consola = crear_conexion_con_kernel(consola_config);

	if (socket_consola == -1) {
		print_mensaje_error("No se pudo conectar con el kernel");
		finalizar(archivo_instrucciones, consola_config);
		return -1;
	}

	if (!se_pudo_hacer_el_handshake(socket_consola)) {
		log_error(log_consola, "No se pudo realizar el handshake");
		finalizar(archivo_instrucciones, consola_config);
		return -1;
	}

	enviar_paquete(socket_consola,archivo_instrucciones, tam_proceso);

	if (!fue_confirmado_el_envio(socket_consola)) {
		print_mensaje_error(
				"No se recibió la confirmación esperada desde el kernel");
		finalizar(archivo_instrucciones, consola_config);
		return -1;
	}

	esperar_finalizacion_proceso(socket_consola);

	finalizar(archivo_instrucciones, consola_config);

	return 0;
}




void esperar_finalizacion_proceso(int socket_consola) {
	op_code cod;
	recv(socket_consola,&cod,sizeof(op_code),0);
	if (cod == FINALIZACION_PROCESO) {
		log_info(log_consola,"El proceso finalizó con exito :D");

	} else {
		log_error(log_consola,"El proceso se detuvo :C");
	}

}

bool fue_confirmado_el_envio(int socket_consola) {

	op_code cod;
	recv(socket_consola,&cod,sizeof(op_code),0);

	if (cod != CONFIRMACION) {
		log_error(log_consola,"NO SE RECIBIÓ BIEN EL PROCESO");
		return false;
	}
	log_info(log_consola,"El proceso llegó bien :D");
	return true;
}

void enviar_paquete(int socket, FILE* archivo, int tam_proceso) {

	int tamanio_instrucciones = cant_instrucciones*sizeof(ID_INSTRUCCION) //identificadores
			+ cant_parametros_leidos*sizeof(int); //parametros

	void* envio = malloc(tamanio_instrucciones);
	int desp = 0;
	op_code cod = 1;
	send(socket, &cod,sizeof(op_code),MSG_NOSIGNAL);

	log_debug(log_consola,"%i",tamanio_instrucciones);
	send(socket,&tamanio_instrucciones,sizeof(int),MSG_NOSIGNAL);

	int tam = tam_proceso;
	log_debug(log_consola,"%i",tam);
	send(socket,&tam,sizeof(int),MSG_NOSIGNAL);

	char* linea = malloc(15);
	char** instruccion_leida;
	ID_INSTRUCCION *id = malloc(sizeof(ID_INSTRUCCION));
	int *parametro = malloc(sizeof(int));
	size_t len = 0;
	fseek(archivo,0,SEEK_SET);
	ssize_t read;
	while ((read = getline(&linea, &len, archivo)) != -1) {
		instruccion_leida = string_split(linea, " ");

		*id = get_id(instruccion_leida[0]);
		if (*id == NO_OP) {

			for(int i = 0; i < atoi(instruccion_leida[1]); i++) {

				memcpy(envio + desp, id,sizeof(ID_INSTRUCCION));
				desp += sizeof(ID_INSTRUCCION);
			}

		} else {

			memcpy(envio + desp, id,sizeof(ID_INSTRUCCION));
			desp += sizeof(ID_INSTRUCCION);

			for(int i = 0; i < cantidad_de_parametros(*id); i++) {
				*parametro = atoi(instruccion_leida[i+1]);
				memcpy(envio + desp,parametro,sizeof(int));
				desp += sizeof(int);
			}
		}

		string_array_destroy(instruccion_leida);
	}

	send(socket,envio,tamanio_instrucciones,MSG_NOSIGNAL);
	mostrar_envio(envio,tamanio_instrucciones);
	free(id);
	free(parametro);
	free(envio);
	free(linea);
}

void mostrar_envio(void* envio, int tam) {

	ID_INSTRUCCION *id = malloc(sizeof(ID_INSTRUCCION));
	int *parametro = malloc(sizeof(int));
	int desp = 0;

	while(desp < tam) {

		memcpy(id,envio + desp,sizeof(ID_INSTRUCCION));
		log_debug(log_consola,"ID: %i",*id);
		desp += sizeof(ID_INSTRUCCION);

		for(int i = 0; i < cantidad_de_parametros(*id); i++) {

			memcpy(parametro,envio + desp,sizeof(int));
			desp += sizeof(int);
			log_debug(log_consola,"PARAMETRO: %i",*parametro);
		}
	}
	free(id);
	free(parametro);


}

bool se_pudo_hacer_el_handshake(int socket) {

	op_code cod_respuesta = HANDSHAKE;
	int mensaje = MENSAJE_HANDSHAKE_ENVIADO;
	send(socket,&cod_respuesta,sizeof(op_code),MSG_NOSIGNAL);
	send(socket,&mensaje,sizeof(int),MSG_NOSIGNAL);

	recibir_confirmacion(&cod_respuesta, socket);

	return cod_respuesta == CONFIRMACION;
}

void recibir_confirmacion(op_code* cod_respuesta, int socket) {
	recv(socket, cod_respuesta, sizeof(op_code), 0);
}

void inicializar_log_consola() {

	log_consola = log_create("consola.log", "CONSOLA", 1, LOG_LEVEL_DEBUG);

	cant_parametros_leidos = 0;
	cant_instrucciones = 0;

	log_info(log_consola,
			"	  _   _   _   _   _   _   _     _   _   _   _   _   _   _   _  ");
	log_info(log_consola,
			"	 / \\ / \\ / \\ / \\ / \\ / \\ / \\   / \\ / \\ / \\ / \\ / \\ / \\ / \\ / \\ ");
	log_info(log_consola,
			"	( C | O | N | S | O | L | A ) ( I | N | I | C | I | A | D | A )");
	log_info(log_consola,
			"	 \\_/ \\_/ \\_/ \\_/ \\_/ \\_/ \\_/   \\_/ \\_/ \\_/ \\_/ \\_/ \\_/ \\_/ \\_/ ");
}

void destruir_log_consola() {
	print_mensaje_info("CONSOLA FINALIZADA");
	log_destroy(log_consola);
}

void print_mensaje_info(char* mensaje) {
	log_info(log_consola, "%s", mensaje);
}

void print_mensaje_error(char* mensaje) {
	log_error(log_consola, "%s", mensaje);
}

bool el_proceso_es_valido(FILE* instrucciones) {

	char* linea = malloc(16);
	char** instruccion_leida;
	ID_INSTRUCCION id = EXIT;
	size_t len = 0;
	fseek(instrucciones,0,SEEK_SET);
	ssize_t read;
	while ((read = getline(&linea, &len, instrucciones)) != -1) {
		cant_instrucciones++;
		instruccion_leida = string_split(linea, " ");

		if(!es_una_instruccion_valida(instruccion_leida)) {
			log_error(log_consola,"La instruccion %i no es valida",cant_instrucciones);
			free(linea);
			return false;
		}

		id = get_id(instruccion_leida[0]);

		if (id == NO_OP) {
			cant_instrucciones += atoi(instruccion_leida[1]) -1;
			cant_parametros_leidos -= 1;
		}

		string_array_destroy(instruccion_leida);
	}

	if (id != EXIT) {
		log_error(log_consola, "EL proceso no termina con EXIT");
		free(linea);
	}
	free(linea);
	return true;

}

bool es_una_instruccion_valida(char** instruccion) {

	if (es_valido(instruccion[0])) {

		if (tiene_los_parametros_correctos(instruccion)) {

			return true;
		}

		print_mensaje_error("Faltan parametros");

		return false;
	}

	log_error(log_consola, "EL identificador %s es desconocido",
			instruccion[0]);

	return false;
}

bool es_valido(char* identificador) {
	return string_contains(IDENTIFICADORES_VALIDOS, identificador);
}

bool tiene_los_parametros_correctos(char** instruccion) {
	int cantidad_de_parametros = string_array_size(instruccion) - 1;
	cant_parametros_leidos += cantidad_de_parametros;
	return cantidad_de_parametros == parametros_segun_id(instruccion[0]);
}

int parametros_segun_id(char* identificador) {

	ID_INSTRUCCION id = get_id(identificador);

	if(id == NO_OP) {
		return 1;
	}
	return cantidad_de_parametros(id);
}

int cantidad_de_parametros(ID_INSTRUCCION identificador) {

	switch (identificador) {

		case IO:
			return 1;
			break;
		case NO_OP:
			return 0;
			break;
		case READ:
			return 1;
			break;
		case EXIT:
			return 0;
			break;
		case COPY:
			return 2;
			break;
		case WRITE:
			return 2;
		}

		return 0;
}

ID_INSTRUCCION get_id(char* identificador) {

	ID_INSTRUCCION id;
	char** identificadores = string_split(IDENTIFICADORES_VALIDOS, ",");
	for (int i = 0; i < string_array_size(identificadores); i++) {
		if (string_equals_ignore_case(identificadores[i], identificador)) {
			id = i;
		}
	}
	string_array_destroy(identificadores);
	return id;
}

int crear_conexion_con_kernel(t_config* config) {

		log_debug(log_consola,"Creando conexion con kernel");

		//log_info(log_consola, "Lei la IP: %s y el PUERTO: %s", ip, puerto);
		char* puerto = config_get_string_value(config, CLAVE_PUERTO);
		char* ip = config_get_string_value(config, CLAVE_IP);

		int conexion = crear_conexion(ip, puerto);
		log_info(log_consola, "Conexion creada: %i", conexion);
		return conexion;
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

void finalizar(FILE* archivo_instrucciones,t_config* consola_config) {
	destruir_log_consola();
	config_destroy(consola_config);
	fclose(archivo_instrucciones);
}
