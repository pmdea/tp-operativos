/*
 * consola.c
 *
 *  Created on: 22 jul. 2022
 *      Author: utnso
 */
#include "../src/consola.h"

#define IDENTIFICADORES_VALIDOS "I/O,READ,WRITE,COPY,EXIT,NO_OP"
#define CLAVE_IP "IP_KERNEL"
#define CLAVE_PUERTO "PUERTO_KERNEL"
#define MENSAJE_HANDSHAKE_ENVIADO "HOLA DON PEPITO"

void inicializar_log_consola();
void destruir_log_consola();
void salto_linea();
void print_mensaje_info(char* mensaje);
void print_mensaje_error(char* mensaje);

bool el_proceso_es_valido(FILE* instrucciones, t_queue* instrucciones_parseadas);
bool es_una_instruccion_valida(char** instruccion);
bool tiene_los_parametros_correctos(char** instruccion);
bool es_valido(char* identificador);
int parametros_segun_id(char* identificador);
int cantidad_de_parametros(ID_INSTRUCCION identificador);
ID_INSTRUCCION get_id(char* identificador);
void agregar_instruccion(t_queue* instrucciones, char** instruccion);
void agregar_parametros(t_instruccion* instruccion_validada, char** instruccion);
int crear_conexion_con_kernel(t_config* config);
int crear_conexion(char* ip, char* puerto);
bool se_pudo_hacer_el_handshake(int socket);
void enviar_mensaje(op_code* codigo, char* mensaje, int socket);
void recibir_confirmacion(op_code* cod_respuesta, int socket);
t_paquete* serializar_proceso(t_queue* instrucciones, op_code codigo,
		int tamanio_proceso);
int parametros_totales(t_queue* instrucciones);
int sumar(int num1, int num2);
int params(t_instruccion* instruccion);
void agregar_contenido(t_queue* instrucciones, void* buffer, int tamanio_proceso);
bool fue_confirmado_el_envio(int socket_consola);
void enviar_paquete(t_paquete* paquete, int socket);
void esperar_finalizacion_proceso(int socket_consola);
void finalizar(t_queue* instrucciones_parseadas, t_config* consola_config);

//FUNCIONES PRUEBA
void prueba_identificadores();
void prueba_instrucciones_validas();
void probar_agregar();
void show_cola(t_queue* cola);
void show_parametros_leidos(t_list* parametros);
void enviar_paquete_prueba();
void show_paquete(t_paquete* envio);
void deserializar_paquete(t_paquete* paquete);
void mostrar_instruccion(t_instruccion* instruccion);

t_log* log_consola;

int main(int argc, char** argv) {

	inicializar_log_consola();

	//prueba_identificadores();
	//prueba_instrucciones_validas();
	//probar_agregar();

	if (argc < 3) {
		print_mensaje_error("No se pasaron los parámetros necesarios");
		destruir_log_consola();
		return -1;
	}

	FILE* archivo_instrucciones = fopen(argv[1], "r");

	if (archivo_instrucciones == NULL) {
		print_mensaje_error("El archivo en la ruta especificada no existe");
		destruir_log_consola();
		return -1;
	}
	log_info(log_consola,
			"El archivo en la ruta \"%s\" fue abierto correctamente", argv[1]);

	t_queue* instrucciones_parseadas = queue_create();

	t_config* consola_config = config_create("./src/consola.config");

	if (!el_proceso_es_valido(archivo_instrucciones, instrucciones_parseadas)) {
		print_mensaje_error("El proceso no es válido, FINALIZANDO...");
		finalizar(instrucciones_parseadas, consola_config);
		return -1;
	}

	//show_cola(instrucciones_parseadas);

	int socket_consola = crear_conexion_con_kernel(consola_config);

	if (socket_consola == -1) {
		print_mensaje_error("No se pudo conectar con el kernel");
		finalizar(instrucciones_parseadas, consola_config);
		return -1;
	}

	if (!se_pudo_hacer_el_handshake(socket_consola)) {
		log_error(log_consola, "No se pudo realizar el handshake");
		finalizar(instrucciones_parseadas, consola_config);
		return -1;
	}

	t_paquete* paquete = serializar_proceso(instrucciones_parseadas,
			ENVIO_PROCESO, atoi(argv[2]));

	enviar_paquete(paquete, socket_consola);

	//deserializar_paquete(paquete);

	if (!fue_confirmado_el_envio(socket_consola)) {
		//Mensaje no se pudo enviar paquete
		print_mensaje_error(
				"No se recibió la confirmación esperada desde el kernel");
		finalizar(instrucciones_parseadas, consola_config);
		return -1;
	}

	esperar_finalizacion_proceso(socket_consola);

	finalizar(instrucciones_parseadas, consola_config);

	return 0;
}

