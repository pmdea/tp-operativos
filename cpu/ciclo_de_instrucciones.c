#include "cpu.h"

t_instruccion* fetch(PCB* unPcb){
	t_instruccion* instruccion = list_get(unPcb->instrucciones,unPcb->program_counter);
	return instruccion;
}

int fetchOperands(t_direccion_logica* direccion_logica, PCB unPcb, t_config_tabla config)
{
	t_direccion_fisica direccion_fisica = mmu(direccion_logica, unPcb, config);
	int valor = leer(direccion_fisica, direccion_logica->entrada_tabla_2do_nivel);

	return valor;
}

void decode(t_instruccion* instruccion, PCB* unPCB)
{
    if(instruccion->identificador == COPY)
    {
    	t_direccion_logica* direccion_logica = malloc(sizeof(t_direccion_logica));
        int direccion = list_remove(instruccion -> parametros -> elements, 1);
        t_config_tabla config = obtener_direccion_logica(direccion, direccion_logica);

        int valor = fetchOperands(direccion_logica, *unPCB, config);
        list_add(instruccion->parametros->elements, valor);
        //free(direccion_logica);
    }
}

void execute(t_instruccion* instruccion, PCB* proceso, int socketA)
{
	int tiempoBloqueo;
	t_direccion_fisica direccion_fisica;
	t_direccion_logica* direccion_logica = malloc(sizeof(t_direccion_logica));
	t_config_tabla config;
	int direccion;
	int valor;
	int ident = instruccion_a_realizar(instruccion->identificador);
	switch (ident){

	case 0:
		usleep(config_cpu.retardo_noop*1000);
		log_info(loggerCpu, "NO_OP");
		rafagaEjecutada++;
		proceso->program_counter +=1;
		break;
	case 1:
		tiempoBloqueo = list_get(instruccion->parametros->elements, 0);
		proceso->program_counter +=1;
		rafagaEjecutada++;
		log_info(loggerCpu, "IO %i", tiempoBloqueo);
		enviarRespuestaKernel(socketA, *proceso, IO_PCB, rafagaEjecutada, tiempoBloqueo, loggerCpu);
		k = 2000;
		break;

//READ(dirección_lógica)

	case 2:
		direccion = list_get(instruccion -> parametros -> elements, 0);
		config = obtener_direccion_logica(direccion, direccion_logica);
		direccion_fisica = mmu(direccion_logica, *proceso, config);
		int leido = leer(direccion_fisica, direccion_logica->entrada_tabla_2do_nivel);

		log_info(loggerCpu, "Read: %i", leido);

		rafagaEjecutada++;
		proceso->program_counter +=1;

		break;

//WRITE(dirección_lógica, valor)

//COPY(dirección_lógica_destino, dirección_lógica_origen)

	case 3:

		direccion = list_get(instruccion -> parametros -> elements, 0);
		config = obtener_direccion_logica(direccion, direccion_logica);
		direccion_fisica = mmu(direccion_logica, *proceso, config);

		valor = list_get(instruccion -> parametros -> elements, 1);
		escribir(valor, direccion_fisica, direccion_logica->entrada_tabla_2do_nivel);

		rafagaEjecutada++;
		proceso->program_counter +=1;

		log_info(loggerCpu, "WRITE/COPY %i", valor);

		break;

	case 4:
		log_info(loggerCpu, "EXIT");
		proceso->program_counter +=1;
		enviarRespuestaKernel(socketA, *proceso, EXIT_PCB, rafagaEjecutada, 0, loggerCpu);
		k = 2000;
		break;
	}
	free(direccion_logica);
}

void checkInterrupt(PCB* proceso, int socketA)
{
    pthread_mutex_lock(&variableCompartida);
    if(interrupcionKernel==1)
    {
    	enviarRespuestaKernel(socketA, *proceso, DESALOJO_PCB, rafagaEjecutada, 0, loggerCpu);
    	pcb_destroyer(proceso);
        //free(*proceso);
        k = 2000;
        interrupcionKernel = 0;
    }
    pthread_mutex_unlock(&variableCompartida);
}
