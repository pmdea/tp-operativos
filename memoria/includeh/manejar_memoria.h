/*
 * manejar_memoria.h
 *
 *      Author: pmdea
 */

#ifndef INCLUDEH_MANEJAR_MEMORIA_H_
#define INCLUDEH_MANEJAR_MEMORIA_H_

#include <stdbool.h>
#include "init_memoria.h"

// metodos para manejar el espacio de usuario
char* escribir_en_memoria(uint32_t start, void* data, uint32_t size);
void* leer_de_memoria(uint32_t start, uint32_t size);
bool entra_en_memoria(uint32_t start, uint32_t size);
void mover_en_memoria(uint32_t start, uint32_t destination, uint32_t size);
void liberar_memoria(uint32_t start, uint32_t size);
#endif /* INCLUDEH_MANEJAR_MEMORIA_H_ */
