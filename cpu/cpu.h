/*
 * cpu.h
 *
 *  Created on: Apr 26, 2022
 *      Author: pmdea
 */

#ifndef CPU_H_
#define CPU_H_

// Includes
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<pthread.h>
// Constantes para leer archivo de configuracion
#define ENTRADAS_TLB "ENTRADAS_TLB"
#define REEMPLAZO_TLB "REEMPLAZO_TLB"
#define RETARDO_NOOP "RETARDO_NOOP"
#define IP_MEMORIA "IP_MEMORIA"
#define IP_KERNEL "IP_KERNEL"
#define PUERTO_MEMORIA "PUERTO_MEMORIA"
#define PUERTO_ESCUCHA_DISPATCH "PUERTO_ESCUCHA_DISPATCH"
#define PUERTO_ESCUCHA_INTERRUPT "PUERTO_ESCUCHA_INTERRUPT"
#define ARCHIVO_CONFIG "cpu.config" //nombre archivo configuracion
#define IP_CPU "127.0.0.1"
t_log* logger;

// Configuracion de logs
#define ARCHIVO_LOG "cpu.log"
#define PROCESS_NAME "CPU"

// Sockets para conservar referencia en todo el programa
int kernel_int_socket;
int kernel_disp_socket;
int mem_socket;

// ServiceNames
#define SRV_KERNEL_DISPATCH "KERNEL_DISPATCH"
#define SRV_KERNEL_INTERRUPT "KERNEL_INTERRUPT"
#define SRV_MEMORY "SRV_MEMORY"

#endif /* CPU_H_ */
