#include "cpu.h"

int main(void)
{
	loggerCpu = iniciar_logger_cpu();
	configCpu = iniciar_config_cpu();

	pthread_mutex_init(&variableCompartida, NULL);
	int cpu_dispatch = iniciar_servidor_dispatch();
	int cpu_interrupt = iniciar_servidor_interrupt();

	pthread_t hiloDispatch, hiloInterrupt;
	pthread_create(&hiloDispatch, NULL, dispatch, cpu_dispatch);
	pthread_create(&hiloInterrupt, NULL, interrupt, cpu_interrupt);
	pthread_join(hiloDispatch, NULL);
	pthread_join(hiloInterrupt, NULL);
}

void dispatch(int escuchaDispatch){
	int kernel_dispatch = esperar_cliente(escuchaDispatch);
	log_info(loggerCpu, "KERNEL DISPATCH INICIALIZADO %i", kernel_dispatch);
	while(1){
		log_info(loggerCpu, "ESPERANDO PCB" );
		PCB* unPCB = deserializarPCB(kernel_dispatch);

//		enviarRespuestaKernel(kernel_dispatch, *unPCB, IO, 3, 5000, loggerCpu);
		int cantidadInstrucciones = list_size(unPCB -> instrucciones);
		rafagaEjecutada = 0;
		//enviarRespuestaKernel(kernel_dispatch, *unPCB, IO, 3, 20000, loggerCpu);
		for(k = 0; k < cantidadInstrucciones; k++){


			t_instruccion* instruccion = fetch(unPCB);
			decode(instruccion, unPCB);
			execute(instruccion, unPCB, kernel_dispatch);

		}

	}
}

void interrupt(int escuchaInterrupt){
	int kernel_interrupt = esperar_cliente(escuchaInterrupt);
	log_info(loggerCpu, "KERNEL INTERRUPT INICIALIZADO %i", kernel_interrupt);
	while(1){
		uint32_t valor = deserializarInt32(kernel_interrupt);
		pthread_mutex_lock(&variableCompartida);
		interrupcionKernel = valor;
		pthread_mutex_unlock(&variableCompartida);
	}
}
