#include "kernel.h"
#include "utils.h"


int main(void)
{
	t_log* logger;
	t_config* config;
	logger = iniciar_logger_kernel();
	log_info(logger, "Inicializando Modulo Kernel...");
	config = iniciar_config_kernel(); // Traer datos del archivo de configuracion

    //Conexiones
    int socket_memoria = crear_conexion(config_kernel.ip_memoria, config_kernel.puerto_memoria);
    int socket_cpu_dispatch = crear_conexion(config_kernel.ip_cpu, config_kernel.puerto_cpu_dispatch);
    int socket_cpu_interrupt = crear_conexion(config_kernel.ip_cpu, config_kernel.puerto_cpu_interrupt);

	// Semaforos
	sem_init(&grado_multiprogramacion, 0, config_kernel.grado_multiprogramacion);
	sem_init(&prioridad_SuspendedReady, 0, 0);
	sem_init(&nuevoProcesoReady, 0, 0);
	sem_init(&mutexReady, 0, 1);
    sem_init(&bloqueoMax, 0, 0);
	sem_init(&mutexExit, 0, 1);
	
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
	bool ejecutando  = 0 ;
	// lo puse en el de desalojo paquetedeCPU_Desalojo = list_create();
	// paquetedeCPU_Analisis = list_create();

	// Hilos
	pthread_create(&planificadorLargoPlazo, NULL, (void *) planificador_LargoPlazo, NULL);
	pthread_create(&planificadorMedianoPlazo, NULL, (void *) planificador_MedianoPlazo, NULL);
	pthread_create(&planificadorCortoPlazo, NULL, (void *) planificador_CortoPlazo, NULL);

	pthread_detach(planificadorLargoPlazo);
	pthread_detach(planificadorMedianoPlazo);
	pthread_detach(planificadorCortoPlazo);


}









