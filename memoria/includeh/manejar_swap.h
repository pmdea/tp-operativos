#ifndef INCLUDEH_MANEJAR_SWAP_H_
#define INCLUDEH_MANEJAR_SWAP_H_

#include <commons/string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include "init_memoria.h"

void* crear_swap(uint32_t size, uint32_t pid);
void eliminar_swap(uint32_t pid, void* swap, uint32_t size);
void escribir_swap(void* swap, void* data, uint32_t size, uint32_t start);
void* leer_swap(void* swap, uint32_t size, uint32_t start);
char* generar_filename(uint32_t pid);

#endif /* INCLUDEH_MANEJAR_SWAP_H_ */
