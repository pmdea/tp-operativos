/*
 * kernel.c
 *
 *  Created on: Apr 23, 2022
 *      Author: pmdea
 */
#include "kernel.h"

bool ordenarSTR(pcb* unPCB,pcb* otroPCB){
	double est1 = unPCB->estimacion_rafaga;
	double est2 = otroPCB->estimacion_rafaga;
	return est2 > est1;
}

void estimador(pcb* unPCB, double alfa, int rafaga_ejecutada){
	unPCB -> estimacion_rafaga = (alfa * rafaga_ejecutada + (1 - alfa) * unPCB->estimacion_rafaga);
}

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

	paquetedeCPU_Desalojo = list_create();
	paquetedeCPU_Analisis = list_create();


/*
	generar_PCB(10,20,5);

	pcb* unProceso;
	unProceso = list_get(procesosNew, 0);
	printf("estimador actual: %f", unProceso->estimacion_rafaga);
	estimador(unProceso, 0.5, 5);
	printf("estimador siguiente: %f", unProceso->estimacion_rafaga);
	list_add(procesosReady, unProceso);
	list_remove(procesosNew, 0);
	free(unProceso);

	pcb* mismoProceso;
	mismoProceso = list_get(procesosReady, 0);
	printf("estimador siguiente: %f", mismoProceso->estimacion_rafaga);


	//printf("Numero de procesos nuevos: %i", list_size(procesosNew));
	printf("Numero del primero antes de ordenar: %i", unProceso->id);
	//list_sort(procesosNew, (void*) ordenarSTR);
	unProceso = list_get(procesosNew, 0);
	printf("Numero del primero despues de ordenar: %i", unProceso->id);
	*/



	/* HILOS ENTRE LOS PLANI PARA GENERAR PARALELISMO
	 * while (1){
	 *
	 * 	planificador_largo_plazo();
	 *	planificador_corto_plazo(algoritmo);
	 * }
	 */

}
//El tamaño y las instrucciones vienen desde consola
void generar_PCB(int idUltimo, int tamanioProceso, t_list* instrucciones){ // Funcion para cargar los datos del proceso al PCB

	pcb *nuevoProceso = malloc(sizeof(pcb));
	nuevoProceso -> id = idUltimo;
	nuevoProceso -> tamanio = tamanioProceso;
	nuevoProceso -> instrucciones = instrucciones; // LISTA
	nuevoProceso ->  program_counter = 0;
	nuevoProceso ->  tabla_paginas = "-"; // LISTA
	nuevoProceso ->  estimacion_rafaga = config_kernel.estimacion_inicial;

	list_add(procesosNew, nuevoProceso);

	//free(nuevoProceso); Si la libero no me tira bien los datos

	//printf("Proceso creado correctamente");
}

