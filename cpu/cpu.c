/*
 * cpu.c
 *
 *  Created on: Apr 23, 2022
 *      Author: pmdea
 */

#include "cpu.h"

void enviar_paquete(int socket, t_paquete* paquete)
{
	int tam = sizeof(*paquete->buffer->stream);
	int tam_buffer = paquete->buffer->size;
	memcpy(tam,tam_buffer,sizeof(int));

	int bytes = paquete->buffer->size + 2*sizeof(int);

	send(socket, paquete, bytes, 0);

}

void serializarChar(void* buffer, int* desplazamiento, char mensaje){
	memcpy(buffer + *desplazamiento, &mensaje, sizeof(char));
	*desplazamiento = *desplazamiento + sizeof(char);
}

void serializarInt(void* buffer, int* desplazamiento, int numero){
	memcpy(buffer + *desplazamiento, &numero, sizeof(int));
	*desplazamiento = *desplazamiento + sizeof(int);
}

void serializarDouble(void* buffer, int* desplazamiento, double numero){
	memcpy(buffer + *desplazamiento, &numero, sizeof(double));
	*desplazamiento = *desplazamiento + sizeof(double);
}

void serializarString(void* buffer, int* desplazamiento, char* mensaje){
	serializarInt(buffer, desplazamiento, strlen(mensaje) + 1);
	memcpy(buffer + *desplazamiento, mensaje, strlen(mensaje) + 1);
	*desplazamiento = *desplazamiento + strlen(mensaje) + 1;
}

void serializarListaString(void* buffer, int* desplazamiento, t_list* listaArchivos){
	serializarInt(buffer, desplazamiento, listaArchivos->elements_count);
	for(int i = 0; i < (listaArchivos->elements_count); i++){
		serializarString(buffer, desplazamiento, list_get(listaArchivos, i));
	}
}

void agregar_pcb(Pcb* proceso, int* desplazamiento, t_paquete* paquete){

	serializarString(paquete->buffer, desplazamiento, proceso->tabla_paginas);
	serializarInt(paquete->buffer, desplazamiento, proceso->id);
	serializarInt(paquete->buffer, desplazamiento, proceso->tamanio);
	serializarInt(paquete->buffer, desplazamiento, proceso->program_counter);
	serializarListaString(paquete->buffer, desplazamiento, proceso->instrucciones);
	serializarDouble(paquete->buffer, desplazamiento, proceso->estimacion_rafaga);
}

void agregar_rafaga(int rafaga, int* desplazamiento, t_paquete* paquete){

	serializarInt(paquete->buffer, desplazamiento, rafaga);
}

void agregar_parametro(int param, int* desplazamiento, t_paquete* paquete){

	serializarInt(paquete->buffer, desplazamiento, param);
}

t_paquete* crear_paquete(op_code operacion){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->operacion = operacion;
	return paquete;
}

int asignarNumero(char* ident)
{
	int n;
	if(string_equals_ignore_case(ident, "NO_OP"))
	{
		n=0;
	}
	if(string_equals_ignore_case(ident, "I/O"))
	{
		n=1;
	}
	if(string_equals_ignore_case(ident, "READ"))
	{
		n=2;
	}
	if(string_equals_ignore_case(ident, "WRITE"))
	{
		n=3;
	}
	if(string_equals_ignore_case(ident, "COPY"))
	{
		n=4;
	}
	if(string_equals_ignore_case(ident, "EXIT"))
	{
		n=5;
	}
	return n;
}

void execute(Instruccion* instruccion, Pcb* proceso, int* rafaga)
{
	int* tiempo = 0;
	t_paquete* paquete;
	int* num = 0;
	int ident = asignarNumero(instruccion->identificador);
	switch (ident){
	case 0:
		num = queue_peek(instruccion->parametros);
		for(int i=0; i<*num; i++)
		{
			i++;
			rafaga++;
		}
		proceso->program_counter++;
		break;
	case 1:
		tiempo = queue_peek(instruccion->parametros);
		proceso->program_counter++;

		paquete = crear_paquete(IO);
		int desplazamiento = 0;

		agregar_pcb(proceso, &desplazamiento, paquete);
		agregar_rafaga(*rafaga, &desplazamiento, paquete);
		agregar_parametro(*tiempo, &desplazamiento, paquete);

		enviar_paquete(conexion, paquete);

		rafaga++;
		break;
	case 2:
		// luego se implementa
		break;
	case 3:
		// luego se implementa
		break;
	case 4:
		// luego se implementa
		break;
	case 5:
		proceso->program_counter++;

		paquete = crear_paquete(EXIT);
		desplazamiento = 0;

		agregar_pcb(proceso, &desplazamiento, paquete);
		agregar_rafaga(*rafaga, &desplazamiento, paquete);

		enviar_paquete(conexion, paquete);
		break;
	}
}

//luego se implementa
void* fetchOperands()
{
	return 0;
}

void decode(Instruccion* instruccion)
{
	if(string_equals_ignore_case(instruccion->identificador, "COPY"))
	{
		instruccion->parametros = fetchOperands();
	}
}

Instruccion* fetch(Pcb* proceso)
{
	Instruccion* instruccion = malloc(sizeof(Instruccion*));
	instruccion = list_get(proceso->instrucciones,proceso->program_counter);

	return instruccion;
}

void *interruption(){
	return 0;
}

//implementar con quien hizo interruption
int hayInterrupcion()
{
	int n = 0;
	if(interruption())
	{
		n = 1;
	}
	return n;
}

void checkInterrupt(int* rafaga, Pcb* proceso)
{
	t_paquete* paquete = crear_paquete(INTERRUPT);
	int* desplazamiento = 0;
	if(hayInterrupcion())
	{
		agregar_pcb(proceso, desplazamiento, paquete);
		enviar_paquete(conexion, paquete);
	}
}

int main(void)
{
	Pcb* proceso = malloc(sizeof(Pcb*));
	proceso->instrucciones=list_create();
	proceso->program_counter=0;
	Instruccion* instrucc = malloc(sizeof(Instruccion*));
	instrucc->identificador = "EXIT";
	instrucc->parametros = queue_create();
	instrucc->tamanio_id = sizeof(instrucc->identificador);

	list_add(proceso->instrucciones,instrucc);

	int* rafaga = 0;
	int tamanio = 0;
	int i = 0;

	tamanio = list_size(proceso->instrucciones);

	while(i < tamanio){

	// obtiene la instruccion del pcb
	Instruccion* instruccion = fetch(proceso);
	printf(instruccion->identificador);

	// se fija si tiene que buscar operandos en memoria
	decode(instruccion);

	//ejecuta la instruccion
	execute(instruccion, proceso, rafaga);

	checkInterrupt(rafaga, proceso);

	i++;
	}
	printf("Sali del while");
	rafaga = 0;
	return 0;
}
