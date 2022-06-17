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

bool mismo_ID(pcb* pcbFinalizado, conexion_consola* dataConsola){
	return pcbFinalizo -> id = dataConsola -> idPBC;
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
	tiemposBlocked = list_create();
	procesosSuspendedBlocked = list_create();
	procesosSuspendedReady = list_create();
	procesosExit = list_create();
	bool ejecutando  = 0 ;
	// lo puse en el de desalojo paquetedeCPU_Desalojo = list_create();
	// paquetedeCPU_Analisis = list_create();


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



	/* HILOS ENTRE LOS PLANI PARA GENERAR PARALELISMO, esta mal el while aca ?
	 * while (1){
	 *
	 * 	planificador_largo_plazo();
	 *	planificador_corto_plazo('FIFO');
	 * }
	 */

}
//El tamaño y las instrucciones vienen desde consola

/* CODIGO ANTERIOR
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
*/
void generar_PCB(int idPCB, t_proceso* proceso){ // Funcion para cargar los datos del proceso al PCB

	pcb *nuevoProceso = malloc(sizeof(pcb));
	nuevoProceso -> id = idPCB;
	nuevoProceso -> tamanio = proceso -> tamanio_proceso;
	nuevoProceso -> instrucciones = list_create(); // LISTA
	nuevoProceso ->  program_counter = 0;
	nuevoProceso ->  tabla_paginas = "-"; // LISTA
	nuevoProceso ->  estimacion_rafaga = config_kernel.estimacion_inicial;

	list_add_all(nuevoProceso -> instrucciones, proceso -> instrucciones -> elements);

	list_add(procesosNew, nuevoProceso);

	//free(nuevoProceso); Si la libero no me tira bien los datos

	//printf("Proceso creado correctamente");
}
/*
 * sem grado_multiprogramacion 4 //  config.kernel->gradomulti
 * sem prioridad_SuspendedReady 0
 * sem nuevoProcesoReady 0 (binario con cortoplazo)
 *
void planificador_LargoPlazo(){

	while(1){
		if(list_size(procesosSuspendedReady) > 0){

			wait(prioridad_SuspendedReady);

		}else if ( list_size(procesoNew) > 0){

			wait(grado_multiprogramacion);

			pcb * nuevoProceso = list_remove(procesosNew, 0);
			
			//Envio de mensaje a Modulo de Memoria para generar estructuras
			enviarYSerializarStringSinHeader("PCB-GENERADO", socket_memoria);

			//Obtengo las estructuras y se las asigno al PCB
			nuevoProceso -> tabla_paginas = desializarString(socket_memoria);

			wait(nuevoProcesoReady) // binario con cortoplazo
			list_add(procesosReady, nuevoProceso);
			signal(nuevoProcesoReady) // binario con cortoplazo

			free(nuevoProceso);
		}
		if(list_size(procesosExit) > 0 ){

			// Obtengo el PCB que finalizo
			pcb * procesoFinalizado = list_remove(procesosExit, 0);
			
			// Aviso a memoria para que libere
			aviso_a_memoria(socket_memoria, procesoFinalizado);

			// Busco el socket_consola asociado al PCB
			int socket_consola_respuesta = devolverID_CONSOLA(procesoFinalizado);

			// Envio el mensaje de finalización
			char* mensajeFinalizacion = "El proceso ha terminado su ejecucion";
			enviarYSerializarStringSinHeader(socket_consola_respuesta, mensajeFinalizacion);
			
			// Libero memoria
			free(procesoFinalizado);
			
			// Incremento el grado de multiprogramacion en 1
			signal(grado_multiprogramacion);
		}

	}
}
*/

