#include "includeh/manejar_memoria.h"

extern void* memoria_principal;
extern t_log* logger;

char* escribir_en_memoria(uint32_t direc_fisica, void* data, uint32_t size){
	log_info(logger, "Escribiendo %d bytes en pos %d", size, direc_fisica);
	memcpy(memoria_principal + direc_fisica, data, size);
	return "OK"; //Según enunciado
}
void* leer_de_memoria(uint32_t start, uint32_t size){
	void* data = malloc(size);
	memcpy(data, memoria_principal + start, size);
	return data;
}

bool entra_en_memoria(uint32_t start, uint32_t size){
	return true;
}

void mover_en_memoria(uint32_t start, uint32_t destination, uint32_t size){
	void* data = malloc(size);
	memcpy(data, memoria_principal + start, size);
	memset(memoria_principal + start, 0, size); //Seteo en 0 lo que muevo. Ctrl + X basicamente
	memcpy(memoria_principal + destination, data, size);
	log_info(logger, "Se movio data %d desde %d a %d", *(uint32_t*)data, start, destination);
	free(data);
}

void liberar_memoria(uint32_t start, uint32_t size){
	memset(memoria_principal + start, 0, size); //Seteo en 0 lo que muevo. Ctrl + X basicamente
	log_info(logger, "Se limpio el frame desde %d a %d. Seteado con 0.", start, size);
}
