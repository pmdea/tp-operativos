#include "kernel.h"

int main(void)
{
	loggerKernel = iniciar_logger_kernel();
	configKernel = iniciar_config_kernel();
	iniciar_settings();

	t_instruccion* instrucc1 = asignarMemoria(sizeof(t_instruccion));
	instrucc1 -> identificador = NO_OP;
	instrucc1 -> parametros = queue_create();

	list_add(instrucc1 -> parametros -> elements, 3);

	t_instruccion* instrucc4 = asignarMemoria(sizeof(t_instruccion));
	instrucc4 -> identificador = IO;
	instrucc4 -> parametros = queue_create();

	list_add(instrucc4 -> parametros -> elements, 5000);

	t_instruccion* instrucc2 = asignarMemoria(sizeof(t_instruccion));
	instrucc2 -> identificador = WRITE;
	instrucc2 -> parametros = queue_create();

	list_add(instrucc2 -> parametros -> elements, 5000);
	list_add(instrucc2 -> parametros -> elements, 3544);

	t_instruccion* instrucc3 = asignarMemoria(sizeof(t_instruccion));
	instrucc3 -> identificador = EXIT;
	instrucc3 -> parametros = queue_create();

	t_proceso* proceso = asignarMemoria(sizeof(t_proceso));
	proceso -> tamanio_proceso = 15;
	proceso -> instrucciones = queue_create();
	list_add(proceso -> instrucciones -> elements, instrucc1);
	list_add(proceso -> instrucciones -> elements, instrucc2);
	list_add(proceso -> instrucciones -> elements, instrucc1);
	list_add(proceso -> instrucciones -> elements, instrucc3);

	generarEstructuraPCB(23, proceso);
//	PCB* unPCB = list_get(procesosNew, 0);

/*	enviarPCB(socket_dispatch, *unPCB, loggerKernel);

	while(1){
		unPCB = deserializarPCB(socket_dispatch);
		log_warning(loggerKernel, "INICIANDO BLOQUEO DE 5000 - LISTA %i", list_size(unPCB->instrucciones));
		sleep(5);
		enviarPCB(socket_dispatch, *unPCB, loggerKernel);
	}*/

	iniciar_planificadores();
}
