#include "cpu.h"
#include "../kernel/utils.c"
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include "../kernel/kernel.h"

int main(void) {
	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);

	int server_fd = iniciar_servidor();
	log_info(logger, "Modulo CPU listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);

	t_list* lista;
	int i = 0;
	while(1){
			pcb* hola = deserializarPCB(cliente_fd);
			if((hola->id == 0 || hola->id ==1) && i < 2){
					i++;
					enviar_respuesta_kernel(cliente_fd, hola,10,"I/0",5000,logger);
			}else{
					i++;
				if(i == 3 || i == 4){
					enviar_respuesta_kernel(cliente_fd, hola,10,"EXIT",5000,logger);
				}
			}
			if(hola->id == 2){
				enviar_respuesta_kernel(cliente_fd, hola,10,"EXIT",5000,logger);
			}
		}
	return EXIT_SUCCESS;
}

