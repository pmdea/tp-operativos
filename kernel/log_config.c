#include "kernel.h"

t_log* iniciar_logger_kernel(void)
{
	t_log* nuevo_logger;
	if ((nuevo_logger = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_INFO)) == NULL){
		printf("No pude crear el logger");
		exit(1);
	}
	return nuevo_logger;
}

t_config* iniciar_config_kernel(void) // CARGO LA INFORMACION DEL CONFIG
{
    t_config* nuevo_config;
    if((nuevo_config = config_create("./kernel.config")) == NULL){
        printf("No pude leer la config\n");
        exit(2);
    }

    config_kernel.ip_memoria = config_get_string_value(nuevo_config, "IP_MEMORIA");
    config_kernel.puerto_memoria = config_get_int_value(nuevo_config, "PUERTO_MEMORIA");
    config_kernel.ip_cpu = config_get_string_value(nuevo_config, "IP_CPU");
    config_kernel.puerto_cpu_dispatch = config_get_int_value(nuevo_config, "PUERTO_CPU_DISPATCH");
    config_kernel.puerto_cpu_interrupt = config_get_int_value(nuevo_config, "PUERTO_CPU_INTERRUPT");
    config_kernel.puerto_escucha = config_get_int_value(nuevo_config, "PUERTO_ESCUCHA");
    config_kernel.algoritmo_planificacion = config_get_string_value(nuevo_config, "ALGORITMO_PLANIFICACION");
    config_kernel.estimacion_inicial = config_get_int_value(nuevo_config, "ESTIMACION_INICIAL");
    config_kernel.alfa = config_get_double_value(nuevo_config, "ALFA");
    config_kernel.grado_multiprogramacion = config_get_int_value(nuevo_config, "GRADO_MULTIPROGRAMACION");
    config_kernel.tiempo_maximo_bloqueado = config_get_int_value(nuevo_config, "TIEMPO_MAXIMO_BLOQUEADO");

    return nuevo_config;

}

void terminar_programa(t_log* logger, t_config* config)
{

    if(logger != NULL){
        log_destroy(logger);
    }

    if(config != NULL){
        config_destroy(config);
    }
}