void esperar_finalizacion_proceso(int socket_consola) {
	op_code *cod = malloc(sizeof(op_code));
	recv(socket_consola,cod,sizeof(op_code),0);
	if (*cod == FINALIZACION_PROCESO) {
		log_info(log_consola,"El proceso finalizó con exito :D");

	} else {
		log_error(log_consola,"El proceso se detuvo :C");
	}

	free(cod);
}

bool fue_confirmado_el_envio(int socket_consola) {

	op_code *cod = malloc(sizeof(op_code));
	recv(socket_consola,cod,sizeof(op_code),0);

	if (*cod != CONFIRMACION) {
		free(cod);
		log_error(log_consola,"NO SE RECIBIÓ BIEN EL PROCESO");
		return false;
	}
	free(cod);
	log_info(log_consola,"El proceso llegó bien :D");
	return true;
}

void enviar_paquete(t_paquete* paquete, int socket) {

	op_code *cod = malloc(sizeof(op_code));
	*cod = paquete->operacion;
	int *tamanio = malloc(sizeof(int));
	*tamanio = paquete->buffer->size;

	send(socket,cod,sizeof(op_code),0);
	send(socket,tamanio,sizeof(int),0);
	send(socket,paquete->buffer->stream,*tamanio,0);

	free(cod);
	free(tamanio);
	free(paquete->buffer);
	free(paquete);
}

void deserializar_paquete(t_paquete* paquete) {

	t_queue* inst = queue_create();
	void* recibido = malloc(paquete->buffer->size);
	memcpy(recibido,paquete->buffer->stream,paquete->buffer->size);
	int *desp = malloc(sizeof(int));
	*desp = 0;
	ID_INSTRUCCION *id = malloc(sizeof(ID_INSTRUCCION));
	int *tamanio_proceso = malloc(sizeof(int));
	t_queue* parametros = queue_create();
	while(*desp < paquete->buffer->size - sizeof(int)) {
		memcpy(id,recibido + *desp,sizeof(ID_INSTRUCCION));
		*desp += sizeof(ID_INSTRUCCION);

		for(int i = 0; i < cantidad_de_parametros(*id);i++) {
			int *parametro = malloc(sizeof(int));
			memcpy(parametro,recibido + *desp,sizeof(int));
			*desp += sizeof(int);
			queue_push(parametros,(void*) *parametro);
		}
		t_instruccion* instruccion = malloc(sizeof(t_instruccion));
		instruccion->identificador = *id;
		instruccion->parametros = parametros;
		queue_push(inst,instruccion);
	}
	memcpy(tamanio_proceso,recibido + *desp,sizeof(int));
	list_map(inst->elements,(void*) mostrar_instruccion);
	log_debug(log_consola,"TAMAÑO PROCESO: %i",*tamanio_proceso);
}

void mostrar_instruccion(t_instruccion* instruccion) {

	log_debug(log_consola,"ID: %i",(instruccion->identificador));
	for(int i = 0; i < cantidad_de_parametros(instruccion->identificador); i++) {
		log_debug(log_consola,"PARAMETRO: %i",(int)queue_peek(instruccion->parametros));
		queue_pop(instruccion->parametros);
	}
}

t_paquete* serializar_proceso(t_queue* instrucciones, op_code codigo,
		int tamanio_proceso) {

	t_paquete* paquete_serializado = malloc(sizeof(t_paquete));

	paquete_serializado->operacion = codigo;

	paquete_serializado->buffer = malloc(sizeof(t_buffer));

	paquete_serializado->buffer->size = sizeof(op_code)
			+ sizeof(ID_INSTRUCCION)*queue_size(instrucciones)
			+ sizeof(int)*parametros_totales(instrucciones);

	void* buffer = malloc(paquete_serializado->buffer->size);

	agregar_contenido(instrucciones,buffer,tamanio_proceso);
	paquete_serializado->buffer->stream = buffer;
	return paquete_serializado;
}

