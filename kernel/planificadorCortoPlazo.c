#include "kernel.h"
#include "utils.h"

void planificadorCortoPlazo();


void planificadorCortoPlazo(){
    char* algoritmo = config_kernel.algoritmo_planificacion;
	// int ciclosTotales=0; NO SE SI VA

	if(algoritmo == 'FIFO'){
		algoritmo_FIFO()
	}else{
		algoritmo_SRT()
	}
}