/*
void planificador_CortoPlazo(char algoritmo){

	int ciclosTotales=0;

	if(algoritmo == 'FIFO'){
		algoritmo_FIFO()
	}else{
		algoritmo_SRT()
	}
}
*/
/* ********************************FIFO NUEVO ************************************
void algoritmo_FIFO(){
	administrar_bloqueos(); // HILO
	while(1){
		//sem  mientras la lee no quiero q cambie
		int enReady = list_size(procesosReady); // Procesos en ready
		int enFinalizacion = list_size(procesosExit);
		int enBlock = list_size(procesosBlocked); // Procesos en blocked
		//sem


		wait(nuevoProcesoReady) // a mati pone un proceso en ready

		if(enReady > 0){
			pcb * unProceso;

			//sem_wait(agregarAReady) QUIZA
			unProceso = list_remove(procesosReady, 0);
			//signal sem_post(agregarAReady) QUIZA

			MANDO EL PCB POR CONEXION dispatch unProceso

			// wait espero lo q me manda el cpu desp de ejecutar el proceso (un exit, bloqueo, desalojo) binario
			//wait(respuestaCpu)

			// paquete = recibirPaquete(socket_memoria); SUPONIENDO Q ME MANDA UN MENSAJE EN EL PAQUETE

			RECIBO EL PCB POR CONEXION dispatch

			if( paquete->mensaje == 'exit' ){
				finalizar_pcb(paquete->pcb);
			}else if(paquete->mensaje == 'i/o'){
				list_add(procesosBlocked, paquete->pcb); // EL PRIMERO ES EL Q ESTA BLOQUEADO
				list_add(tiemposBlocked, paquete->tiempo);
			}
		}
	}
}

void finalizar_pcb(pcb * pcbFinalizar){ //hilo
    //fijarme sem
    list_add(procesosExit, pcbFinalizar);
    //fijarme sem
}

void administrar_bloqueos(){ // hilo

    int enBlock = list_size(procesosBlocked);
    if(enBlock > 0){
        pcb * pcb = list_remove(procesosBlocked, 0);
        int tiempo = list_remove(tiemposBlocked, 0);

       	log_info(logger, "Iniciando bloqueo de %d........",tiempo);
        usleep(tiempo);
        log_info(logger, "Finalizando bloqueo");

        // QUIZA
        if(tiempo > config_kernel.tiempo_maximo_bloqueado){
        	signal(bloqueoMax)
        }
        //
        //mutex
        list_add(procesosReady,pcb);
        //mutex
        free(pcb)
    }
}

 **********************************FIN FIFO NUEVO ************************************** */

/*
void algoritmo_SRT(){

	 // a mati pone un proceso en ready

	 * join thread desalojo_PCB()
	 * join thread respuesta_cpu()

}
*/


/* ************************** NUEVO SRT ********************************

void desalojo_PCB() {
	while(1) {
        wait(algo) //  Me bloquea si el cpu me manda un mensaje : )
		//sem  mientras la lee no quiero q cambie
		int enReady = list_size(procesosReady); // Procesos en ready [ B , J ]
		int enBlock = list_size(procesosBlocked); // Procesos en blocked []
		//sem

        //ejecutando variable global un bool  [ a , b]
         *
		wait(nuevoProcesoReady) binario

        if(enReady == 1 && ejecutando == 0){ // la primera vez q ejecuta [ ]

            //EJECUTANDO...
            sem_wait(agregarAReady);
            pcb *pcbExecute = list_remove(procesosReady, 0);
            sem_post(agregarAReady);

			// MANDO EL PCB POR CONEXION dispatch unProceso
            enviar_paquete(pcbExecute, cpu_Dispatch); // serializar
            ejecutando = 1;
            //EJECUTANDO... del cpu

            free(pcbExecute);

            signal(algo) // 1
        }else if (enReady >= 1 && ejecutando == 1){ // si llega un nuevo proceso y esta ejecutando

            enviar_paquete('nuevo proceso', INTERRUP);
            // wait(respuestaCpu)
			paquete = recibirPaquete(dispatch); // deserializar
            int rafagas_executed = paquete->rafagas_executed;
            pcb *pcbExecuted = paquete->pcb;

            estimador(pcbExecuted, 0,5, rafagas_executed); // actualizo pcb

            sem_wait(agregarAReady);
            list_add(procesosReady, pcbExecuted);
            sem_post(agregarAReady);

            list_sort(procesosReady, ordenarSRT); // tomo lista de ready y ordeno con todos los procesos

            free(pcbExecuted);

            //EJECUTANDO...
            sem_wait(agregarAReady);
            pcb *pcbExecute = list_remove(procesosReady, 0);
            sem_post(agregarAReady);

			// MANDO EL PCB POR CONEXION dispatch unProceso
            enviar_paquete(pcbExecute, cpu_Dispatch); // serializar
            ejecutando = 1;
            //EJECUTANDO...

            free(pcbExecute);

            signal(algo)
        }

        signal(algo)
    }
}

void respuesta_cpu(){
	administrar_bloqueos(); // HILO
    while(1){
        wait(Esperar_CPU);
        wait(algo);
        pcb * pcb;
        paquetedeCPU = recibir_paquete(conexion_dispatch); // PCB + Tiempo
        pcb = list_get(paquetedeCPU, 1);
        char instruccion =  list_get(paquetedeCPU, 2);
        int tiempo =  list_get(paquetedeCPU, 3);

        if(instruccion == "EXIT"){
            finalizar_pcb(paquetedeCPU);
        }

        if(instruccion == "I/O"){


            list_add(procesosBlocked, paquete->pcb); // EL PRIMERO ES EL Q ESTA BLOQUEADO
			list_add(tiemposBlocked, tiempo);
        }
        signal(algo)
    }
}


************************** NUEVO SRT FIN ******************************** */


