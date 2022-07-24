#include "kernel.h"

int main(void)
{
	// Logger y Config
	loggerKernel = iniciar_logger_kernel();
	configKernel = iniciar_config_kernel();
	//iniciar_conexiones();
	log_info(loggerKernel, "CONFIGURACION, LOGGER Y CONEXIONES CARGADOS", config_kernel.puerto_memoria);

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

/*	log_info(loggerKernel, "LISTAS INICIADAS");

    // Semaforos
	pthread_mutex_init(&mutexExit, NULL);
	sem_init(&grado_multiprogramacion, 0, config_kernel.grado_multiprogramacion);
    sem_init(&prioridad_SuspendedReady, 0, 0);
    sem_init(&nuevoProcesoReady, 0, 0);
    pthread_mutex_init(&mutexReady, NULL);
    sem_init(&bloqueoMax, 0, 0);
    pthread_mutex_init(&mutexBloqueo, NULL);
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
	 *
	 */
}
