/*
 * memoria_main.c
 *
 *      Author: pmdea
 */
#include "includeh/main.h"

int main(void){


	if(!init() ||!cargar_config() || !cargar_memoria() || !ini_servidor()){
		finalizar_programa();
		return 1;
	}

	while(escuchar_server());

	finalizar_programa();
	return 0;

}
