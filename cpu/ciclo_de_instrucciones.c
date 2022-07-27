#include "cpu.h"

t_instruccion* fetch(PCB* unPcb){
	t_instruccion* instruccion = list_get(unPcb->instrucciones,unPcb->program_counter);
	return instruccion;
}

int fetchOperands(t_direccion_logica* direccion_logica, PCB unPcb)
{
	t_direccion_fisica* direccion_fisica = mmu(direccion_logica, unPcb);
	int valor = leer(direccion_fisica);

	return valor;
}

void decode(t_instruccion* instruccion, PCB* unPCB)
{
    t_direccion_logica* direccion_logica = malloc(sizeof(direccion_logica));

    if(instruccion->identificador == COPY)
    {
        t_direccion_logica* direccion_logica;
        int direccion = list_remove(instruccion -> parametros -> elements, 1);
        obtener_direccion_logica(direccion, direccion_logica);

        int valor = fetchOperands(direccion_logica, *unPCB);
        list_add(instruccion->parametros->elements, valor);

    }

}

void execute(t_instruccion* instruccion, PCB* proceso, int socketA)
{
	int num;
	int tiempoBloqueo;

	t_direccion_fisica* direccion_fisica;
	t_direccion_logica* direccion_logica;
	int direccion;
	int valor;
	int ident = instruccion_a_realizar(instruccion->identificador);

	switch (ident){
	case 0:
		num = list_get(instruccion -> parametros -> elements, 0);
		for(int i=0; i<num; i++)
		{
			sleep(config_cpu.retardo_noop/1000);
			log_info(loggerCpu, "NO_OP %i", num);
			rafagaEjecutada++;
		}
		proceso->program_counter +=1;
		break;
	case 1:
		tiempoBloqueo = list_get(instruccion->parametros->elements, 0);
		proceso->program_counter +=1;
		rafagaEjecutada++;
		log_info(loggerCpu, "IO %i", tiempoBloqueo);
		enviarRespuestaKernel(socketA, *proceso, IO_PCB, rafagaEjecutada, tiempoBloqueo, loggerCpu);
		k = 2000;
		check = 0;
		break;

//READ(dirección_lógica)

	case 2:
		direccion = list_get(instruccion -> parametros -> elements, 0);
		obtener_direccion_logica(direccion, direccion_logica);
		direccion_fisica = mmu(direccion_logica, *proceso);
		int leido = leer(direccion_fisica);
		log_info(loggerCpu, "Ejecute Instruccion Read: %i", leido);

		rafagaEjecutada++;
		proceso->program_counter +=1;

		break;

//WRITE(dirección_lógica, valor)

//COPY(dirección_lógica_destino, dirección_lógica_origen)

	case 3:
		direccion = list_get(instruccion -> parametros -> elements, 0);
		log_info(loggerCpu, "Direccion %i", direccion);
		obtener_direccion_logica(direccion, direccion_logica);
		log_info(loggerCpu, "Direccion logica %i", direccion_logica);
//		direccion_fisica = mmu(direccion_logica, *proceso);
//
//		valor = list_get(instruccion -> parametros -> elements, 1);
//		escribir(valor, direccion_fisica);

		rafagaEjecutada++;
		proceso->program_counter +=1;

		log_info(loggerCpu, "Ejecute Instruccion WRITE/COPY %i", valor);

		break;

	case 4:
		log_info(loggerCpu, "EXIT");
		proceso->program_counter +=1;
		enviarRespuestaKernel(socketA, *proceso, EXIT_PCB, rafagaEjecutada, 0, loggerCpu);
		k = 2000;
		break;
	}
}

void checkInterrupt(PCB* proceso, int socketA)
{
    if(interrupcionKernel==1)
    {
    	enviarRespuestaKernel(socketA, *proceso, DESALOJO_PCB, rafagaEjecutada, 0, loggerCpu);
        k = 2000;
        interrupcionKernel = 0;
    }
}
