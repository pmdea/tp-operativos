#include "cpu.h"

int main(void)
{
	loggerCpu = iniciar_logger_cpu();
	configCpu = iniciar_config_cpu();

	pthread_mutex_init(&variableCompartida, NULL);
	socket_memoria = crear_conexion(config_cpu.ip_memoria, config_cpu.puerto_memoria);
	conectar_a_memoria(socket_memoria);
	sleep(1);
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

		int cantidadInstrucciones = list_size(unPCB -> instrucciones);
		rafagaEjecutada = 0;
		check = 1;
		for(k = 0; k < cantidadInstrucciones; k++){
			t_instruccion* instruccion = fetch(unPCB);

			decode(instruccion, unPCB);

			execute(instruccion, unPCB, kernel_dispatch);

			if(check == 1){
				pthread_mutex_lock(&variableCompartida);
				checkInterrupt(unPCB, kernel_dispatch);
				pthread_mutex_unlock(&variableCompartida);
			} else{
				pthread_mutex_unlock(&variableCompartida);
				interrupcionKernel = 0;
				pthread_mutex_unlock(&variableCompartida);
			}

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