void agregar_contenido(t_queue* instrucciones, void* buffer, int tamanio_proceso) {

	t_instruccion* instruccion = malloc(sizeof(t_instruccion));
	int *desp = malloc(sizeof(int));
	*desp = 0;
	int *parametro = malloc(sizeof(int));
	ID_INSTRUCCION *id = malloc(sizeof(ID_INSTRUCCION));
	int *tp = malloc(sizeof(int));
	*tp = tamanio_proceso;
	while(!queue_is_empty(instrucciones)) {

		instruccion = queue_peek(instrucciones);

		*id = instruccion->identificador;

		memcpy(buffer + *desp,id,sizeof(ID_INSTRUCCION));
		*desp += sizeof(ID_INSTRUCCION);

		t_queue* parametros = queue_create();
		parametros = instruccion->parametros;

		for(int i = 0; i < cantidad_de_parametros(instruccion->identificador); i++) {

			*parametro = (int) queue_peek(parametros);

			memcpy(buffer + *desp,parametro,sizeof(int));
			*desp += sizeof(int);

			queue_pop(parametros);
		}

		queue_pop(instrucciones);
	}
	memcpy(buffer + *desp,tp,sizeof(int));
	free(tp);
	free(instruccion->parametros);
	free(instruccion);
	free(desp);
}

int parametros_totales(t_queue* instrucciones) {

	return (int) list_fold1(list_map(instrucciones->elements,(void*) params),(void*) sumar);

}

int params(t_instruccion* instruccion) {
	return queue_size(instruccion->parametros);
}

int sumar(int num1, int num2) {

	return num1 + num2;
}


bool se_pudo_hacer_el_handshake(int socket) {

	op_code *cod_respuesta = malloc(sizeof(op_code));
	*cod_respuesta = HANDSHAKE;
	// enviar mensaje handhsake
	enviar_mensaje(cod_respuesta, MENSAJE_HANDSHAKE_ENVIADO, socket);
	//recibir mensaje
	recibir_confirmacion(cod_respuesta, socket);

	if (*cod_respuesta != CONFIRMACION) {

		print_mensaje_error("No se recibió la confirmación del kernel");
		free(cod_respuesta);
		return false;
	}

	free(cod_respuesta);
	return true;
}

void enviar_mensaje(op_code* codigo, char* mensaje, int socket) {

	int *tamanio_mensaje = malloc(sizeof(int));

	*tamanio_mensaje = string_length(mensaje);

	int *tamanio_total = malloc(sizeof(int));

	*tamanio_total = *tamanio_mensaje + sizeof(op_code);

	void* envio = malloc(*tamanio_total);

	memcpy(envio, codigo, sizeof(op_code));
	memcpy(envio + sizeof(op_code), mensaje, *tamanio_mensaje);

	send(socket, (void*) tamanio_total, sizeof(int), 0);
	send(socket, envio, *tamanio_total, 0);

	free(tamanio_mensaje);
	free(tamanio_total);
	free(envio);
}

void recibir_confirmacion(op_code* cod_respuesta, int socket) {
	recv(socket, cod_respuesta, sizeof(op_code), 0);
}

void inicializar_log_consola() {
	log_consola = log_create("consola.log", "CONSOLA", 1, LOG_LEVEL_DEBUG);

	log_info(log_consola,
			"	  _   _   _   _   _   _   _     _   _   _   _   _   _   _   _  ");
	log_info(log_consola,
			"	 / \\ / \\ / \\ / \\ / \\ / \\ / \\   / \\ / \\ / \\ / \\ / \\ / \\ / \\ / \\ ");
	log_info(log_consola,
			"	( C | O | N | S | O | L | A ) ( I | N | I | C | I | A | D | A )");
	log_info(log_consola,
			"	 \\_/ \\_/ \\_/ \\_/ \\_/ \\_/ \\_/   \\_/ \\_/ \\_/ \\_/ \\_/ \\_/ \\_/ \\_/ ");
	salto_linea();
}

void destruir_log_consola() {
	print_mensaje_info("CONSOLA FINALIZADA");
	log_destroy(log_consola);
}

void salto_linea() {
	log_info(log_consola, "\n");
}

void print_mensaje_info(char* mensaje) {
	log_info(log_consola, "%s", mensaje);
}

