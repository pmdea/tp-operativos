#include "cpu.h"

t_instruccion* fetch(pcb* unPcb){
	t_instruccion* instruccion = list_get(unPcb->instrucciones,unPcb->program_counter);
	return instruccion;
}

void decode(t_instruccion* instruccion, pcb* unPCB)
{
	//direccion_logica* direccion_logica = malloc(sizeof(direccion_logica));

	if(string_equals_ignore_case(instruccion->identificador, "COPY"))
	{
		direccion_logica* direccion_logica;
		int direccion = list_remove(instruccion -> parametros -> elements, 1);
		obtener_direccion_logica(direccion, direccion_logica);

		int valor = fetchOperands(direccion_logica, unPCB);
		list_add(instruccion->parametros->elements, valor);

	}
}

int fetchOperands(direccion_logica* direccion_logica, pcb* unPcb)
{
	direccion_fisica* direccion_fisica = mmu(direccion_logica, unPcb);
	int valor = leer(direccion_fisica);

	return valor;
}

void execute(t_instruccion* instruccion, pcb* proceso, int raf, int socketA)
{

	log_info(loggerCpu, "55PCB ID %i a Kernel....", proceso -> id);
	log_info(loggerCpu, "55PCB EST %i de CPU ", proceso -> estimacion_rafaga);
	log_info(loggerCpu, "55PCB TAB %i de CPU ", proceso -> tabla_paginas);

	int num = 0;
	int tiempoBloqueo = 0;
	int rafaga = raf;
	int ident = asignarNumero(instruccion->identificador);
	direccion_fisica* direccion_fisica;
	direccion_logica* direccion_logica;
	int direccion = 0;
	int valor = 0;

	switch (ident){
	case 0:
		num = list_get(instruccion -> parametros -> elements, 0);
		for(int i=0; i<num; i++)
		{
			usleep(config_cpu.retardo_noop);
			log_info(loggerCpu, "Ejecute Instruccion NO_OP %i", num);
			rafaga++;
		}

		proceso->program_counter++;
		break;
	case 1:
		tiempoBloqueo = list_get(instruccion->parametros->elements, 0);
		log_info(loggerCpu, "TIEMPO BLOQUEO %i", tiempoBloqueo);
		proceso->program_counter++;
		rafaga++;

		log_info(loggerCpu, "4444444PCB ID %i a Kernel....", proceso -> id);
		log_info(loggerCpu, "4444444PCB EST %i de CPU ", proceso -> estimacion_rafaga);
		log_info(loggerCpu, "4444444PCB TAB %i de CPU ", proceso -> tabla_paginas);

		enviar_respuesta_kernel(socketA, proceso, rafaga , IO, tiempoBloqueo, loggerCpu);
		j = 99;
		log_info(loggerCpu, "Ejecute Instruccion IO %i", tiempoBloqueo);
		break;

//READ(dirección_lógica)

	case 2:
		direccion = list_get(instruccion -> parametros -> elements, 0);
		obtener_direccion_logica(direccion, direccion_logica);
		direccion_fisica = mmu(direccion_logica, proceso);
		int leido = leer(direccion_fisica);
		log_info(loggerCpu, "Ejecute Instruccion Read: %i", leido);

		rafaga++;
		proceso->program_counter++;

		break;

//WRITE(dirección_lógica, valor)

//COPY(dirección_lógica_destino, dirección_lógica_origen)

	case 3:
		direccion = list_get(instruccion -> parametros -> elements, 0);
		obtener_direccion_logica(direccion, direccion_logica);
		direccion_fisica = mmu(direccion_logica, proceso);

		valor = list_get(instruccion -> parametros -> elements, 1);
		escribir(valor, direccion_fisica);

		rafaga++;
		proceso->program_counter++;

		log_info(loggerCpu, "Ejecute Instruccion WRITE/COPY %i", valor);
		break;

	case 4:
		proceso->program_counter++;
		enviar_respuesta_kernel(socketA, proceso, 0, EXIT, 0, loggerCpu);
		j = 99;
		log_info(loggerCpu, "Ejecute Instruccion EXIT");
		break;
	}
}

void checkInterrupt(int rafaga, pcb* proceso, int cortarEjecucion, int socketA)
{
	pthread_mutex_lock(&interrupcionVariable);
	if(interrupcion==1) /// int interruption() implementado en otra rama
	{
		enviar_respuesta_kernel(socketA, proceso, rafaga, DESALOJO, 0, loggerCpu);
		interrupcion = 0;
	}
	pthread_mutex_unlock(&interrupcionVariable);
}
