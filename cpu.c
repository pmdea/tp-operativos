#include "cpu.h"

//void serializarChar(void* buffer, int* desplazamiento, char mensaje){
//	memcpy(buffer + *desplazamiento, &mensaje, sizeof(char));
//	*desplazamiento = *desplazamiento + sizeof(char);
//}
//
//void serializarInt(void* buffer, int* desplazamiento, int numero){
//	memcpy(buffer + *desplazamiento, &numero, sizeof(int));
//	*desplazamiento = *desplazamiento + sizeof(int);
//}
//
//void serializarDouble(void* buffer, int* desplazamiento, double numero){
//	memcpy(buffer + *desplazamiento, &numero, sizeof(double));
//	*desplazamiento = *desplazamiento + sizeof(double);
//}
//
//void serializarString(void* buffer, int* desplazamiento, char* mensaje){
//	serializarInt(buffer, desplazamiento, strlen(mensaje) + 1);
//	memcpy(buffer + *desplazamiento, mensaje, strlen(mensaje) + 1);
//	*desplazamiento = *desplazamiento + strlen(mensaje) + 1;
//}
//
//void serializarListaString(void* buffer, int* desplazamiento, t_list* listaArchivos){
//	serializarInt(buffer, desplazamiento, listaArchivos->elements_count);
//	for(int i = 0; i < (listaArchivos->elements_count); i++){
//		serializarString(buffer, desplazamiento, list_get(listaArchivos, i));
//	}
//}
//
//int obtenerTamanioListaStrings(t_list* lista){
//	int respuesta = sizeof(int);
//	for(int i = 0; i < lista->elements_count; i++){
//		respuesta += sizeof(int) + strlen(list_get(lista,i)) + 1;
//	}
//	return respuesta;
//}
//
//void serializarPCB(void* buffer, int* desplazamiento, Pcb* proceso)
//{
//	serializarInt(buffer, desplazamiento, proceso->id);
//	serializarInt(buffer, desplazamiento, proceso->tamanio);
//	serializarInt(buffer, desplazamiento, proceso->program_counter);
//	serializarListaString(buffer, desplazamiento, proceso->instrucciones);
//	serializarInt(buffer, desplazamiento, proceso->tabla_paginas);
//	serializarDouble(buffer, desplazamiento, proceso->estimacion_rafaga);
//}
//
//void* serializar_paquete(t_paquete* paquete, int tamanio)
//{
//	void* paquete_a_enviar = malloc(tamanio);
//	int desplazamiento = 0;
//
//	memcpy(paquete_a_enviar + desplazamiento, &(paquete->operacion), sizeof(paquete->operacion));
//	desplazamiento+= sizeof(paquete->operacion);
//	memcpy(paquete_a_enviar + desplazamiento, &(paquete->buffer->size), sizeof(int));
//	desplazamiento+= sizeof(int);
//	memcpy(paquete_a_enviar + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
//	desplazamiento+= paquete->buffer->size;
//
//	return paquete_a_enviar;
//}
//
//t_paquete* crear_paquete(op_code operacion){
//	t_paquete* paquete = malloc(sizeof(t_paquete));
//	paquete->operacion = operacion;
//	paquete->buffer = malloc(sizeof(t_buffer));
//	paquete->buffer->size = 0;
//	paquete->buffer->stream = NULL;
//	return paquete;
//}
//
//void enviar_exit(Pcb* proceso, int rafaga)
//{
//	t_paquete* paquete = crear_paquete(EXIT);
//
//	int tam = (5*sizeof(int))+obtenerTamanioListaStrings(proceso->instrucciones)+1+sizeof(double);
//	void* buffer = malloc(tam);
//
//	int desplazamiento = 0;
//
//	serializarPCB(buffer, &desplazamiento, proceso);
//
//	serializarInt(buffer, &desplazamiento, rafaga);
//
//	realloc(paquete->buffer->stream, tam);
//	paquete->buffer->stream = buffer;
//	paquete->buffer->size=tam;
//
//	int tamanio=sizeof(paquete->operacion)+sizeof(paquete->buffer->size)+paquete->buffer->size;
//	void* paquete_a_enviar = serializar_paquete(paquete, tamanio);
//	send(conexion, paquete_a_enviar, tamanio, 0);
//
//	free(paquete);
//	free(buffer);
//	free(paquete_a_enviar);
//}
//
//void enviar_IO(Pcb* proceso, int rafaga, int tiempo)
//{
//	t_paquete* paquete = crear_paquete(IO);
//
//	int tam = (6*sizeof(int))+obtenerTamanioListaStrings(proceso->instrucciones)+1+sizeof(double);
//	void* buffer = malloc(tam);
//
//	int desplazamiento = 0;
//
//	serializarPCB(buffer, &desplazamiento, proceso);
//
//	serializarInt(buffer, &desplazamiento, rafaga);
//
//	serializarInt(buffer, &desplazamiento, tiempo);
//
//	realloc(paquete->buffer->stream, tam);
//	paquete->buffer->stream = buffer;
//	paquete->buffer->size=tam;
//
//	int tamanio=sizeof(paquete->operacion)+sizeof(paquete->buffer->size)+paquete->buffer->size;
//	void* paquete_a_enviar = serializar_paquete(paquete, tamanio);
//	send(conexion, paquete_a_enviar, tamanio, 0);
//
//	free(paquete);
//	free(paquete_a_enviar);
//	free(buffer);
//}
//
//void enviar_interrupt(Pcb* proceso)
//{
//	t_paquete* paquete = crear_paquete(INTERRUPT);
//
//		int tam = (4*sizeof(int))+obtenerTamanioListaStrings(proceso->instrucciones)+1+sizeof(double);
//		void* buffer = malloc(tam);
//
//		int desplazamiento = 0;
//
//		serializarPCB(buffer, &desplazamiento, proceso);
//
//		realloc(paquete->buffer->stream, tam);
//		paquete->buffer->stream = buffer;
//		paquete->buffer->size=tam;
//
//		int tamanio=sizeof(paquete->operacion)+sizeof(paquete->buffer->size)+paquete->buffer->size;
//		void* paquete_a_enviar = serializar_paquete(paquete, tamanio);
//		send(conexion, paquete_a_enviar, tamanio, 0);
//
//		free(paquete);
//}
//
//int asignarNumero(char* ident)
//{
//	int n;
//	if(string_equals_ignore_case(ident, "NO_OP"))
//	{
//		n=0;
//	}
//	if(string_equals_ignore_case(ident, "I/O"))
//	{
//		n=1;
//	}
//	if(string_equals_ignore_case(ident, "READ"))
//	{
//		n=2;
//	}
//	if(string_equals_ignore_case(ident, "WRITE"))
//	{
//		n=3;
//	}
//	if(string_equals_ignore_case(ident, "COPY"))
//	{
//		n=3;
//	}
//	if(string_equals_ignore_case(ident, "EXIT"))
//	{
//		n=4;
//	}
//	return n;
//}
//
//int leer(Direccion_fisica* direccion_fisica)
//{
//	uint32_t tam = sizeof(uint32_t) * 3;
//	void* buffer = malloc(tam);
//
//	int desplazamiento = 0;
//
//	serializarInt(buffer, &desplazamiento, 2);
//	serializarInt(buffer, &desplazamiento, direccion_fisica->marco);
//	serializarInt(buffer, &desplazamiento, direccion_fisica->desplazamiento);
//
//	send(socket_memoria, buffer, tam, 0);
//
//	uint32_t* stream=0;
//	recv(socket_memoria, stream, sizeof(uint32_t), MSG_WAITALL);
//
//	int valor=*stream;
//
//	free(buffer);
//	free(stream);
//	return valor;
//}
//
////mando la direccion fisica seguida del numero a escribir
//void escribir(int valor, Direccion_fisica* direccion_fisica)
//{
//	uint32_t tam = sizeof(uint32_t) * 4;
//	void* buffer = malloc(tam);
//
//	int desplazamiento = 0;
//
//	serializarInt(buffer, &desplazamiento, 3);
//	serializarInt(buffer, &desplazamiento, direccion_fisica->marco);
//	serializarInt(buffer, &desplazamiento, direccion_fisica->desplazamiento);
//	serializarInt(buffer, &desplazamiento, valor);
//
//	send(socket_memoria, buffer, tam, 0);
//
//	char* stream = malloc(string_length("OK"));
//
//	recv(socket_memoria, stream, string_length("OK"), MSG_WAITALL);
//
//	if(string_equals_ignore_case(stream, "OK"))
//	{
//		printf("Valor escirto en memoria con exito.");
//	}
//	else
//	{
//		printf("Error en escribir el valor en memoria.");
//	}
//
//	free(buffer);
//	free(stream);
//}
//
//void obtener_config(int* tam_pagina, int* nro_entradas)
//{
//	uint32_t tam = 2 * sizeof(uint32_t);
//	int desplazamiento = 0;
//	void* buffer = malloc(tam);
//	serializarInt(buffer, &desplazamiento, 0);
//	serializarInt(buffer, &desplazamiento, 1);
//
//	send(socket_memoria, buffer, tam, 0);
//
//	void* stream = malloc(tam);
//
//	recv(socket_memoria, stream, tam, MSG_WAITALL);
//
//	desplazamiento = 0;
//
//	memcpy(tam_pagina, stream + desplazamiento, sizeof(uint32_t));
//	desplazamiento+=sizeof(uint32_t);
//	memcpy(nro_entradas, stream + desplazamiento, sizeof(uint32_t));
//
//	free(buffer);
//	free(stream);
//}
//
//uint32_t obtener_tabla_2do_nivel(int tabla_paginas_1er_nivel, int entrada_pagina_1er_nivel)
//{
//	uint32_t tam = sizeof(uint32_t) * 3;
//	void* buffer = malloc(tam);
//
//	int desplazamiento = 0;
//
//	serializarInt(buffer, &desplazamiento, 5);
//	serializarInt(buffer, &desplazamiento, tabla_paginas_1er_nivel);
//	serializarInt(buffer, &desplazamiento, entrada_pagina_1er_nivel);
//
//	send(socket_memoria, buffer, tam, 0);
//
//	uint32_t* stream=0;
//
//	recv(socket_memoria, stream, sizeof(uint32_t), MSG_WAITALL);
//
//	uint32_t valor = *stream;
//
//	free(stream);
//	free(buffer);
//	return valor;
//}
//
//uint32_t obtener_marco(uint32_t tabla_1er_nivel, uint32_t tabla_2do_nivel, uint32_t entrada_2do_nivel)
//{
//	uint32_t tam = sizeof(uint32_t) * 4;
//	void* buffer = malloc(tam);
//
//	int desplazamiento = 0;
//
//	serializarInt(buffer, &desplazamiento, 6);
//	serializarInt(buffer, &desplazamiento, tabla_1er_nivel);
//	serializarInt(buffer, &desplazamiento, tabla_2do_nivel);
//	serializarInt(buffer, &desplazamiento, entrada_2do_nivel);
//
//	send(socket_memoria, buffer, tam, 0);
//
//	uint32_t* stream=0;
//
//	recv(socket_memoria, stream, sizeof(uint32_t), MSG_WAITALL);
//
//	return *stream;
//}
//
//void obtener_direccion_logica(int* direccion, Direccion_logica* direccion_logica)
//{
//	int* tamanio_pagina=0;
//	int* cant_entradas_por_tabla=0;
//	obtener_config(tamanio_pagina,cant_entradas_por_tabla);
//
//	int numero_pagina = floor(*direccion / *tamanio_pagina);
//	direccion_logica->entrada_tabla_1er_nivel = floor(numero_pagina / *cant_entradas_por_tabla);
//	direccion_logica->entrada_tabla_2do_nivel = numero_pagina % (*cant_entradas_por_tabla);
//	direccion_logica->desplazamiento = *direccion - numero_pagina * *tamanio_pagina;
//}