void print_mensaje_error(char* mensaje) {
	log_error(log_consola, "%s", mensaje);
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
		instruccion = malloc(
				string_array_size(string_split(linea, " ")) * sizeof(char*));
		instruccion = string_split(linea, " ");
		//VALIDAR INSTRUCCION
		if (!es_una_instruccion_valida(instruccion)) {
			log_error(log_consola,
					"La instruccion en la linea %i no es correcta.",
					lineas_leidas);
			log_error(log_consola,
					"No se puede enviar datos incompletos. Finalizando...");

			return false;
		}

		agregar_instruccion(instrucciones_parseadas, instruccion);

		//log_debug(log_consola, "La linea %i fue leida con la instruccion %s de parametro: %sy fue agregada con exito",
		//lineas_leidas, instruccion[0],show_parametros(instruccion));
		strcpy(ultimo_identificador, instruccion[0]);
	}
//show_contenido(instrucciones_parseadas);
//VALIDAR QUE TERMINE CON EXIT
	if (EXIT != get_id(ultimo_identificador)) {
		print_mensaje_error(
				"El proceso no termina con una instruccion de EXIT");
		free(ultimo_identificador);
		print_mensaje_error("No se puede enviar datos incompletos.");
		return false;
	}
	log_info(log_consola, "Las %i instrucciones fueron validadas y guardadas",
			lineas_leidas);
//show_contenido(instrucciones_parseadas);
	string_array_destroy(instruccion);
	return true;
//FIN VALIDAR QUE TERMINE CON EXIT
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

	return cantidad_de_parametros == parametros_segun_id(instruccion[0]);
}

int parametros_segun_id(char* identificador) {

	ID_INSTRUCCION id = get_id(identificador);

	return cantidad_de_parametros(id);
}

