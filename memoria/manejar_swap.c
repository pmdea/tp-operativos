/*
 * manejar_swap.c
 *
 *  Created on: Jul 4, 2022
 *      Author: pmdea
 */

#include "includeh/manejar_swap.h"

extern t_mem_config* config;
extern t_log* logger;

void* crear_swap(uint32_t size, uint32_t pid){
	char* swap_name = generar_filename(pid);
	log_info(logger, "Creando file swap para proceso %d en %s", pid, swap_name);
	int swap = open(swap_name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	void* swap_process_mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, swap, 0);
	close(swap);
	free(swap_name);
	log_info(logger, "Espacio swap creado correctamente!");
	return swap_process_mem;
}

void eliminar_swap(uint32_t pid, void* swap, uint32_t size){
	char* file = generar_filename(pid);
	log_info(logger, "Eliminando file swap para proceso %d en %s", pid, file);
	munmap(swap, size);
	remove(file);
	free(file);
	log_info(logger, "Espacio swap eliminado correctamente!");
}

char* generar_filename(uint32_t pid){
	char* swap_name = string_new();
	string_append(&swap_name, "/");
	string_append(&swap_name, config->path_swap);
	string_append(&swap_name, string_itoa(pid));
	string_append(&swap_name, ".swap");
	return swap_name;
}

void escribir_swap(void* swap, void* data, uint32_t size, uint32_t start){
	memcpy(swap+start, data, size);
	free(data);
}

void* leer_swap(void* swap, uint32_t size, uint32_t start){
	void* data = malloc(size);
	memcpy(data, swap+start, size);
	return data;
}
