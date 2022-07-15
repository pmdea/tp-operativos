#include "kernel.h"

void generar_PCB(int idPCB, t_proceso* proceso){ // Funcion para cargar los datos del proceso al PCB
    pcb *nuevoProceso = malloc(sizeof(pcb));
    nuevoProceso -> id = idPCB;
    nuevoProceso -> tamanio = proceso -> tamanio_proceso;
    nuevoProceso -> instrucciones = list_create(); // Lista proveniente de consola
    nuevoProceso ->  program_counter = 0;
    nuevoProceso ->  tabla_paginas = (-1); // Esta como (-1) por no tener una tabla asignada.
    nuevoProceso ->  estimacion_rafaga = config_kernel.estimacion_inicial;

    list_add_all(nuevoProceso -> instrucciones, proceso -> instrucciones -> elements);

    list_add(procesosNew, nuevoProceso);

    log_info(loggerKernel, "Se agrego correctamente el Proceso de ID: %i", nuevoProceso -> id);
    //free(nuevoProceso); Si lo descomento rompe la funcion y altera los valores
}

