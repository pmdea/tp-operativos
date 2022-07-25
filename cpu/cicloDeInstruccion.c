#include "cpu.h"

t_instruccion* fetch(PCB unPcb){
	t_instruccion* instruccion = list_get(unPcb.instrucciones,unPcb.program_counter);
	return instruccion;
}

void decode(t_instruccion* instruccion, PCB unPCB)
{
	//direccion_logica* direccion_logica = malloc(sizeof(direccion_logica));

	if(instruccion->identificador == COPY)
	{
		direccion_logica* direccion_logica;
		int direccion = list_remove(instruccion -> parametros -> elements, 1);
		obtener_direccion_logica(direccion, direccion_logica);

		//int valor = fetchOperands(direccion_logica, unPCB);
		//list_add(instruccion->parametros->elements, valor);

	}
}

int fetchOperands(direccion_logica* direccion_logica, PCB unPcb)
{
	///direccion_fisica* direccion_fisica = mmu(direccion_logica, unPcb);
	//int valor = leer(direccion_fisica);

	//return valor;
	return 0;
}

void execute(t_instruccion* instruccion, PCB* proceso, int raf, int socketA)
{
	int num = 0;
	int tiempoBloqueo = 0;
	int rafaga = raf;
	int ident = asignarNumero(instruccion->identificador);
	direccion_fisica* direccion_fisica;
	direccion_logica* direccion_logica;
	int direccion = 0;
	int valor = 0;
	log_warning(loggerCpu, "ESTOY ACA");
	switch (ident){
	case 0:
		num = list_get(instruccion -> parametros -> elements, 0);
		for(int i=0; i<num; i++)
		{
			usleep(config_cpu.retardo_noop);
			log_info(loggerCpu, "Instruccion NO_OP %i", num);
			rafaga++;
		}

		proceso->program_counter +=1;
		break;
	case 1:
		tiempoBloqueo = list_get(instruccion->parametros->elements, 0);
		proceso->program_counter +=1;
		rafaga++;
		log_info(loggerCpu, "Instruccion IO con Bloqueo de %i", tiempoBloqueo);
		enviarRespuestaKernel(socketA, *proceso, IO, rafaga, tiempoBloqueo, loggerCpu);
		j = 99;
		break;

//READ(dirección_lógica)

	case 2:
		direccion = list_get(instruccion -> parametros -> elements, 0);
		obtener_direccion_logica(direccion, direccion_logica);
		//direccion_fisica = mmu(direccion_logica, proceso);
		int leido = leer(direccion_fisica);
		log_info(loggerCpu, "Ejecute Instruccion Read: %i", leido);

		rafaga++;
		proceso->program_counter +=1;

		break;

//WRITE(dirección_lógica, valor)

//COPY(dirección_lógica_destino, dirección_lógica_origen)

	case 3:
		direccion = list_get(instruccion -> parametros -> elements, 0);
		obtener_direccion_logica(direccion, direccion_logica);
		//direccion_fisica = mmu(direccion_logica, proceso);

		valor = list_get(instruccion -> parametros -> elements, 1);
		escribir(valor, direccion_fisica);

		rafaga++;
		proceso->program_counter +=1;

		log_info(loggerCpu, "Ejecute Instruccion WRITE/COPY %i", valor);
		break;

	case 4:
		log_info(loggerCpu, "Instruccion EXIT");
		proceso->program_counter +=1;
		enviarRespuestaKernel(socketA, *proceso, EXIT, rafaga, 0, loggerCpu);
		j = 99;
		break;
	}
}

void checkInterrupt(int rafaga, PCB proceso, int socketA)
{
	pthread_mutex_lock(&interrupcionVariable);
	if(interrupcion==1) /// int interruption() implementado en otra rama
	{
//		enviar_respuesta_kernel(socketA, proceso, rafaga, DESALOJO, 0, loggerCpu);
		//free(*proceso);
		j = 250;
		interrupcion = 0;
	}
	pthread_mutex_unlock(&interrupcionVariable);
}