int comparar_elementos_tlb(Entrada_tlb* elem, int pag)
{
	int i=0;

	if(elem->pagina==pag)
	{
		i=1;
	}

	return i;
}

int esta_en_tlb(t_list* tlb, int pag)
{
	int i=0;
	Entrada_tlb* entrada;
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

int TLB(t_list* tlb, int pag)
{
	int marco;
	Entrada_tlb* entrada;
	for(int j=0;j<list_size(tlb);j++)
	{
		entrada = list_get(tlb, j);
		if(entrada->pagina==pag)
		{
			marco=entrada->marco;
			if(string_equals_ignore_case(config_cpu.reemplazo_tlb, "LRU"))
			{
				list_remove(tlb,j);
				list_add(tlb,entrada);
			}
		}
	}

	return marco;
}

void reemplazo_tlb(t_list* tlb, Entrada_tlb* entrada)
{
	list_remove(tlb, 0);
	list_add(tlb, entrada);
}

void agregar_a_TLB(t_list* tlb, int pagina, int marco)
{
	Entrada_tlb* entrada = malloc(sizeof(Entrada_tlb*));
	entrada->marco=marco;
	entrada->pagina=pagina;

	if (list_size(tlb)<4)
	{
		list_add(tlb, entrada);
	}
	else
	{
		reemplazo_tlb(tlb, entrada);
	}
}

//traduce direciones logicas en direcciones fisicas
Direccion_fisica* MMU(Direccion_logica* direccion_logica, Pcb* proceso, t_list* tlb)
{
	Direccion_fisica* direccion_fisica = malloc(sizeof(Direccion_fisica*));

	uint32_t id_2do_nivel = obtener_tabla_2do_nivel(proceso->tabla_paginas, direccion_logica->entrada_tabla_1er_nivel);

	if(esta_en_tlb(tlb,id_2do_nivel))
	{
		direccion_fisica->marco = TLB(tlb,id_2do_nivel);
	}
	else
	{
		direccion_fisica->marco = obtener_marco(proceso->tabla_paginas, id_2do_nivel, direccion_logica->entrada_tabla_2do_nivel);
		agregar_a_TLB(tlb, id_2do_nivel, direccion_fisica->marco);
	}

	direccion_fisica->desplazamiento = direccion_logica->desplazamiento;
	return direccion_fisica;
}

//void execute(Instruccion* instruccion, Pcb* proceso, int* raf, t_list* tlb)
//{
//	int* num = 0;
//	int tiempo = 0;
//	int rafaga = raf;
//	int ident = asignarNumero(instruccion->identificador);
//	Direccion_fisica* direccion_fisica = malloc(sizeof(Direccion_fisica*));
//	Direccion_logica* direccion_logica = malloc(sizeof(Direccion_logica*));
//	int* direccion = 0;
//	int valor = 0;
//
//	switch (ident){
//	case 0:
//		num = queue_peek(instruccion->parametros);
//		for(int i=0; i<*num; i++)
//		{
//			i++;
//			rafaga++;
//		}
//		proceso->program_counter++;
//
//		break;
//	case 1:
//		tiempo = queue_peek(instruccion->parametros);
//		proceso->program_counter++;
//		rafaga++;
//
//		enviar_IO(proceso, rafaga, tiempo);
//
//		break;
//
////READ(dirección_lógica)
//
//	case 2:
//		direccion = queue_peek(instruccion->parametros);
//		obtener_direccion_logica(direccion, direccion_logica);
//		direccion_fisica = MMU(direccion_logica, proceso, tlb);
//		int leido = leer(direccion_fisica);
//		printf("%i", leido);
//
//		rafaga++;
//		proceso->program_counter++;
//
//		break;
//
////WRITE(dirección_lógica, valor)
//
////COPY(dirección_lógica_destino, dirección_lógica_origen)
//
//	case 3:
//		direccion = queue_pop(instruccion->parametros);
//		obtener_direccion_logica(direccion, direccion_logica);
//		direccion_fisica = MMU(direccion_logica, proceso, tlb);
//
//		valor = queue_pop(instruccion->parametros);
//		escribir(valor, direccion_fisica);
//
//		rafaga++;
//		proceso->program_counter++;
//
//		break;
//
//	case 4:
//		proceso->program_counter++;
//
//		enviar_exit(proceso, rafaga);
//
//		break;
//	}
//	free(num);
//	free(direccion_fisica);
//	free(direccion_logica);
//	free(direccion);
//}
//
//int* fetchOperands(Direccion_logica* direccion_logica, Pcb* proceso, t_list* tlb)
//{
//	Direccion_fisica* direccion_fisica = MMU(direccion_logica, proceso, tlb);
//	int valor = leer(direccion_fisica);
//
//	free(direccion_fisica);
//	return valor;
//}
//
//void decode(Instruccion* instruccion, Pcb* proceso, t_list* tlb)
//{
//	Direccion_logica* direccion_logica = malloc(sizeof(Direccion_logica*));
//
//	if(string_equals_ignore_case(instruccion->identificador, "COPY"))
//	{
//		int* aux = queue_pop(instruccion->parametros);
//		int* direccion = queue_pop(instruccion->parametros);
//		obtener_direccion_logica(direccion, direccion_logica);
//
//		queue_push(instruccion->parametros,aux);
//		int* valor = fetchOperands(direccion_logica, proceso, tlb);
//		queue_push(instruccion->parametros, valor);
//
//		free(aux);
//		free(direccion);
//		free(valor);
//	}
//
//	free(direccion_logica);
//}
//
//Instruccion* fetch(Pcb* proceso)
//{
//	Instruccion* instruccion = malloc(sizeof(Instruccion*));
//	instruccion = list_get(proceso->instrucciones,proceso->program_counter);
//
//	return instruccion;
//}
//
////solo para probar
//int interruption()
//{
//	return 0;
//}
//
////solo para probar
//Pcb* dispatch()
//{
//	Pcb* proceso = malloc(sizeof(Pcb*));
//	proceso->instrucciones = list_create();
//	proceso->program_counter = 0;
//	Instruccion* instrucc = malloc(sizeof(Instruccion*));
//	instrucc->identificador = "READ";
//	instrucc->parametros = queue_create();
//	int* num = malloc(sizeof(int));
//	*num = 200;
//	queue_push(instrucc->parametros, num);
//	instrucc->tamanio_id = sizeof(instrucc->identificador);
//	list_add(proceso->instrucciones,instrucc);
//	num = queue_pop(instrucc->parametros);
//
//	return proceso;
//}
//
//void checkInterrupt(int* rafaga, Pcb* proceso)
//{
//	if(interruption()==1)
//	{
//		enviar_interrupt(proceso);
//	}
//}
//
//t_config* iniciar_config(void)
//{
//	t_config* nuevo_config = config_create("cpu.config");
//
//	return nuevo_config;
//}

int main(void)
{
	t_list* tlb = list_create();
	Entrada_tlb* elem;
	elem->pagina=0;
	elem->marco=1;
	list_add(tlb,elem);



//	int* rafaga = 0;
//	int tamanio = 0;
//	int i = 0;
//
//	config_cpu = iniciar_config();
//
//	tamanio = list_size(proceso->instrucciones);
//
//	while(i < tamanio){
//
//	// obtiene la instruccion del pcb
//	Instruccion* instruccion = fetch(proceso);
//
//	// se fija si tiene que buscar operandos en memoria
//	decode(instruccion, proceso, tlb);
//
//	//ejecuta la instruccion
//	execute(instruccion, proceso, rafaga, tlb);
//
//	checkInterrupt(rafaga, proceso);
//
//	i++;
//
//	free(instruccion);
//	}

	list_clean(tlb);

//	rafaga = 0;
//
//	free(tlb);
//	free(config_cpu);
//	free(proceso);
//	free(rafaga);
	return 0;
}
