#include "memoria.h"
#include "../kernel/utils.c"
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>

int main(void) {
	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);

	int server_fd = iniciar_servidor();
	log_info(logger, "Modulo MEMORIA listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);

	t_list* lista;
	while(1){
		int tamanioPcb = deserializarInt(cliente_fd);
		int idPcb = deserializarInt(cliente_fd);
		char* estado = deserializarString(cliente_fd);

		enviarIntSerializado(1, cliente_fd);
	}
	return EXIT_SUCCESS;
}
