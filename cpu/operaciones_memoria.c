#include "cpu.h"

// Operaciones con memoria
int leer(t_direccion_fisica direccion_fisica, uint32_t entrada_2do_nivel)
{
	int tamanioBuffer = sizeof(uint32_t)*5;
	int tamanioStream = sizeof(uint32_t)*3;

	void* buffer = asignarMemoria(tamanioBuffer);

	int desplazamiento = 0;

	concatenarInt32(buffer, &desplazamiento, (uint32_t) 2);
	concatenarInt32(buffer, &desplazamiento, (uint32_t) tamanioStream);
	concatenarInt32(buffer, &desplazamiento, direccion_fisica.id_2do_nivel);
	concatenarInt32(buffer, &desplazamiento, entrada_2do_nivel);
	concatenarInt32(buffer, &desplazamiento, direccion_fisica.direccion);

	enviarMensaje(socket_memoria, buffer, tamanioBuffer);

	log_info(loggerCpu, "ESPERANDO RECIBIR RESPUESTA DE MEMORIA");

	uint32_t valor = deserializarInt32(socket_memoria);

	free(buffer);
	return valor;
}

void escribir(int valor, t_direccion_fisica direccion_fisica, uint32_t entrada_2do_nivel)
{

	int tamanioBuffer = sizeof(uint32_t)*6;
	int tamanioStream = sizeof(uint32_t)*4;

	void* buffer = asignarMemoria(tamanioBuffer);

	int desplazamiento = 0;

	concatenarInt32(buffer, &desplazamiento, (uint32_t) 3);
	concatenarInt32(buffer, &desplazamiento, (uint32_t) tamanioStream);
	concatenarInt32(buffer, &desplazamiento, direccion_fisica.id_2do_nivel);
	concatenarInt32(buffer, &desplazamiento, entrada_2do_nivel);
	concatenarInt32(buffer, &desplazamiento, direccion_fisica.direccion);
	concatenarInt32(buffer, &desplazamiento, (uint32_t) valor);

	enviarMensaje(socket_memoria, buffer, tamanioBuffer);

	log_info(loggerCpu, "ESPERANDO RECIBIR OK DE MEMORIA");

	char* stream = malloc(sizeof(char)*3);

	recv(socket_memoria, stream, sizeof(char)*3, MSG_WAITALL);

	if(string_equals_ignore_case(stream, "OK"))
	{
		log_info(loggerCpu, "Valor escirto en memoria con exito.");
	}
	else
	{
		log_error(loggerCpu, "Error en escribir el valor en memoria. %s", stream);
	}

	free(stream);
	free(buffer);
}

t_config_tabla obtener_direccion_logica(int direccion, t_direccion_logica* direccion_logica)
{
	t_config_tabla* config = obtener_tamanioPag_Entradas();

	int numero_pagina = floor(direccion / config->tamanio_pagina);
	direccion_logica->entrada_tabla_1er_nivel = floor(numero_pagina / config->cantidad_entradas);
	direccion_logica->entrada_tabla_2do_nivel = numero_pagina % (config->cantidad_entradas);
	direccion_logica->desplazamiento = direccion - numero_pagina * config->tamanio_pagina;

	t_config_tabla retornar = *config;
	free(config);
	return retornar;
}

t_config_tabla* obtener_tamanioPag_Entradas()
{
	t_config_tabla* config = malloc(sizeof(t_config_tabla));
	int tamanioBuffer = sizeof(uint32_t)*2;
	int tamanioStream = 0;

	void* buffer = asignarMemoria(tamanioBuffer);

	int desplazamiento = 0;

//	concatenarInt32(buffer, &desplazamiento, (uint32_t) 0);
	concatenarInt32(buffer, &desplazamiento, (uint32_t) 1);
	concatenarInt32(buffer, &desplazamiento, (uint32_t) tamanioStream);

	send(socket_memoria, buffer, tamanioBuffer, 0);

	log_info(loggerCpu, "ESPERANDO RECIBIR RESPUESTA DE MEMORIA");

	int tamanioBuffer2 = sizeof(uint32_t)*2;
	void* buffer2 = asignarMemoria(tamanioBuffer2);
	log_debug(loggerCpu, "TAMAÑO RESPUESTA ESPERADA: %i",tamanioBuffer2);
	recv(socket_memoria, buffer2, tamanioBuffer2, MSG_WAITALL);

	memcpy(&config->tamanio_pagina, buffer2, sizeof(uint32_t));
	memcpy(&config->cantidad_entradas, buffer2 + sizeof(uint32_t), sizeof(uint32_t));

	free(buffer);
	free(buffer2);
	return(config);
}

uint32_t obtener_tabla_2do_nivel(int tabla_paginas_1er_nivel, int entrada_pagina_1er_nivel)
{
	int tamanioBuffer = sizeof(uint32_t)*4;
	int tamanioStream = sizeof(uint32_t)*2;

	void* buffer = asignarMemoria(tamanioBuffer);

	int desplazamiento = 0;

	concatenarInt32(buffer, &desplazamiento, (uint32_t) 5);
	concatenarInt32(buffer, &desplazamiento, (uint32_t) tamanioStream);
	concatenarInt32(buffer, &desplazamiento, (uint32_t) tabla_paginas_1er_nivel);
	concatenarInt32(buffer, &desplazamiento, (uint32_t) entrada_pagina_1er_nivel);

	send(socket_memoria, buffer, tamanioBuffer, 0);

	log_info(loggerCpu, "ESPERANDO RECIBIR id tabla 2do Nivel DE MEMORIA");

	uint32_t* valor = malloc(sizeof(uint32_t));
	recv(socket_memoria, valor, sizeof(uint32_t), MSG_WAITALL);
	int retornar = *valor;

	free(buffer);
	free(valor);
	return retornar;
}

