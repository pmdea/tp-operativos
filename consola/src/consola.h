/*
 * consola.h
 *
 *  Created on: 23 abr. 2022
 *      Author: utnso
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/collections/list.h>
#include<commons/collections/queue.h>
#include<commons/config.h>
#include <string.h>

//char *identificadores[6] = {"NO_OP","I/O","READ","WRITE","COPY","EXIT"};

/*
typedef struct {
	char* identificador;
	t_list* parametros; // LISTA
} Instruccion;
*/
typedef struct {
	t_queue* instrucciones_list; // LISTA
} Instrucciones;



#endif /* CONSOLA_H_ */

