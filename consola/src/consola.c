/*
 * consola.c
 *
 *  Created on: Apr 23, 2022
 *      Author: pmdea
 */
#include "consola.h"


int main(void)
{
	const char* filename = "identificadores.txt";

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
