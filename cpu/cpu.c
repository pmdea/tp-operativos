#include "cpu.h"

int main(void)
{
	loggerCpu = iniciar_logger_cpu();
	configCpu = iniciar_config_cpu();

	int cpu_dispatch = iniciar_servidor_dispatch();
	int cpu_interrupt = iniciar_servidor_interrupt();

//	int kernel_interrupt = esperar_cliente(cpu_interrupt);
//	log_info(loggerCpu, "KERNEL INTERRUPT %i", kernel_interrupt);

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
		unPCB -> id++;
		sleep(1);
		enviarPCB(kernel_dispatch, *unPCB, loggerCpu);
	}
}

void interrupt(int escuchaInterrupt){
	int kernel_interrupt = esperar_cliente(escuchaInterrupt);
	log_info(loggerCpu, "KERNEL INTERRUPT INICIALIZADO %i", kernel_interrupt);
	while(1){
		log_warning(loggerCpu, "ESPERANDO INTERRUPCION");
		sleep(10);
	}
}