/*
void desalojo_PCB() { // SI LLEGA UN NUEVO PROCESO
	paquetedeCPU_Desalojo = list_create();
	while(1) {
		//sem  mientras la lee no quiero q cambie
		int enReady = list_size(procesosReady); // Procesos en ready [ B , J ]
		int enFinalizacion = list_size(procesosExit); []
		int enBlock = list_size(procesosBlocked); // Procesos en blocked []
		//sem

		if(enReady >= 1){

			if(enReady > 1){
				list_sort(procesosReady, ordenarSRT);  ME ORDENA POR MAS CHICO [ J, B]
			}
			pcb* ready_menorEstimador_PCB;
			menorEstimador_PCB_Ready = list_get(procesosReady, 0); // agarra el pcb mas de rafaga corta [J 3]

			if(enEjecucion > 0) {

				pcb* execute_PCB;
				execute_PCB = list_get(procesosExecute, 0);



				if(menorEstimador_PCB_Ready -> estimacion_rafaga < execute_PCB -> estimacion_rafaga){
				// *********** EJECUCION DEL PROCESO ***********
				 *
					// Creo conexion interrupt cpu - Mando mensaje de desalojo
					 * conexion_cpu_interrupt = crear_conexion(config_kernel.ip_cpu, config_kernel.puerto_cpu_interrupt);
					 * enviar_mensaje("Desalojo", conexion_cpu_interrupt); O ENVIAR UN SIGNAL
					 * wait(esperarDesalojo); Espero desalojo
					 * paquetedeCPU_Desalojo = recibir_paquete(cpu_dispatch); Vincular con las conexiones realizadas
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
					 * Armar un paquete t_paquete* pcbExecute; (Ver como hacer);
					 * enviar_paquete(pcbExecute, cpu_Dispatch); // Se elimina dentro de la funcion luego de ser mandado.
					 *
				}
				free(execute_PCB);
			} else {
                sem_wait(agregarAReady); REIVSAR OJO
                list_add(procesosExecute, menorEstimador_PCB_Ready);
                list_remove(procesosReady, 0);
                sem_post(agregarAReady);

                Este PCB es el que tengo que agregar al paquete -> menorEstimador_PCB_Ready
                Armar un paquete t_paquete* pcbExecute; (Ver como hacer);
                enviar_paquete(pcbExecute, cpu_Dispatch); // Se elimina dentro de la funcion luego de ser mandado.
				EJECUTANDO ...

		}
	free(menorEstimador_PCB_Ready);
	}
}
*/
// SRT Analisis // variable global => conexion_dispatch
 *
 *void interrupcion_io_PCB(){
	 //

 *	paquetedeCPU_Analisis = list_create();
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


