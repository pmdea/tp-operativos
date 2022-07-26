#include "cpu.h"

t_log* iniciar_logger_cpu(void)
{
	t_log* nuevo_logger;
	if ((nuevo_logger = log_create("cpu.log", "CPU", 1, LOG_LEVEL_DEBUG)) == NULL){
		printf("No pude crear el logger");
		exit(1);
	}
	return nuevo_logger;
}

t_config* iniciar_config_cpu(void) // CARGO LA INFORMACION DEL CONFIG
{
    t_config* nuevo_config;
    if((nuevo_config = config_create("./cpu.config")) == NULL){
        printf("No pude leer la config\n");
        exit(2);
    }

    config_cpu.entradas_tlb = config_get_int_value(nuevo_config, "ENTRADAS_TLB");
    config_cpu.reemplazo_tlb = config_get_int_value(nuevo_config, "REEMPLAZO_TLB");
    config_cpu.retardo_noop = config_get_int_value(nuevo_config, "RETARDO_NOOP");
    config_cpu.ip_memoria = config_get_string_value(nuevo_config, "IP_MEMORIA");
    config_cpu.puerto_memoria = config_get_int_value(nuevo_config, "PUERTO_MEMORIA");
    config_cpu.puerto_cpu_dispatch = config_get_int_value(nuevo_config, "PUERTO_ESCUCHA_DISPATCH");
    config_cpu.puerto_cpu_interrupt = config_get_int_value(nuevo_config, "PUERTO_ESCUCHA_INTERRUPT");


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
