/*
 * consola.c
 *
 *  Created on: 27 may. 2022
 *      Author: utnso
 */


#include "consola.h"

bool el_proceso_es_valido(FILE* instrucciones, t_queue* instrucciones_parseadas);
char* quitar_eol(char* identificador);
bool es_una_instruccion_valida(char** instruccion);
void inicializar_identificadores_validos(t_list* identificadores_validos);
bool esta_en_la_lista(t_list* lista, void* elemento);
bool tiene_los_parametros_correctos(char** instruccion);
bool es_igual_a(void* un_string, void* otro_string);
int cantidad_de_parametros(char* identificador);
void agregar_instruccion(t_queue* instrucciones, char** instruccion);
void agregar_parametros(t_instruccion* instruccion_validada, char** instruccion);
int crear_conexion_con_kernel(t_config* config);
int crear_conexion(char* ip, char* puerto);
bool se_pudo_hacer_el_handshake(int socket_consola);
bool enviar_mensaje(op_code codigo, char* mensaje, int socket_consola);
t_paquete* serializar_paquete(t_queue* contenido, op_code codigo,
		int tamanio_proceso);
int parametros_totales(t_queue* contenido);
int parametros(t_instruccion* instruccion);
int sumar(int un_numero, int otro_numero);
int tamanio_identificadores(t_queue* instrucciones);
int tamanio_identificador(t_instruccion* instruccion);
void agregar_contenido(t_queue* contenido, void* buffer, int tamanio);
void copiar_parametros(void* buffer, int *desplazamiento, t_queue* parametros);
void enviar_paquete(t_paquete* paquete, int socket_consola);
void eliminar_paquete(t_paquete* paquete);
void esperar_confirmacion(int socket_consola);


void finalizar_consola(FILE* archivo, t_config* config, t_queue* instrucciones);

char* show_parametros(char** instruccion);
void show_contenido(t_queue* contenido);
void show_paquete(t_paquete* paquete);
void deserializar_stream(t_paquete* paquete);
void deserializar_datos(t_proceso* proceso, t_paquete* stream);
void show_proceso(t_proceso* proceso);
void show_instrucciones(t_queue* instrucciones);
void show_parametros_leidos(t_list* parametros);

#define CLAVE_IP "IP_KERNEL"
#define CLAVE_PUERTO "PUERTO_KERNEL"
#define UNSIGNED_INT uint32_t
#define CONFIRM_ESPERADA "RECIBIDO :)"
#define MENSAJE_HANDSHAKE_ENVIADO "HOLA DON PEPITO"
#define MENSAJE_HANDSHAKE_ESPERADO "HOLA DON JOSE"

t_log* log_consola;

int main(int argc, char** argv) {
	//INICIALIZAR LOG
		log_consola = log_create("consola.log", "CONSOLA", 1, LOG_LEVEL_DEBUG);
		log_info(log_consola,"Hola, soy el log de la consola");
		log_debug(log_consola,"recibi los parametros: %s %i",argv[1],atoi(argv[2]));
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

				t_queue* instrucciones_parseadas = queue_create();
				if(el_proceso_es_valido(instrucciones, instrucciones_parseadas)) {
					//mostrar_contenido(instrucciones_parseadas);
					t_config* consola_config = config_create("./consola-v2/consola.config");
					log_debug(log_consola,"EL IP del config es: %s", config_get_string_value(consola_config, CLAVE_IP));
					log_debug(log_consola,"El puerto del config es: %s", config_get_string_value(consola_config, CLAVE_PUERTO));
					//int socket_consola = crear_conexion_con_kernel(consola_config);
					if (/*socket_consola != -1 && se_pudo_hacer_el_handshake(socket_consola)*/true) {
						t_paquete* paquete = serializar_paquete(instrucciones_parseadas, ENVIO_DATOS, atoi(argv[2]));
						//enviar_paquete(paquete, socket_consola);
						//esperar_confirmacion(socket_consola);
						config_destroy(consola_config);
						log_debug(log_consola,"codigo operacion del paquete: %i",paquete->operacion);
						log_debug(log_consola,"tamaño datos enviados: %i",paquete->buffer->size);
						log_debug(log_consola,"todo el buffer de nuevo por las dudas jaja: %s",(char*) (paquete->buffer->buffer));
						deserializar_stream(paquete);
						eliminar_paquete(paquete);
					}
					fclose(instrucciones);
				}
			} else {
				log_error(log_consola,"el archivo en la direccion %s no existe.",argv[1]);
			}
		}
		log_info(log_consola,"Consola finalizada");
		log_destroy(log_consola);

		return 0;
}

