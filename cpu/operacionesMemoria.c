#include "cpu.h"

// Operaciones con memoria
int leer(direccion_fisica* direccion_fisica)
{
	int tamanioBuffer = sizeof(uint32_t)*4;

	void* buffer = asignarMemoria(tamanioBuffer);

	int desplazamiento = 0;

	concatenarInt32(buffer, &desplazamiento, (uint32_t) 0);
	concatenarInt32(buffer, &desplazamiento, (uint32_t) 2);
	concatenarInt32(buffer, &desplazamiento, direccion_fisica->marco);
	concatenarInt32(buffer, &desplazamiento, direccion_fisica->desplazamiento);

	enviarMensaje(socket_memoria, buffer, tamanioBuffer);

	uint32_t valor = deserializarInt32(socket_memoria);

	free(buffer);
	return valor;
}

void escribir(int valor, direccion_fisica* direccion_fisica)
{

	int tamanioBuffer = sizeof(uint32_t)*5;

	void* buffer = asignarMemoria(tamanioBuffer);

	int desplazamiento = 0;

	concatenarInt32(buffer, &desplazamiento, (uint32_t) 0);
	concatenarInt32(buffer, &desplazamiento, (uint32_t) 3);
	concatenarInt32(buffer, &desplazamiento, direccion_fisica->marco);
	concatenarInt32(buffer, &desplazamiento, direccion_fisica->desplazamiento);
	concatenarInt32(buffer, &desplazamiento, (uint32_t) valor);

	enviarMensaje(socket_memoria, buffer, tamanioBuffer);

	char* stream = deserializarString(socket_memoria);

	if(string_equals_ignore_case(stream, "OK"))
	{
		log_info(loggerCpu, "Valor escirto en memoria con exito.");
	}
	else
	{
		log_error(loggerCpu, "Error en escribir el valor en memoria.");
	}

	free(buffer);
}

void obtener_direccion_logica(int direccion, direccion_logica* direccion_logica)
{
	int tamanio_pagina;
	int cant_entradas_por_tabla;
	obtener_tamanioPag_Entradas(tamanio_pagina,cant_entradas_por_tabla);

	int numero_pagina = floor(direccion / tamanio_pagina);
	direccion_logica->entrada_tabla_1er_nivel = floor(numero_pagina / cant_entradas_por_tabla);
	direccion_logica->entrada_tabla_2do_nivel = numero_pagina % (cant_entradas_por_tabla);
	direccion_logica->desplazamiento = direccion - numero_pagina * tamanio_pagina;

}

void obtener_tamanioPag_Entradas(int tamanio_pagina, int cant_entradas_por_tabla)
{
	int tamanioBuffer = sizeof(uint32_t)*4;
	void* buffer = asignarMemoria(tamanioBuffer);

	int desplazamiento = 0;

	concatenarInt32(buffer, &desplazamiento, (uint32_t) 0);
	concatenarInt32(buffer, &desplazamiento, (uint32_t) 1);


	enviarMensaje(socket_memoria, buffer, tamanioBuffer);


	tamanio_pagina = deserializarInt32(socket_memoria);
	cant_entradas_por_tabla = deserializarInt32(socket_memoria);

	free(buffer);

}

uint32_t obtener_tabla_2do_nivel(int tabla_paginas_1er_nivel, int entrada_pagina_1er_nivel)
{
	int tamanioBuffer = sizeof(uint32_t)*4;
	void* buffer = asignarMemoria(tamanioBuffer);

	int desplazamiento = 0;

	concatenarInt32(buffer, &desplazamiento, (uint32_t) 0);
	concatenarInt32(buffer, &desplazamiento, (uint32_t) 5);
	concatenarInt32(buffer, &desplazamiento, (uint32_t) tabla_paginas_1er_nivel);
	concatenarInt32(buffer, &desplazamiento, (uint32_t) entrada_pagina_1er_nivel);

	enviarMensaje(socket_memoria, buffer, tamanioBuffer);

	uint32_t valor = deserializarInt32(socket_memoria);

	free(buffer);
	return valor;
}

uint32_t obtener_marco(uint32_t tabla_1er_nivel, uint32_t tabla_2do_nivel, uint32_t entrada_2do_nivel)
{
	int tamanioBuffer = sizeof(uint32_t)*5;
	void* buffer = asignarMemoria(tamanioBuffer);

	int desplazamiento = 0;

	concatenarInt32(buffer, &desplazamiento, (uint32_t) 0);
	concatenarInt32(buffer, &desplazamiento, (uint32_t) 6);
	concatenarInt32(buffer, &desplazamiento, tabla_1er_nivel);
	concatenarInt32(buffer, &desplazamiento, tabla_2do_nivel);
	concatenarInt32(buffer, &desplazamiento, entrada_2do_nivel);

	enviarMensaje(socket_memoria, buffer, tamanioBuffer);

	uint32_t valor = deserializarInt32(socket_memoria);

	return valor;
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
		n=3;
	}
	if(string_equals_ignore_case(ident, "EXIT"))
	{
		n=4;
	}
	return n;
}

//traduce direciones logicas en direcciones fisicas
direccion_fisica* MMU(direccion_logica* direccion_logica, pcb* proceso)
{
	direccion_fisica* direccion_fisica;
	uint32_t tabla_2do = obtener_tabla_2do_nivel(proceso->tabla_paginas, direccion_logica->entrada_tabla_1er_nivel);

	if(estaEnTLB(tabla_2do))
	{
		direccion_fisica->marco = TLB(tabla_2do);
	}
	else
	{
		direccion_fisica->marco = obtener_marco(proceso->tabla_paginas, tabla_2do, direccion_logica->entrada_tabla_2do_nivel);
	}

	direccion_fisica->desplazamiento = direccion_logica->desplazamiento;
	return direccion_fisica;
}

int TLB(int pag)
{
	int marco = 0;

	//implementar

	return marco;
}

int estaEnTLB(int pag)
{
	//implementar

	return 0;
}
