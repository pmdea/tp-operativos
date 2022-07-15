#include "kernel.h"
#include "utils.c"
#include "conexion.c"
#include "funcionesPCB.c"
#include "logger_y_config.c"

int main(void){

	logger = iniciar_logger_kernel();
	log_info(logger, "Inicializando Modulo Kernel...");
	config = iniciar_config_kernel(); // Traer datos del archivo de configuracion

	t_instruccion* inst1;
	inst1 -> identificador = "NO_OP";
	inst1 -> parametros = queue_create();
	list_add(inst1 -> parametros -> elements, 3);

	t_instruccion* inst4;
	inst1 -> identificador = "NO_OP";
	inst1 -> parametros = queue_create();
	list_add(inst1 -> parametros -> elements, 10);

	t_instruccion* inst2;
	inst2 -> identificador = "EXIT";
	inst2 -> parametros = queue_create();

	t_instruccion* inst3;
	inst2 -> identificador = "I/O";
	inst2 -> parametros = queue_create();
	list_add(inst2 -> parametros -> elements, 5000);


	t_proceso* proceso;
	proceso-> tamanio_proceso = 10;
	proceso-> instrucciones = queue_create();

	t_proceso* proceso2;
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

    //Conexiones
    socket_memoria = crear_conexion(config_kernel.ip_memoria, config_kernel.puerto_memoria);
    socket_cpu_dispatch = crear_conexion(config_kernel.ip_cpu, config_kernel.puerto_cpu_dispatch);
    socket_cpu_interrupt = crear_conexion(config_kernel.ip_cpu, config_kernel.puerto_cpu_interrupt);

	// Semaforos
	sem_init(&grado_multiprogramacion, 0, config_kernel.grado_multiprogramacion);
	sem_init(&prioridad_SuspendedReady, 0, 0);
	sem_init(&nuevoProcesoReady, 0, 0);
	sem_init(&mutexReady, 0, 1);
    sem_init(&bloqueoMax, 0, 0);
	sem_init(&mutexExit, 0, 1);
	sem_init(&mutexBloqueo, 0, 1);
	sem_init(&procesoBloqueado, 0, 0);
	
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
	int ejecutando  = 0 ;
	int tiemposBlockedSuspended = 0;
	// lo puse en el de desalojo paquetedeCPU_Desalojo = list_create();
	// paquetedeCPU_Analisis = list_create();

	// Hilos
	pthread_create(&planificadorLargoPlazoHilo, NULL, (void *) planificadorLargoPlazo, NULL);
	pthread_create(&planificadorMedianoPlazoHilo, NULL, (void *) planificadorMedianoPlazo, NULL);
	pthread_create(&planificadorCortoPlazoHilo, NULL, (void *) planificadorCortoPlazo, NULL);

	pthread_detach(planificadorLargoPlazoHilo);
	pthread_detach(planificadorMedianoPlazoHilo);
	pthread_detach(planificadorCortoPlazoHilo);


}