bool el_proceso_es_valido(FILE* instrucciones, t_queue* instrucciones_parseadas) {

	char* linea = string_new();
	int lineas_leidas = 0;
	char** instruccion;
	char* ultimo_identificador = string_new();

	while (fgets(linea, 15, instrucciones) != NULL) {
		lineas_leidas++;
		instruccion = malloc(string_array_size(string_split(linea," "))*sizeof(char*));

		instruccion = string_split(quitar_eol(linea)," ");
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
}

char* quitar_eol(char* string) {
	if(string_ends_with(string,"\n")) {
		return string_substring_until(string,string_length(string) -1);
	}
	return string;
}

bool es_una_instruccion_valida(char** instruccion) {

	t_list* identificadores_validos = list_create();
	inicializar_identificadores_validos(identificadores_validos);

	if (esta_en_la_lista(identificadores_validos, instruccion[0])) {

		if (tiene_los_parametros_correctos(instruccion)) {
			//list_destroy_and_destroy_elements(identificadores_validos,free);
			return true;
		} else {
			log_error(log_consola,"Faltan parametros");
		}
	} else {
		log_error(log_consola,"EL identificador %s es desconocido",instruccion[0]);
	}
	//list_destroy_and_destroy_elements(identificadores_validos,free);
	return false;
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
	return strcmp(un_string, otro_string) == 0;
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

		log_info(log_consola, "Lei la IP: %i y el PUERTO: %s", *ip, puerto);

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
	if (enviar_mensaje(HANDSHAKE, MENSAJE_HANDSHAKE_ENVIADO, socket_consola)) {
		t_mensaje* recibido = malloc(sizeof(t_mensaje));

		if (recv(socket_consola, &recibido->operacion, sizeof(op_code), 0) == -1
				|| recibido->operacion != HANDSHAKE) {
			log_error(log_consola,
					"No se pudo realizar el handshake, recibida operacion incorrecta");
			free(recibido);
			return false;
		} else {
			recv(socket_consola, &recibido->tamanio_mensaje, sizeof(int), 0);
			recv(socket_consola, &recibido->mensaje, recibido->tamanio_mensaje,
					0);
			if (strcmp(MENSAJE_HANDSHAKE_ESPERADO, recibido->mensaje) != 0) {
				log_error(log_consola,
						"El mensaje de handshake recibido es distinto del esperado");
				free(recibido);
				return false;
			}
		}
		free(recibido);
	}
	log_info(log_consola, "El handshake fue exitoso");

	return true;
}

bool enviar_mensaje(op_code codigo, char* mensaje, int socket_consola) {
	t_mensaje* envio = malloc(sizeof(t_mensaje));
	envio->operacion = codigo;
	envio->tamanio_mensaje = string_length(mensaje);
	envio->mensaje = mensaje;
	if (send(socket_consola, envio,
			envio->tamanio_mensaje + sizeof(envio->operacion)
					+ sizeof(envio->tamanio_mensaje), 0) == -1) {
		log_error(log_consola, "el envio del mensaje no pudo ser relizado");
		free(envio);
		return false;
	}
	log_info(log_consola, "El mensaje %i fue enviado con exito", codigo);
	free(envio);
	return true;
}

t_paquete* serializar_paquete(t_queue* contenido, op_code codigo,
		int tamanio_proceso) {
	t_paquete* paquete_serializado = malloc(sizeof(t_paquete));
	paquete_serializado->operacion = codigo;
	paquete_serializado->buffer = malloc(sizeof(t_buffer));
	log_debug(log_consola,"codigo: %i",paquete_serializado->operacion);
	paquete_serializado->buffer->size = sizeof(int)+queue_size(contenido)*sizeof(int) + tamanio_identificadores(contenido)*sizeof(char) + parametros_totales(contenido)*sizeof(UNSIGNED_INT);
	void* buffer = malloc(paquete_serializado->buffer->size);
	agregar_contenido(contenido,buffer,tamanio_proceso);
	paquete_serializado->buffer->buffer = buffer;
	return paquete_serializado;
}

int parametros_totales(t_queue* contenido) {
	t_list* tamanios = list_map(contenido->elements,(void*) parametros);
	log_debug(log_consola,"Parametros totales: %i",(int) list_fold1(tamanios,(void*) sumar));
	return (int) list_fold1(tamanios,(void*) sumar);
}

int parametros(t_instruccion* nodo) {
	return cantidad_de_parametros(nodo->identificador);
}

int sumar(int num1, int num2) {
	return num1 + num2;
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

void agregar_contenido(t_queue* contenido, void* buffer, int tamanio) {
	int desplazamiento = 0;
	memcpy(buffer, &tamanio, sizeof(int));
	desplazamiento += sizeof(int);
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

void enviar_paquete(t_paquete* paquete, int socket_consola) {
	send(socket_consola, paquete,
			paquete->buffer->size + 2*sizeof(int) + sizeof(op_code), 0);
	log_info(log_consola,
			"El paquete fue enviado exitosamente, esperando confirmacion de recepcion...");
	eliminar_paquete(paquete);
}

void eliminar_paquete(t_paquete* paquete) {
	free(paquete->buffer->buffer);
	free(paquete->buffer);
	free(paquete);
}

void esperar_confirmacion(int socket_consola) {
	// ESPERO RECIBIR "RECIBIDO :)", CUALQUIER OTRA COSA IMPLICA FALLO
	t_mensaje* recibido = malloc(sizeof(t_mensaje));
	if (recv(socket_consola, &recibido->operacion, sizeof(op_code), 0) == -1
			|| recv(socket_consola, &recibido->tamanio_mensaje, sizeof(int), 0)
					== -1
			|| recv(socket_consola, &recibido->mensaje,
					recibido->tamanio_mensaje, 0) == -1) {
		log_error(log_consola,
				"No se pudo recibir la confirmacion, error de conexion");
	} else if (recibido->operacion != CONFIRMACION
			&& es_igual_a(CONFIRM_ESPERADA,recibido->mensaje)) {
		log_error(log_consola, "Los datos no fueron correctamente recibidos");
	} else {
		log_info(log_consola, "confirmado el envio del paquete");
	}
	free(recibido);
}



void finalizar_consola(FILE* archivo, t_config* config, t_queue* instrucciones) {
	log_info(log_consola, "Cerrando archivos y liberando memoria...");

	//config_destroy(config);
	//queue_destroy_and_destroy_elements(instrucciones, free);
	log_info(log_consola, "Consola finalizada");
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

void deserializar_stream(t_paquete* paquete) {
	t_proceso* proceso = malloc(sizeof(t_proceso));
	proceso->instrucciones = queue_create();
	switch(paquete->operacion) {
	case ENVIO_DATOS:
		log_debug(log_consola,"Deserializando datos...");
		deserializar_datos(proceso,paquete); break;
	case CONFIRMACION:
		//deserializar_mensaje(paquete_recibido,paquete); break;
	case HANDSHAKE:
		//deserializar_handshake(paquete_recibido,paquete); break;
	default: log_error(log_consola,"La operacion ingresada es desconocida");
	}
	show_proceso(proceso);
}

void deserializar_datos(t_proceso* proceso, t_paquete* stream) {
	int desplazamiento = 0;
	t_instruccion* instruccion_actual = malloc(sizeof(t_instruccion));
	memcpy(&(proceso->tamanio_proceso),stream->buffer->buffer + desplazamiento,sizeof(int));
	log_debug(log_consola,"Tamaño del proceso: %i",proceso->tamanio_proceso);
	desplazamiento += sizeof(int);
	log_debug(log_consola,"Deserializando las instrucciones...");
	while(desplazamiento < stream->buffer->size) {

		memcpy(&(instruccion_actual->tamanio_id),stream->buffer->buffer + desplazamiento,sizeof(int));
		log_debug(log_consola,"grabe tamaño id: %i",instruccion_actual->tamanio_id);
		desplazamiento += sizeof(int);
		memcpy(&(instruccion_actual->identificador),stream->buffer->buffer + desplazamiento, instruccion_actual->tamanio_id);
		desplazamiento += instruccion_actual->tamanio_id;
		log_debug(log_consola,"grabe id: %s",instruccion_actual->identificador);
		log_debug(log_consola,"leyendo parametros...");
		int parametro_actual;
		instruccion_actual->parametros = queue_create();
		for(int i = 1; i <= cantidad_de_parametros(instruccion_actual->identificador); i++){
			memcpy(&parametro_actual,stream->buffer->buffer + desplazamiento,sizeof(int));
			log_debug(log_consola,"grabe param %i: %i",i,parametro_actual);
			desplazamiento += sizeof(int);
			queue_push(instruccion_actual->parametros, parametro_actual);
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
	char* algo = paquete->buffer->buffer;
	log_debug(log_consola,"stream: %s", algo);
}
