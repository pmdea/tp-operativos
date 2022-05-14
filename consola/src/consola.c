/*
 * consola.c
 *
 *  Created on: Apr 23, 2022
 *      Author: pmdea
 */
#include "consola.h"


int main(int argc, char **argv) {
	char* filePath;
	char* fileSize;
	t_log* logger = log_create("console.log", "console", true, LOG_LEVEL_DEBUG);
	// tomar los elementos por parámetros. Se deben recibir:
	// 1: nombre archivo
	// 2: tamaño archivo
	if(argc > 3){
		log_info(logger, "No se recibieron los param correctos");
		return EXIT_FAILURE;
	}
	filePath = argv[1];
	fileSize = argv[2];

	const char* filename = filePath;

	FILE* input_file = fopen(filename, "r");
	//FILE* input_file = fopen("identificadores.txt", "r"); // SI QUERES PROBAR SIN PASAR POR PARAM EL NOMBRE DEL ARCHIVO
	if (!input_file)
		exit(EXIT_FAILURE);

	char *contents = NULL;
	size_t len = 0;

	//t_list* identificadores_list = list_create();

	while (getline(&contents, &len, input_file) != -1){

		char **identificador;
		char **paramA;
		char **paramB;

		asignar_valores(contents, &identificador, &paramA, &paramB);

		if(validar_identificadores(identificador, paramA, paramB) == 0){
			printf("Error en el identificador %s", identificador);
			exit (EXIT_FAILURE);
		}
		// ESTO HAY Q MANDAR --> identificador, paramA, paramB




		//list_add(identificadores_list, element);

	}
	printf("Lei y parsee todo como un campeon");

	fclose(input_file);
	free(contents);
	exit(EXIT_SUCCESS);



}
