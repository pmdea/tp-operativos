#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <sys/socket.h>
#include <netdb.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IP "127.0.0.1"
#define PUERTO "8000"

t_log* logger;

typedef enum
{
	MENSAJE,
	PAQUETE,
	COLA
}op_code;