int cantidad_de_parametros(ID_INSTRUCCION identificador) {

	switch (identificador) {

		case IO:
			return 1;
			break;
		case NO_OP:
			return 1;
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
	char** identificadores = string_array_new();
	identificadores = string_split(IDENTIFICADORES_VALIDOS, ",");
	for (int i = 0; i < string_array_size(identificadores); i++) {
		if (string_equals_ignore_case(identificadores[i], identificador)) {
			id = i;
		}
	}
	return id;
}

void agregar_instruccion(t_queue* instrucciones, char** instruccion) {

	t_instruccion* instruccion_validada = malloc(sizeof(t_instruccion));

	instruccion_validada->identificador = get_id(instruccion[0]);
	instruccion_validada->parametros = queue_create();

	agregar_parametros(instruccion_validada, instruccion);
	queue_push(instrucciones, instruccion_validada);
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

		log_info(log_consola, "Lei la IP: %s y el PUERTO: %s", ip, puerto);

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

void agregar_parametros(t_instruccion* instruccion_validada, char** instruccion) {

	int contador = 0;
	while (contador <= string_array_size(instruccion) - 2
			&& instruccion[++contador] != NULL) {
		log_debug(log_consola, "meti este valor como parametro nro %i: %i",
				contador, (int) atoi(instruccion[contador]));
		queue_push(instruccion_validada->parametros,
				(void*) atoi(instruccion[contador]));
	}
}

void finalizar(t_queue* instrucciones_parseadas, t_config* consola_config) {
	queue_destroy_and_destroy_elements(instrucciones_parseadas, (void*) free);
	config_destroy(consola_config);
}

void enviar_paquete_prueba(int socket) {
	op_code codigo = HANDSHAKE;
	char* mensaje = "HOLA 32 COMO ESTAS";

	t_paquete* envio = malloc(sizeof(t_paquete));
	envio->operacion = codigo;
	envio->buffer = malloc(sizeof(t_buffer));
	envio->buffer->size = string_length(mensaje) + 1 + sizeof(int);
	envio->buffer->stream = malloc(envio->buffer->size);

	int desplazamiento = 0;
	int *tamanio = malloc(sizeof(int));
	*tamanio = string_length(mensaje) + 1;
	memcpy((envio->buffer->stream) + desplazamiento, tamanio, sizeof(int));
	desplazamiento += sizeof(int);
	memcpy((envio->buffer->stream) + desplazamiento, mensaje, *tamanio);
	free(tamanio);
	show_paquete(envio);
//int a = enviar_paquete(envio,socket);
//log_info(log_consola,"envio: %i",a);
}

void show_paquete(t_paquete* envio) {

	int tamanio = 0;
	memcpy(&tamanio, &(envio->buffer->size), sizeof(int));
	void* mensaje = malloc(tamanio);
	memcpy(mensaje, envio->buffer->stream, tamanio);
	memcpy(&tamanio, &(envio->buffer->size), sizeof(int));
	log_debug(log_consola, "codigo envio: %i", envio->operacion);
	log_debug(log_consola, "mensaje: %s", (char*) mensaje);
}
/*
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
 */

// FUNCIONES DE PRUEBA
void prueba_identificadores() {

	log_debug(log_consola, "Codigo: %s, Identificador: %i", "NO_OP",
			get_id("NO_OP"));
	log_debug(log_consola, "cantidad de parametros: %i, %s",
			parametros_segun_id("NO_OP"), "NO_OP");

	log_debug(log_consola, "Codigo: %s, Identificador: %i", "COPY",
			get_id("COPY"));
	log_debug(log_consola, "cantidad de parametros: %i, %s",
			parametros_segun_id("COPY"), "COPY");

	log_debug(log_consola, "Codigo: %s, Identificador: %i", "I/O",
			get_id("I/O"));
	log_debug(log_consola, "cantidad de parametros: %i, %s",
			parametros_segun_id("I/O"), "I/O");

	log_debug(log_consola, "Codigo: %s, Identificador: %i", "READ",
			get_id("READ"));
	log_debug(log_consola, "cantidad de parametros: %i, %s",
			parametros_segun_id("READ"), "READ");

	log_debug(log_consola, "Codigo: %s, Identificador: %i", "WRITE",
			get_id("WRITE"));
	log_debug(log_consola, "cantidad de parametros: %i, %s",
			parametros_segun_id("WRITE"), "WRITE");

	log_debug(log_consola, "Codigo: %s, Identificador: %i", "EXIT",
			get_id("EXIT"));
	log_debug(log_consola, "cantidad de parametros: %i, %s",
			parametros_segun_id("EXIT"), "EXIT");
	salto_linea();
}

void prueba_instrucciones_validas() {

	log_debug(log_consola, "es valido: %s , %i", "NO_OP", es_valido("NO_OP"));
	log_debug(log_consola, "es valido: %s , %i", "EXIT", es_valido("EXIT"));
	log_debug(log_consola, "es valido: %s , %i", "READ", es_valido("READ"));
	log_debug(log_consola, "es valido: %s , %i", "WRITE", es_valido("WRITE"));
	log_debug(log_consola, "es valido: %s , %i", "COPY", es_valido("COPY"));
	log_debug(log_consola, "es valido: %s , %i", "I/O", es_valido("I/O"));

	log_debug(log_consola, "es valido: %s , %i", "NO_PO", es_valido("NO_PO"));
	salto_linea();
}

void probar_agregar() {

	t_queue* cola = queue_create();

	char** instruccion_1p = string_array_new();
	instruccion_1p = string_split("NO_OP,3", ",");

	char** instruccion_0p = string_array_new();
	instruccion_0p = string_split("EXIT", ",");

	char** instruccion_2p = string_array_new();
	instruccion_2p = string_split("COPY,42,125", ",");

	agregar_instruccion(cola, instruccion_0p);
	agregar_instruccion(cola, instruccion_1p);
	agregar_instruccion(cola, instruccion_2p);

	show_cola(cola);
}

void show_cola(t_queue* cola) {

	t_queue* aux = malloc(sizeof(t_queue));
	t_instruccion* instruccion_actual = malloc(sizeof(t_instruccion));

	aux->elements = list_duplicate(cola->elements);
	while (!queue_is_empty(aux)) {

		instruccion_actual = queue_peek(aux);

		log_debug(log_consola, "ID: %i", instruccion_actual->identificador);

		show_parametros_leidos(
				list_duplicate(instruccion_actual->parametros->elements));

		queue_pop(aux);
	}

	queue_destroy(aux);
}

void show_parametros_leidos(t_list* parametros) {
	t_queue* aux = malloc(sizeof(t_queue));
	aux->elements = parametros;
	int parametro = 0;
	while (!queue_is_empty(aux)) {
		parametro = (int) queue_peek(aux);
		log_info(log_consola, "Parametro: %i", parametro);
		queue_pop(aux);
	}
}

