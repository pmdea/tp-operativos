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

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
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

	t_list* lista;
	char* valor;

	while (1) {
		int cod_op = recibir_operacion(socket_cliente);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(socket_cliente);
			break;
		case PAQUETE:
			lista = recibir_paquete(socket_cliente);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			for(int i=0; i<list_size(lista); i++)
			{
				valor = list_get(lista, i);
				log_info(logger, valor);
			}
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
