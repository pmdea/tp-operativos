/*
 * servidor.h
 *
 *      Author: pmdea
 */

#ifndef INCLUDEH_SERVIDOR_H_
#define INCLUDEH_SERVIDOR_H_

#include<pthread.h>
#include "init_memoria.h"
#include<sys/socket.h>
#include<netdb.h>

uint8_t ini_servidor();
void* escuchar_kernel(void* arg);
void* escuchar_cpu(void* arg);
void finalizar_servidor();

#endif /* INCLUDEH_SERVIDOR_H_ */
