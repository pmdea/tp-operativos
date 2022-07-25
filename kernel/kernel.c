#include "kernel.h"

int main(void)
{
	// Logger y Config
	loggerKernel = iniciar_logger_kernel();
	configKernel = iniciar_config_kernel();
	iniciar_conexiones();

	// Listas
	procesosNew = list_create();
	procesosReady = list_create();
	procesosExecute = list_create();
	procesosBlocked = list_create();
	tiemposBlocked = list_create();
	tiemposBlockedSuspendMax = list_create();
	procesosSuspendedBlocked = list_create();
	procesosSuspendedReady = list_create();
	procesosExit = list_create();
	conexiones_pcb = list_create();
/*
	struct sockaddr_in direccion_kernel;
	inicializar_direccion_kernel(&direccion_kernel);

	int kernel = socket(AF_INET, SOCK_STREAM, 0);
	int activado = 1;
	setsockopt(kernel, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	//LEER ARCHIVO CONFIG

	//ESPERAR CONEXION CONSOLA
	bindear_kernel(kernel, direccion_kernel);

	escuchar(kernel);

	//CREAR HILOS

	procesar_entradas_de_consolas(kernel);

	//FINALIZAR KERNEL

	return 0;
*/

    // Semaforos
	pthread_mutex_init(&mutexExit, NULL);
	sem_init(&grado_multiprogramacion, 0, config_kernel.grado_multiprogramacion);
    sem_init(&prioridad_SuspendedReady, 0, 0);
    sem_init(&nuevoProcesoReady, 0, 0);
    pthread_mutex_init(&mutexReady, NULL);
    sem_init(&bloqueoMax, 0, 0);
    pthread_mutex_init(&mutexBloqueo, NULL);
    sem_init(&procesoBloqueado, 0, 0);

	log_info(loggerKernel, "CONFIGURACION, LOGGER, CONEXIONES, SEMAFOROS, LISTAS: Generadas....");

	t_instruccion* instrucc1 = asignarMemoria(sizeof(t_instruccion));
	instrucc1 -> identificador = NO_OP;
	instrucc1 -> parametros = queue_create();

	list_add(instrucc1 -> parametros -> elements, 3);

	t_instruccion* instrucc2 = asignarMemoria(sizeof(t_instruccion));
	instrucc2 -> identificador = IO;
	instrucc2 -> parametros = queue_create();

	list_add(instrucc2 -> parametros -> elements, 5000);

	t_instruccion* instrucc3 = asignarMemoria(sizeof(t_instruccion));
	instrucc3 -> identificador = EXIT;


	t_proceso* proceso = asignarMemoria(sizeof(t_proceso));
	proceso -> tamanio_proceso = 15;
	proceso -> instrucciones = queue_create();
	list_add(proceso -> instrucciones -> elements, instrucc1);
	list_add(proceso -> instrucciones -> elements, instrucc2);
	list_add(proceso -> instrucciones -> elements, instrucc1);
	list_add(proceso -> instrucciones -> elements, instrucc3);


	generarEstructuraPCB(23, proceso);


	pthread_create(&planificadorLargoPlazoHilo, NULL, planificador_LargoPlazo, NULL);
	pthread_create(&planificadorMedianoPlazoHilo, NULL, planificador_MedianoPlazo, NULL);
	pthread_create(&planificadorCortoPlazoHilo, NULL, planificador_CortoPlazo, NULL);

	pthread_join(planificadorLargoPlazoHilo, NULL);
	pthread_join(planificadorMedianoPlazoHilo, NULL);
	pthread_join(planificadorCortoPlazoHilo, NULL);


	//pthread_detach(planificadorLargoPlazoHilo);
	//pthread_detach(planificadorCortoPlazoHilo);




    // FIN PRUEBAS PCB

	//printf("Proceso creado correctamente");

}
