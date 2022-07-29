#include "includeh/manejar_swap.h"

extern t_mem_config* config;
extern t_log* logger;

void* crear_swap(uint32_t size, uint32_t pid){
	char* swap_name = generar_filename(pid);
	log_info(logger, "Creando file swap para proceso %d en %s", pid, swap_name);
	int swap_fd = open(swap_name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	ftruncate(swap_fd, size);
	void* swap_process_mem = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, swap_fd, 0);
	free(swap_name);
    close(swap_fd);
	msync(swap_process_mem, size, MS_SYNC);
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
	string_append(&swap_name, config->path_swap);
	string_append(&swap_name, "/");
	char* pid_string = string_itoa(pid);
	string_append(&swap_name, pid_string);
	string_append(&swap_name, ".swap");
	free(pid_string);
	return swap_name;
}

void escribir_swap(void* swap, void* data, uint32_t size, uint32_t start){
	memcpy(swap+start, data, size);
	msync(swap+start, size, MS_SYNC);
}

void* leer_swap(void* swap, uint32_t size, uint32_t start){
	void* data = malloc(size);
	memcpy(data, swap+start, size);
	return data;
}

void retardo_swap(){
	usleep(config->retardo_swap*1000); // Espero el tiempo dado por configuracion
}
