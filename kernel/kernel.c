#include "kernel.h"

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);
	log_info(logger, "Recibí el buffer");

	return buffer;
}

t_queue* recibir_buffer_cola(int* size, int socket_cliente)
{
	t_queue* buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);
	log_info(logger, "Recibí el buffer");

	return buffer;
}


void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list* queue_pope(t_queue* buffer)
{
	t_list* lista = list_remove(buffer->elements, 0);

	return lista;
}

t_queue* recibir_cola(int socket_cliente)
{
	int size;
	t_queue* buffer = recibir_buffer_cola(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje");
	return buffer;
	free(buffer);
}

t_queue* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
//	t_list* valores = list_create();
	int tamanio;
	t_queue* valor;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
//		list_add(valores, valor);
	}
	free(buffer);
	return valor;
}

// espera un cliente y lo acepta

int esperar_cliente(int socket_kernel)
{
	int socket_cliente = accept(socket_kernel, NULL, NULL);
	log_info(logger, "Se conecto un cliente");

	return socket_cliente;
}

// prepara el servidor

int conectar_kernel()
{
	int socket_kernel;

	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(IP, PUERTO, &hints, &servinfo);

	socket_kernel = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

	bind(socket_kernel, servinfo->ai_addr, servinfo->ai_addrlen);

	listen(socket_kernel, SOMAXCONN);

	freeaddrinfo(servinfo);
	log_trace(logger, "Listo para escuchar al cliente");

	return socket_kernel;
}

void iterador(char* value) {
	log_info(logger,"%s", value);
}

int main(void)
{
	logger = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_DEBUG);

	int socket_kernel = conectar_kernel();
	log_info(logger, "Servidor listo para recibir al cliente");
	int socket_cliente = esperar_cliente(socket_kernel);

	t_queue* cola;
	t_list* lista;
//	char* identificador;
//	int* paramA;
//	int* paramB;

	while (1) {
		int cod_op = recibir_operacion(socket_cliente);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(socket_cliente);
			break;
		case COLA:
			cola = recibir_cola(socket_cliente);
			log_info(logger, "Recibí la cola");
			while(!queue_is_empty(cola))
			{
				log_info(logger, "im in");
				lista = queue_pop(cola);
				log_info(logger, "popie");
			}
			break;
		case PAQUETE:
			recibir_paquete(socket_cliente);
			break;
		case -1:
			log_error(logger, "El cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida.");
			break;
		}
	}
	return EXIT_SUCCESS;
}
