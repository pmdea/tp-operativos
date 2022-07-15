#include "kernel.h"
#include <semaphore.h>

int main(void)
{
	// Logger y Config
	loggerKernel = iniciar_logger_kernel();
	configKernel = iniciar_config_kernel();

	log_info(loggerKernel, "CONFIGURACION Y LOGGER CARGADOS", config_kernel.puerto_memoria);

	// Listas
	procesosNew = list_create();
	procesosReady = list_create();
	procesosExecute = list_create();
	procesosBlocked = list_create();
	tiemposBlocked = list_create();
	procesosSuspendedBlocked = list_create();
	procesosSuspendedReady = list_create();
	procesosExit = list_create();
	conexiones_pcb = list_create();

	log_info(loggerKernel, "LISTAS INICIADAS");

    // Semaforos
    sem_init(&mutexExit, 0, 1);
	sem_init(&grado_multiprogramacion, 0, config_kernel.grado_multiprogramacion);
    sem_init(&prioridad_SuspendedReady, 0, 0);
    sem_init(&nuevoProcesoReady, 0, 0);
    sem_init(&mutexReady, 0, 1);
    sem_init(&bloqueoMax, 0, 0);
    sem_init(&mutexBloqueo, 0, 1);
    sem_init(&procesoBloqueado, 0, 0);

	log_info(loggerKernel, "SEMAFOROS INICIADOS");

	// GENERO PRUEBA DEL PCB
	t_proceso* proceso = malloc(sizeof(t_proceso));
	t_proceso* proceso2 = malloc(sizeof(t_proceso));

	t_instruccion* inst1 = malloc(sizeof(t_instruccion));
	t_instruccion* inst2= malloc(sizeof(t_instruccion));
	t_instruccion* inst3= malloc(sizeof(t_instruccion));
	t_instruccion* inst4= malloc(sizeof(t_instruccion));


	inst1 -> identificador = "NO_OP";
	inst1 -> parametros = queue_create();
	list_add(inst1 -> parametros -> elements, 3);

	inst2 -> identificador = "EXIT";
	inst2 -> parametros = queue_create();

	inst3 -> identificador = "I/O";
	inst3 -> parametros = queue_create();
	list_add(inst3 -> parametros -> elements, 5000);

	inst4 -> identificador = "NO_OP";
	inst4 -> parametros = queue_create();
	list_add(inst4 -> parametros -> elements, 10);

	log_info(loggerKernel, "ANDAN INSTRUCCIONES");

    proceso-> tamanio_proceso = 10;
    proceso-> instrucciones = queue_create();

    proceso2-> tamanio_proceso = 10;
    proceso2-> instrucciones = queue_create();


    list_add(proceso-> instrucciones -> elements, inst1);
    list_add(proceso-> instrucciones -> elements, inst3);
    list_add(proceso-> instrucciones -> elements, inst1);
    list_add(proceso-> instrucciones -> elements, inst2);

    list_add(proceso2-> instrucciones -> elements, inst4);
    list_add(proceso2-> instrucciones -> elements, inst2);


    generar_PCB(0, proceso);
    generar_PCB(1, proceso);
    generar_PCB(2, proceso2);
    generar_PCB(3, proceso2);
    generar_PCB(4, proceso2);

    log_info(loggerKernel, "SEMAFOROS INICIAD2222OS");

	pthread_create(&planificadorLargoPlazoHilo, NULL, planificador_LargoPlazo, NULL);
	//pthread_detach(planificadorLargoPlazoHilo);
	pthread_join(planificadorLargoPlazoHilo, NULL);
    // FIN PRUEBAS PCB

}


