#include <sys/socket.h>
#include <netdb.h>
#include <commons/collections/list.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <readline/readline.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>

t_log* logger_consola;

char* identificador = "WRITE";
char* paramA = "1";
char* paramB = "2";

typedef enum
{
	MENSAJE,
	PAQUETE,
	COLA
}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;
