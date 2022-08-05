#include "cpu.h"

int main(int argc, char **argv)
{
	char* path_config;
	if(argc < 2){
		path_config = "memoria.config";
	}else{
		path_config = argv[1];
	}

	loggerCpu = iniciar_logger_cpu();
	configCpu = iniciar_config_cpu(path_config);

	tlb = list_create();

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
	finalizar_programa();
	close(cpu_dispatch);
	close(cpu_interrupt);
}

void dispatch(int escuchaDispatch){
	int kernel_dispatch = esperar_cliente(escuchaDispatch);
	log_info(loggerCpu, "KERNEL DISPATCH INICIALIZADO %i", kernel_dispatch);
	while(1){
		log_info(loggerCpu, "ESPERANDO PCB" );
		PCB* unPCB = deserializarPCB(kernel_dispatch);
		if(unPCB == NULL){
			log_warning(loggerCpu, "Se desconectó del dispatch");
			return;
		}
		int cantidadInstrucciones = list_size(unPCB -> instrucciones);
		rafagaEjecutada = 0;
		for(k = 0; k < cantidadInstrucciones; k++){
			t_instruccion* instruccion = fetch(unPCB);

			decode(instruccion, unPCB);

			execute(instruccion, unPCB, kernel_dispatch);

			switch(instruccion -> identificador){
				case EXIT:
			        pthread_mutex_lock(&variableCompartida);
			        interrupcionKernel = 0;
			        pthread_mutex_unlock(&variableCompartida);
			        pcb_destroyer(unPCB);
					break;
				case IO:
			        pthread_mutex_lock(&variableCompartida);
			        interrupcionKernel = 0;
			        pthread_mutex_unlock(&variableCompartida);
			    	pcb_destroyer(unPCB);
			        break;
				default:
					checkInterrupt(unPCB, kernel_dispatch);
					break;
			}
		}
	}
}

void interrupt(int escuchaInterrupt){
	int kernel_interrupt = esperar_cliente(escuchaInterrupt);
	log_info(loggerCpu, "KERNEL INTERRUPT INICIALIZADO %i", kernel_interrupt);
	while(1){
		uint32_t valor = deserializarInt32(kernel_interrupt);
		if(valor == (uint32_t)(-1)){
			log_warning(loggerCpu, "Se desconectó del interrupt");
			return;
		}
		pthread_mutex_lock(&variableCompartida);
		interrupcionKernel = valor;
		pthread_mutex_unlock(&variableCompartida);
	}
}

void pcb_destroyer(PCB* pcb){
	void instruccion_destroy(t_instruccion* self){
		queue_destroy(self->parametros);
		free(self);
	}
	list_destroy_and_destroy_elements(pcb->instrucciones, (void*)instruccion_destroy);
	free(pcb);
	log_info(loggerCpu, "PCB destruido!");
}

void finalizar_programa(){
	// Logs y Config
	log_info(loggerCpu, "Finalizando CPU");
	log_destroy(loggerCpu);
	pthread_mutex_destroy(&variableCompartida);
	void tlb_destroyer(t_entrada_tlb* self){
		free(self);
	}
	list_clean_and_destroy_elements(tlb, (void*)tlb_destroyer);
	list_destroy(tlb);
	close(socket_memoria);
}
