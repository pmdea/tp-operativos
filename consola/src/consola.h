/*
 * consola.h
 *
 *  Created on: 23 abr. 2022
 *      Author: utnso
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/collections/list.h>
#include<commons/config.h>
#include <string.h>

//char *identificadores[6] = {"NO_OP","I/O","READ","WRITE","COPY","EXIT"};

void asignar_valores(char *string, char **param1, char **param2, char **param3){

	char** split = string_split(string, " ");

	*param1 = split[0];

	if(split[1]){
		*param2 = atoi(split[1]);
	}else{
		*param2 = NULL;
	}
	if(split[2]){
		*param3 = atoi(split[2]);
	}else{
		*param3 = NULL;
	}
}


int validar_identificadores(char *string, char **param, char **param2){

	if (strcmp(string,"NO_OP") == 0){
		if((param || param == 0) && param2 == NULL){
			return 1;
		}
		return 0;
	}

	if (strcmp(string,"I/O") == 0){
		if((param || param == 0) && param2 == NULL){
			return 1;
		}
		return 0;
	}

	if (strcmp(string,"READ") == 0){
		if((param || param == 0) && param2 == NULL){
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
		if(param == NULL && param2 == NULL){
			return 1;
		}
		return 0;
	}

	//exit (EXIT_FAILURE);
	return 0;
}


#endif /* CONSOLA_H_ */

