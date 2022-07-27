#include "kernel.h"

//void avisar_a_memoria(socket_memoria, INICIALIZA, nuevoProceso, loggerKernel){
void avisar_a_memoria(OP_MEMORIA operacion, PCB unPCB, t_log* logger){
	int tamanioBuffer = sizeof(uint32_t)*4;
	void* buffer = asignarMemoria(tamanioBuffer);
	int desplazamiento = 0;
	concatenarInt32(buffer, &desplazamiento, operacion);
	concatenarInt32(buffer, &desplazamiento, unPCB . tamanio);
	concatenarInt32(buffer, &desplazamiento, unPCB . id);
	enviarMensaje(socket_memoria, buffer, tamanioBuffer);
	free(buffer);

	switch(operacion){
		case INICIALIZA:
			log_info(logger,"ENVIANDO AVISO A MEMORIA - NUEVO PROCESO ID %i...", unPCB . id);
			break;
		case SUSPENDE:
			log_info(logger,"ENVIANDO AVISO A MEMORIA - SUSPENSION PROCESO ID %i...", unPCB . id);
			break;
		case FINALIZA:
			log_info(logger,"ENVIANDO AVISO A MEMORIA - FINALIZA PROCESO ID %i...", unPCB . id);
			break;
	}
}
