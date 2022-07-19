/*
 * servidor.h
 *
 *      Author: pmdea
 */

#ifndef INCLUDEH_SERVIDOR_H_
#define INCLUDEH_SERVIDOR_H_

#include<pthread.h>
#include "init_memoria.h"
#include "memoria.h"
#include<sys/socket.h>
#include<netdb.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
uint8_t ini_servidor();
void* escuchar_kernel(void* arg);
void* escuchar_cpu(void* arg);
void finalizar_servidor();
message_kernel* parsear_message_kernel(int cliente);
message_cpu* parsear_message_cpu(int cliente);
int esperar_cliente();
int enviar_mensaje_cliente(int cliente, void* data, int size);
void get_values_from_data(void* data, uint32_t* primer, uint32_t* segundo, uint32_t* tercero);
#endif /* INCLUDEH_SERVIDOR_H_ */
