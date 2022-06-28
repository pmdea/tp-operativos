#include "cpu.h"

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

int obtenerTamanioListaStrings(t_list* lista){
	int respuesta = sizeof(int);
	for(int i = 0; i < lista->elements_count; i++){
		respuesta += sizeof(int) + strlen(list_get(lista,i)) + 1;
	}
	return respuesta;
}

void serializarPCB(void* buffer, int* desplazamiento, Pcb* proceso)
{
	serializarInt(buffer, desplazamiento, proceso->id);
	serializarInt(buffer, desplazamiento, proceso->tamanio);
	serializarInt(buffer, desplazamiento, proceso->program_counter);
	serializarListaString(buffer, desplazamiento, proceso->instrucciones);
	serializarString(buffer, desplazamiento, proceso->tabla_paginas);
	serializarDouble(buffer, desplazamiento, proceso->estimacion_rafaga);
}

void* serializar_paquete(t_paquete* paquete, int tamanio)
{
	void* paquete_a_enviar = malloc(tamanio);
	int desplazamiento = 0;

	memcpy(paquete_a_enviar + desplazamiento, &(paquete->operacion), sizeof(paquete->operacion));
	desplazamiento+= sizeof(paquete->operacion);
	memcpy(paquete_a_enviar + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(paquete_a_enviar + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return paquete_a_enviar;
}

t_paquete* crear_paquete(op_code operacion){
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->operacion = operacion;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
	return paquete;
}

void enviar_exit(Pcb* proceso, int rafaga)
{
	t_paquete* paquete = crear_paquete(EXIT);

	int tam = sizeof(int)+sizeof(int)+sizeof(int)+obtenerTamanioListaStrings(proceso->instrucciones)
			+strlen(proceso->tabla_paginas)+1+sizeof(double)+sizeof(int);
	void* buffer = malloc(tam);

	int desplazamiento = 0;

	serializarPCB(buffer, &desplazamiento, proceso);

	serializarInt(buffer, &desplazamiento, rafaga);

	realloc(paquete->buffer->stream, tam);
	paquete->buffer->stream = buffer;
	paquete->buffer->size=tam;

	int tamanio=sizeof(paquete->operacion)+sizeof(paquete->buffer->size)+paquete->buffer->size;
	void* paquete_a_enviar = serializar_paquete(paquete, tamanio);
	//send(conexion, paquete_a_enviar, tamanio, 0);

	free(paquete);
}

void enviar_IO(Pcb* proceso, int rafaga, int tiempo)
{
	t_paquete* paquete = crear_paquete(IO);

		int tam = sizeof(int)+sizeof(int)+sizeof(int)+obtenerTamanioListaStrings(proceso->instrucciones)
				+strlen(proceso->tabla_paginas)+1+sizeof(double)+sizeof(int)+sizeof(int);
		void* buffer = malloc(tam);

		int desplazamiento = 0;

		serializarPCB(buffer, &desplazamiento, proceso);

		serializarInt(buffer, &desplazamiento, rafaga);

		serializarInt(buffer, &desplazamiento, tiempo);

		realloc(paquete->buffer->stream, tam);
		paquete->buffer->stream = buffer;
		paquete->buffer->size=tam;

		int tamanio=sizeof(paquete->operacion)+sizeof(paquete->buffer->size)+paquete->buffer->size;
		void* paquete_a_enviar = serializar_paquete(paquete, tamanio);
		//send(conexion, paquete_a_enviar, tamanio, 0);

		free(paquete);
}

void enviar_interrupt(Pcb* proceso)
{
	t_paquete* paquete = crear_paquete(INTERRUPT);

		int tam = sizeof(int)+sizeof(int)+sizeof(int)+obtenerTamanioListaStrings(proceso->instrucciones)
				+strlen(proceso->tabla_paginas)+1+sizeof(double);
		void* buffer = malloc(tam);

		int desplazamiento = 0;

		serializarPCB(buffer, &desplazamiento, proceso);

		realloc(paquete->buffer->stream, tam);
		paquete->buffer->stream = buffer;
		paquete->buffer->size=tam;

		int tamanio=sizeof(paquete->operacion)+sizeof(paquete->buffer->size)+paquete->buffer->size;
		void* paquete_a_enviar = serializar_paquete(paquete, tamanio);
		//send(conexion, paquete_a_enviar, tamanio, 0);

		free(paquete);
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

void execute(Instruccion* instruccion, Pcb* proceso, int* raf)
{
	int* num = 0;
	int tiempo = 0;
	int rafaga = raf;
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
		rafaga++;

		enviar_IO(proceso, rafaga, tiempo);

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

		enviar_exit(proceso, rafaga);

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
	if(hayInterrupcion())
	{
		enviar_interrupt(proceso);
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
