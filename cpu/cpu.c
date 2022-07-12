#include<cpu.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>

int main(void) {
	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);

	int server_fd = iniciar_servidor();
	log_info(logger, "Modulo CPU listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);

	t_list* lista;

	return EXIT_SUCCESS;
}