uint32_t obtener_marco(uint32_t tabla_1er_nivel, uint32_t tabla_2do_nivel, uint32_t entrada_2do_nivel)
{
	int tamanioBuffer = sizeof(uint32_t)*5;
	int tamanioStream = sizeof(uint32_t)*3;

	void* buffer = asignarMemoria(tamanioBuffer);

	int desplazamiento = 0;

	concatenarInt32(buffer, &desplazamiento, (uint32_t) 6);
	concatenarInt32(buffer, &desplazamiento, (uint32_t) tamanioStream);
	concatenarInt32(buffer, &desplazamiento, tabla_1er_nivel);
	concatenarInt32(buffer, &desplazamiento, tabla_2do_nivel);
	concatenarInt32(buffer, &desplazamiento, entrada_2do_nivel);

	send(socket_memoria, buffer, tamanioBuffer, 0);

	log_info(loggerCpu, "ESPERANDO RECIBIR Marco DE MEMORIA");

	uint32_t* valor = malloc(sizeof(uint32_t));
	recv(socket_memoria, valor, sizeof(uint32_t), MSG_WAITALL);

	int retornar = *valor;

	free(buffer);
	free(valor);
	return retornar;
}

int comparar_elementos_tlb(t_entrada_tlb* elem, int pag)
{
	int i=0;

	if(elem->pagina==pag)
	{
		i=1;
	}

	return i;
}

int esta_en_tlb(int pag)
{
	int i=0;
	t_entrada_tlb* entrada;
	for(int j=0;j<list_size(tlb);j++)
	{
		entrada = list_get(tlb, j);
		if(entrada->pagina==pag)
		{
			i=1;
		}
	}
	return i;
}

int tlb_cache(int pag)
{
	int marco = 0;
	t_entrada_tlb* entrada;

	for(int j=0;j<list_size(tlb);j++)
	{
		entrada = list_get(tlb, j);

		if(entrada->pagina==pag)
		{
			marco=entrada->marco;

			if(string_equals_ignore_case(config_cpu.reemplazo_tlb, "LRU"))
			{
				list_remove(tlb,j);
				list_add(tlb, entrada);
			}

		j=list_size(tlb);
		}
	}
	log_info(loggerCpu, "Se ha encontardo en la TLB el marco %i correpondiente a la página %i.", marco, pag);
	return marco;
}

void reemplazo_tlb(t_entrada_tlb* entrada)
{
	t_entrada_tlb* removido = list_remove(tlb, 0);
	list_add(tlb, entrada);
	log_info(loggerCpu, "No habia espacio, reemplacé la entrada con página %i y marco %i.", removido->pagina, removido->marco);
	free(removido);
}

void agregar_a_TLB(int pagina, int marco)
{
	t_entrada_tlb* entrada = malloc(sizeof(t_entrada_tlb));
	entrada->marco=marco;
	entrada->pagina=pagina;

	if (list_size(tlb)<config_cpu.entradas_tlb)
	{
		list_add(tlb, entrada);
		log_info(loggerCpu, "La entrada con página %i y marco %i ha sido agregada. No fue necesario reemplazar.", entrada->pagina, entrada->marco);
	}
	else
	{
		reemplazo_tlb(entrada);
		log_info(loggerCpu, "La entrada con página %i y marco %i ha sido agregada.", entrada->pagina, entrada->marco);
	}
}

//traduce direciones logicas en direcciones fisicas
t_direccion_fisica mmu(t_direccion_logica* direccion_logica, PCB proceso, t_config_tabla config)
{
	t_direccion_fisica* direccion_fisica = malloc(sizeof(t_direccion_fisica));
	uint32_t id_2do_nivel = obtener_tabla_2do_nivel(proceso.tabla_paginas, direccion_logica->entrada_tabla_1er_nivel);

	log_info(loggerCpu, "ENTRE A MMU %i", id_2do_nivel);

	if(esta_en_tlb(id_2do_nivel))
	{
		log_info(loggerCpu, "El id está en la TLB: %i", id_2do_nivel);
		direccion_fisica->marco = tlb_cache(id_2do_nivel);
	}
	else
	{
		log_info(loggerCpu, "El id no está en la TLB: %i", id_2do_nivel);
		direccion_fisica->marco = obtener_marco(proceso.tabla_paginas, id_2do_nivel, direccion_logica->entrada_tabla_2do_nivel);
		agregar_a_TLB(id_2do_nivel, direccion_fisica->marco);
	}

	direccion_fisica->desplazamiento = direccion_logica->desplazamiento;
	log_info(loggerCpu, "Direccion fisica obtenida (Marco: %i, Desplazamiento: %i)", direccion_fisica->marco, direccion_fisica->desplazamiento);
	direccion_fisica->direccion = direccion_fisica->marco * config.tamanio_pagina + direccion_fisica->desplazamiento;
	direccion_fisica->id_2do_nivel = id_2do_nivel;

	t_direccion_fisica retornar = *direccion_fisica;
	free(direccion_fisica);
	return retornar;
}
