#ifndef SRC_CONSOLA_H_
#define SRC_CONSOLA_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/queue.h>
#include <commons/string.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

typedef enum {
	IO = 0,
	READ = 1,
	WRITE = 2,
	COPY = 3,
	EXIT = 4,
	NO_OP = 5
} ID_INSTRUCCION;

typedef enum {
	HANDSHAKE,
	ENVIO_PROCESO,
	RECHAZO,
	CONFIRMACION,
	FINALIZACION_PROCESO,
	ERROR
} op_code;

typedef struct {
	op_code operacion;
	int tamanio_mensaje;
	char* mensaje;
} __attribute__((packed))
t_mensaje;

#define IDENTIFICADORES_VALIDOS "I/O,READ,WRITE,COPY,EXIT,NO_OP"
#define CLAVE_IP "IP_KERNEL"
#define CLAVE_PUERTO "PUERTO_KERNEL"
#define MENSAJE_HANDSHAKE_ENVIADO 9992751
#define PATH_CONFIG "./src/consola.config"
#endif /* SRC_CONSOLA_H_ */
