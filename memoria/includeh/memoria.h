/*
 * memoria.h
 *
 *  Created on: Jun 3, 2022
 *      Author: pmdea
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<stdio.h>
#include<inttypes.h>
#include "init_memoria.h"
#include "manejar_memoria.h"

void config_module(void);
// Ante un pedido de lectura, devolver el valor que se encuentra en la posición pedida
void* read_request(void* posicion);
// Ante un pedido de escritura, escribir lo indicado en la posición pedida. En caso satisfactorio se responderá un mensaje de ‘OK’ o, de lo contrario, un mensaje de error.
char* write_request(void* posicion);

void module_swap(void);

// Los archivos deben tener el formato [pid].swap
void create_swap(int pid, void* process);

void init_mem(void);

t_mem_config *create_config_struct(t_config* config);

void delete_swap(int pid);

void kill_module(t_config* config, t_log* logger);

void alg_clock_mod();

void alg_clock();

//Deberia enviar los datos pertinentes al CPU
void handshake_cpu(t_mem_config* config, int* socket_cpu);

void crear_tp(uint32_t id);




#endif /* MEMORIA_H_ */
