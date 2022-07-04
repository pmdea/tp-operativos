/*
 * manejar_memoria.h
 *
 *  Created on: Jul 4, 2022
 *      Author: pmdea
 */

#ifndef INCLUDEH_MANEJAR_MEMORIA_H_
#define INCLUDEH_MANEJAR_MEMORIA_H_

#include <stdbool.h>

void escribir_en_memoria(uint32_t start, void* data, uint32_t size);
void* leer_de_memoria(uint32_t start, uint32_t size);
bool entra_en_memoria(uint32_t start, uint32_t size);
void mover_en_memoria(uint32_t start, uint32_t destination, uint32_t size);

#endif /* INCLUDEH_MANEJAR_MEMORIA_H_ */
