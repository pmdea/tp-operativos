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
	if(argc < 3){
		log_info(logger, "No se recibieron los param correctos");
		return EXIT_FAILURE;
	}
	filePath = argv[1];
	fileSize = argv[2];

	const char* filename = filePath;

	FILE* input_file = fopen(filename, "r");
	if (!input_file)
		exit(EXIT_FAILURE);

	char *contents = NULL;
	size_t len = 0;
	while (getline(&contents, &len, input_file) != -1){
		printf("%s", contents); // imprime el contenido (contenido = linea q lee)
	}

	fclose(input_file);
	free(contents);

	exit(EXIT_SUCCESS);

}
