/*
 * main.c
 *
 *  Created on: Apr 23, 2022
 *      Author: pmdea
 */
#include "./includeh/memoria.h"

int main(void){


	if(!init() ||!cargar_config() || !cargar_memoria()){
		finalizar_programa();
		return 1;
	}
	// Crear servidor

	//Escuchar para dos clientes: cpu y kernel

	//Devolver mensaje genérico.

	finalizar_programa();
	return 0;

}
