/*
 * memoria_main.c
 *
 *      Author: pmdea
 */
#include "includeh/main.h"

int main(void){


	if(!init() ||!cargar_config() || !cargar_memoria() || ini_servidor()){
		finalizar_programa();
		return 1;
	}

	finalizar_programa();
	return 0;

}
