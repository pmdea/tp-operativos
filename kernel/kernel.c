/*
 * kernel.c
 *
 *  Created on: Apr 23, 2022
 *      Author: pmdea
 */
#include "kernel.h"

int main(void)
{
	t_log* logger;
	t_config* config;
	logger = iniciar_logger_kernel();
	log_info(logger, "Modulo Kernel");
	config = iniciar_config_kernel(); // Traer datos del archivo de configuracion

	// Listas
	procesosNew = list_create();
	procesosReady = list_create();
	procesosExecute = list_create();
	procesosBlocked = list_create();
	procesosSuspendedBlocked = list_create();
	procesosSuspendedReady = list_create();
	procesosExit = list_create();



	/*
	 * while (1){
	 *
	 * 	planificador_largo_plazo();
	 *
	 * }
	 */
}
//El tamaño y las instrucciones vienen desde consola
void generar_PCB(int idUltimo, int tamanioProceso, char* instrucciones){ // Funcion para cargar los datos del proceso al PCB

	pcb *nuevoProceso = malloc(sizeof(pcb));
	nuevoProceso -> id = idUltimo;
	nuevoProceso -> tamanio = tamanioProceso;
	nuevoProceso -> instrucciones = ""; // LISTA
	nuevoProceso ->  program_counter = 0;
	nuevoProceso ->  tabla_paginas = "-"; // LISTA
	nuevoProceso ->  estimacion_rafaga = config_kernel.estimacion_inicial;

	list_add(procesosNew, nuevoProceso);

	//free(nuevoProceso); Si la libero no me tira bien los datos

	//printf("Proceso creado correctamente");
}

void planificador_LargoPlazo(){

	int enEjecucion = list_size(procesosExecute); // Procesos en ejecucion
	int enReady = list_size(procesosReady); // Procesos en ready
	int enBlock = list_size(procesosBlocked); // Procesos en blocked
	int enFinalizacion = list_size(procesosExit);

	int espacio_ocupado_memoria_principal = enEjecucion + enReady + enBlock;

	while (espacio_ocupado_memoria_principal < config_kernel.grado_multiprogramacion) {

		// Obtengo el primero de la lista de procesosNew
		pcb * unProceso;
		unProceso = list_get(procesosNew, 0);

		//Envio de mensaje a Modulo de Memoria para generar estructuras

		//enviarMensaje("PCB-Generado", socket_memoria);

		//wait(esperarRespuesta); // el signal lo deberia tener memoria luego de crear las estructuras (Tablas/Paginas)

		//Obtengo tablas de direccion

			//recibirMensaje(socket_memoria);

		// Asigno al PCB y lo guardo en la lista procesosReady
		unProceso -> tabla_paginas = "reemplazar cuando este la funcion realizada arriba";
		list_add(procesosReady, unProceso);
		list_remove(procesosNew, 0);

		free(unProceso); // Ver si se guarda bien la info o no

		// Analizar si tengo procesos en finalizacion
		if (enFinalizacion != 0){
			pcb * unProcess = list_get(procesosExit, 0);
			// Envio Msj a Memoria para liberar
			// Aviso a consola que termino.
			list_remove(unProceso, 0);
			free(unProcess);
		}

	}
}


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