void planificador_LargoPlazo(){
	//sem mientras la lee no quiero q cambie
	int enEjecucion = list_size(procesosExecute); // Procesos en ejecucion
	int enReady = list_size(procesosReady); // Procesos en ready
	int enBlock = list_size(procesosBlocked); // Procesos en blocked
	int enFinalizacion = list_size(procesosExit); // Procesos que finalizan
	//sem
	int espacio_ocupado_memoria_principal = enEjecucion + enReady + enBlock;

	while (espacio_ocupado_memoria_principal < config_kernel.grado_multiprogramacion) {
		int enSuspendedReady = list_size(procesosSuspendedReady); // Procesos con más prioridad para acceder a Ready
		//Doy prioridad al Planificador Mediano Plazo
		if(enSuspendedReady > 0){
			//wait(prioridadSuspendedReady);
		} else {

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


			// SEMAFORO  CON EL DE LARGO CORTO MUTEX sem_wait(agregarAReady)
			list_add(procesosReady, unProceso);
			// SEMAFORO  CON EL DE LARGO CORTO MUTEX sem_post(agregarAReady)

			list_remove(procesosNew, 0);

			free(unProceso); // Ver si se guarda bien la info o no
		}

		// Analizar si tengo procesos en finalizacion
		if (enFinalizacion != 0){
			pcb * unProceso = list_get(procesosExit, 0);
			// Envio Msj a Memoria para liberar
			// Aviso a consola que termino.
			list_remove(unProceso, 0);
			free(unProceso);
		}

	}
}
/*
void planificador_CortoPlazo(char algoritmo){

	int ciclosTotales=0;


	if(algoritmo == 'FIFO'){
		algoritmo_FIFO()
	}else{
		algoritmo_SRT()
	}

	//algoritmo
	while(enReady > 0 && enEjecucion == 0){
		pcb * unProceso;
		// unProceso = EL QUE DECIDE EL ALGORITMO
		// se lo mando a CPU
	}
}

void algoritmo_FIFO(){

	//sem  mientras la lee no quiero q cambie
	int enEjecucion = list_size(procesosExecute); // Procesos en ejecucion
	int enReady = list_size(procesosReady); // Procesos en ready
	int enFinalizacion = list_size(procesosExit);
	int enBlock = list_size(procesosBlocked); // Procesos en blocked
	//sem

	t_list*  tiempoABlockear = list_create();
	// algoritmo
	// FIFO PROCESOS READY ==>  [ b, c, d] bloqueado a
	while(enReady > 0){
		pcb * unProceso;

		//wait  necesito un sem mutex? sem_wait(agregarAReady) QUIZA
		unProceso = list_remove(procesosReady, 0);

		list_add(procesosExecute, unProceso);
		// signal sem_post(agregarAReady) QUIZA


		// signal(cheTemandoPRoceso) binario
		// se lo mando al cpu el Proceso

		// wait espero lo q me manda el cpu desp de ejecutar el proceso (un exit, bloqueo, desalojo) binario

		// paquete = recibirPaquete(socket_memoria); SUPONIENDO Q ME MANDA UN MENSAJE EN EL PAQUETE
		if( paquete->mensaje == 'exit' ){
			list_add(procesosExit, paquete->pcb);
		}

		if(paquete->mensaje == 'i/o'){
			//necesito el tiempo
			// suponiendo que tengo el tiempo
			int tiempo =  paquete->tiempo + 1 ; // 2 para q le reste la primera vez
			list_add(procesosBlocked, paquete->pcb); // lista bloqueados [ a, b ]
			list_add(tiempoABlockear, tiempo) ;// [ 3 ]

		}

		int tiempoBlockeadoAhora = list_get(tiempoABlockear, 0); // 0
		if(tiempoBlockeadoAhora){ // si
			if(tiempoBlockeadoAhora == 0){ // si

				// SEMAFORO  CON EL DE LARGO PLAZO MUTEX sem_wait(agregarAReady)
				list_add(procesosReady, list_remove(procesosBlocked, 0)); // [ a, c ] => [ c ] ** devuelve un pcb
				// SEMAFORO  CON EL DE LARGO PLAZO MUTEX sem_post(agregarAReady)

				// se termino el bloqueo por lo tanto lo meto devuelta en la lista de ready
				list_remove(tiempoABlockear, 0); // [ 0, 4 ] => [ 4 ]

			}
			list_replace(tiempoABlockear, 0, tiempoBlockeadoAhora--); // [ 3 ]
		}
	}
}




void algoritmo_SRT(){
	//sem  mientras la lee no quiero q cambie
	int enEjecucion = list_size(procesosExecute); // Procesos en ejecucion
	int enReady = list_size(procesosReady); // Procesos en ready
	int enFinalizacion = list_size(procesosExit);
	int enBlock = list_size(procesosBlocked); // Procesos en blocked
	//sem
	 *
	 * join thread desalojo_PCB()
	 * join thread analizar_PCB()
}

void desalojo_PCB() {
	while(1) {
		//sem  mientras la lee no quiero q cambie
		int enEjecucion = list_size(procesosExecute); // Procesos en ejecucion
		int enReady = list_size(procesosReady); // Procesos en ready
		int enFinalizacion = list_size(procesosExit);
		int enBlock = list_size(procesosBlocked); // Procesos en blocked
		//sem
		if(enReady >= 1){

			if(enReady > 1){
				list_sort(procesosReady, ordenarSRT);
			}
			pcb* ready_menorEstimador_PCB;
			menorEstimador_PCB_Ready = list_get(procesosReady, 0);

			if(enEjecucion > 0) {

				pcb* execute_PCB;
				execute_PCB = list_get(procesosExecute, 0);

				if(menorEstimador_PCB_Ready -> estimacion_rafaga > execute_PCB -> estimacion_rafaga){

					// Creo conexion interrupt cpu - Mando mensaje de desalojo
					 * conexion_cpu_interrupt = crear_conexion(config_kernel.ip_cpu, config_kernel.puerto_cpu_interrupt);
					 * enviar_mensaje("Desalojo", conexion_cpu_interrupt); O ENVIAR UN SIGNAL
				//	 * wait(esperarDesalojo); Espero desalojo
				//	 * paquetedeCPU_Desalojo = recibir_paquete(cpu_dispatch); Vincular con las conexiones realizadas
				 	 *
					 * sem_wait(agregarAReady) // Mantiene el grado de multiprogramacion actual
					 * pcb * cpu_PCB_Recibido = list_remove(paquetedeCPU, 0);
					 * list_remove(procesosExecute, 0);
					 * list_add(procesosReady, cpu_PCB_Recibido);
					 *
					 *
					 * list_remove(procesosReady, 0);
					 * list_add(procesosExecute, menorEstimador_PCB_Ready);
					 * sem_post(agregarAReady)
					 *
					 * free(cpu_PCB_Recibido);
					 * Este PCB es el que tengo que agregar al paquete -> menorEstimador_PCB_Ready
					 *
				//	 * Armar un paquete t_paquete* pcbExecute; (Ver como hacer);
					 * enviar_paquete(pcbExecute, cpu_Dispatch); // Se elimina dentro de la funcion luego de ser mandado.
					 *
				}
					 *
					 *
				free(execute_PCB);
			} else {

					sem_wait(agregarAReady);
					list_add(procesosExecute, menorEstimador_PCB_Ready);
					list_remove(procesosReady, 0);
					sem_post(agregarAReady);

					Este PCB es el que tengo que agregar al paquete -> menorEstimador_PCB_Ready

					Armar un paquete t_paquete* pcbExecute; (Ver como hacer);
					enviar_paquete(pcbExecute, cpu_Dispatch); // Se elimina dentro de la funcion luego de ser mandado.


		}
	}
}

// SRT Analisis
 *
 *void analizar_PCB(conexion_dispatch){
 *	int tiempoBloqueado;
 *	int indice;
 *	t_list* tiempo_bloqueado;
 *	char** string;
 *
 *	while(1){
 *		wait(Esperar_CPU);
 *		pcb * pcb_actualizado;
 *		paquetedeCPU_SoloAnalisis = recibir_paquete(conexion_dispatch); // PCB + Tiempo
 *		pcb_actualizado = list_get(paquetedeCPU_SoloAnalisis, 1);
 *		lista = [a, b, c]
 *		#A QUE HACE EXIT - I/O
 *		#B PCB ACTUALIZADO
 *		#C TIEMPO
 *		indice = pcb_actualizado -> program_counter - 1;
 *		if(indice < 0){
 *			indice = 0;
 *		}
 *		string = list_get(pcb_actualizar->instrucciones, indice);
 *		char** split = string_split(string, " ");
 *
 *		if(split[0] == "EXIT"){
 *			WAIT SEM MANEJO LISTAS
 *			list_add(procesosExit, pcb_actualizado);
 *			list_remove(procesosExec, 0);
 *			SIGNAL SEM MANEJO LISTAS
 *		}
 *
 *		if(split[0] == "I/O"){
 *			WAIT SEM MANEJO LISTAS
 *			list_add(procesosBlocked, pcb_actualizado);
 *			list_remove(procesosExecute, 0);
 *			SIGNAL SEM MANEJO LISTAS
 *			tiempoBloqueado = list_get(paquetedeCPU_SoloAnalisis, 1) + 1;
 *			list_add(tiempo_bloqueado, tiempoBloqueado);
 *		}
 *
 *		tiempoBloqueado = list_get(tiempoABlockear, 0); // 0
 *		if(tiempoBloqueado){ // si
 *			if(tiempoBloqueado == 0){ // si
 *
 *				// SEMAFORO  CON EL DE LARGO PLAZO MUTEX sem_wait(agregarAReady)
 *				list_add(procesosReady, list_remove(procesosBlocked, 0)); // [ a, c ] => [ c ] ** devuelve un pcb
 *				// SEMAFORO  CON EL DE LARGO PLAZO MUTEX sem_post(agregarAReady)
 *
 *				// se termino el bloqueo por lo tanto lo meto devuelta en la lista de ready
 *				list_remove(tiempo_bloqueado, 0); // [ 0, 4 ] => [ 4 ]
 *
 *			}
 *			list_replace(tiempo_bloqueado, 0, tiempoBloqueado--); // [ 3 ]
 * 		}
 *
 *
 *}
 *




*/

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


