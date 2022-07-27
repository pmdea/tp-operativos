#include "includeh/manejar_memoria.h"

extern void* memoria_principal;
extern t_log* logger;
extern pthread_mutex_t mutex_memoria;

char* escribir_en_memoria(uint32_t direc_fisica, void* data, uint32_t size){
	log_info(logger, "Escribiendo %d bytes en pos %d", size, direc_fisica);
	pthread_mutex_lock(&mutex_memoria);
	memcpy(memoria_principal + direc_fisica, data, size);
	pthread_mutex_unlock(&mutex_memoria);
	free(data);
	return "OK"; //Según enunciado
}
void* leer_de_memoria(uint32_t start, uint32_t size){
	void* data = malloc(size);
	pthread_mutex_lock(&mutex_memoria);
	memcpy(data, memoria_principal + start, size);
	pthread_mutex_unlock(&mutex_memoria);
	return data;
}

bool entra_en_memoria(uint32_t start, uint32_t size){
	return true;
}

void mover_en_memoria(uint32_t start, uint32_t destination, uint32_t size){
	void* data = malloc(size);
	pthread_mutex_lock(&mutex_memoria);
	memcpy(data, memoria_principal + start, size);
	memset(memoria_principal + start, 0, size); //Seteo en 0 lo que muevo. Ctrl + X basicamente
	memcpy(memoria_principal + destination, data, size);
	pthread_mutex_unlock(&mutex_memoria);
	log_info(logger, "Se movio data %d desde %d a %d", *(uint32_t*)data, start, destination);
	free(data);
}

void liberar_memoria(uint32_t start, uint32_t size){
	pthread_mutex_lock(&mutex_memoria);
	memset(memoria_principal + start, 0, size); //Seteo en 0 lo que muevo. Ctrl + X basicamente
	pthread_mutex_unlock(&mutex_memoria);
	log_info(logger, "Se limpio el frame desde %d a %d. Seteado con 0.", start, size);
}
