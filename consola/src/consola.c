/*
 * consola.c
 *
 *  Created on: Apr 23, 2022
 *      Author: pmdea
 */
#include "consola.h"

void asignar_valores(char *string, char **param1, int *param2, int *param3){

	char** split = string_split(string, " ");

	*param1 = split[0];

	if(split[1]){
		*param2 = atoi(split[1]);
	}else{
		*param2 = -1;
	}
	if(split[2]){
		*param3 = atoi(split[2]);
	}else{
		*param3 = -1;
	}
}


int validar_identificadores(char *string, int *param, int *param2){

	if (strcmp(string,"NO_OP") == 0){
		if((param || param == 0) && param2 == -1){
			return 1;
		}
		return 0;
	}

	if (strcmp(string,"I/O") == 0){
		if((param || param == 0) && param2 == -1){
			return 1;
		}
		return 0;
	}

	if (strcmp(string,"READ") == 0){
		if((param || param == 0) && param2 == -1){
			return 1;
		}
		return 0;
	}

	if (strcmp(string,"WRITE") == 0){
		if((param || param == 0) && param2){
			return 1;
		}
		return 0;
	}

	if (strcmp(string,"COPY") == 0){
		if((param || param == 0 )&& param2){
			return 1;
		}
		return 0;
	}
	if (strcmp(string,"EXIT") == 0){
		if(param == -1 && param2 == -1){
			return 1;
		}
		return 0;
	}

	//exit (EXIT_FAILURE);
	return 0;
}


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

	//FILE* input_file = fopen(filename, "r");
	FILE* input_file = fopen("identificadores.txt", "r"); // SI QUERES PROBAR SIN PASAR POR PARAM EL NOMBRE DEL ARCHIVO
	if (!input_file)
		exit(EXIT_FAILURE);

	char *contents = NULL;
	size_t len = 0;

	Instrucciones* instrucciones ;
	instrucciones->instrucciones_list = queue_create();

	while (getline(&contents, &len, input_file) != -1){

		t_list * instruccion ;
		instruccion = list_create();

		char **identificador;
		int *paramA;
		int *paramB;

		asignar_valores(contents, &identificador, &paramA, &paramB);
		if(validar_identificadores(identificador, paramA, paramB) == 0){
			printf("Error en el identificador %s", identificador);
			exit (EXIT_FAILURE);
		}

		// CON STRUCT Instruccion instruccion;
		// CON STRUCT instruccion.identificador = identificador;
		// CON STRUCT instruccion.parametros = list_create();
		// CON STRUCT list_add(instruccion.parametros, paramA);
		// CON STRUCT list_add(instruccion.parametros, paramB);
		// CON STRUCT queue_push(instrucciones->instrucciones_list, instruccion); ESTRUCTURA


		list_add(instruccion, identificador);
		if(paramA > 0 ){
			list_add(instruccion, paramA);
		}
		if(paramB > 0 ){
			list_add(instruccion, paramB);
		}

		queue_push(instrucciones->instrucciones_list, instruccion);

		free(identificador);

	}

	printf("Lei y parsee todo como un campeon");
	printf("cantidad de instrucciones %i", queue_size(instrucciones->instrucciones_list));

	//queue_destroy_and_destroy_elements(instrucciones->instrucciones_list, );
	fclose(input_file);
	free(contents);
	//free(instrucciones);
	exit(EXIT_SUCCESS);



}
