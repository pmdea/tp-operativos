#include "includeh/manejar_memoria.h"

extern void* memoria_principal;
extern t_log* logger;
extern t_mem_config* config;

void escribir_en_memoria(uint32_t start, void* data, uint32_t size){
	log_info(logger, "Escribiendo %d bytes en pos %d", size, start);
	memcpy(memoria_principal + start, data, size);
}
void* leer_de_memoria(uint32_t start, uint32_t size){
	void* data = malloc(size);
	memcpy(data, memoria_principal + start, size);
	log_info(logger, "Leido de memoria: %d", *(uint32_t*)data);
	return data;
}
bool entra_en_memoria(uint32_t start, uint32_t size){

	return true; //TODO: ver si es necesario un metodo para verificar si entre en memoria
}
void mover_en_memoria(uint32_t start, uint32_t destination, uint32_t size){
	void* data = malloc(size);
	memcpy(data, memoria_principal + start, size);
	memset(memoria_principal + start, 0, size); //Seteo en 0 lo que muevo. Ctrl + X basicamente
	memcpy(memoria_principal + destination, data, size);
	log_info(logger, "Se movio data %d desde %d a %d", *(uint32_t*)data, start, destination);
	free(data);
}
