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
#include<commons/config.h>
#include <string.h>

char *identificadores[6] = {"NO_OP","I/O","READ","WRITE","COPY","EXIT"};

void asignar_valores(char *string, char **param1, char **param2, char **param3){

	char** split = string_split(string, " ");
	*param1 = split[0];
	*param2 = split[1];
	*param3 = split[2];

	/* Version facha

	 char *token;
    int j;

    for (j = 1; ; j++, string = NULL) {
        token = strtok(string," ");
        if (token == NULL)
            break;
        if( j == 1){
        	//strcpy(param1, token);
        	*param1 = token;

        }else if( j ==2 ){
        	//strcpy(param2, token);
        	*param2 = token;
        }else{
        	if( token != NULL){
        		//strcpy(param3,token);
        		*param3 = token;
        	}
        }
        //printf("%d: %s\n", j, token);
    }

    //exit(EXIT_SUCCESS);
    */

}


int validar_identificadores(char *string, char **param, char **param2){

	if (strcmp(string,"NO_OP") == 0){
		if(param != NULL  && param2 == NULL){

			return 1;
		}
		return 0;
	}

	if (strcmp(string,"I/O") == 0){
		if(param != NULL  && param2 == NULL){
			return 1;
		}
		return 0;
	}

	if (strcmp(string,"READ") == 0){
		if(param != NULL  && param2 == NULL){
			return 1;
		}
		return 0;
	}

	if (strcmp(string,"WRITE") == 0){
		if(param != NULL  && param2 != NULL){
			return 1;
		}
		return 0;
	}

	if (strcmp(string,"COPY") == 0){
		if(param != NULL  && param2 != NULL){
			return 1;
		}
		return 0;
	}
	if (strcmp(string,"EXIT") == 0){
		printf("identificador= %s", string);
		if(param == NULL && param2 == NULL){
			return 1;
		}
		return 0;
	}
	if (strcmp(string," ") == 0 || strcmp(string,"\n") == 0){
		return 0;
	}

	//exit (EXIT_FAILURE);
	return 0;
}


#endif /* CONSOLA_H_ */

