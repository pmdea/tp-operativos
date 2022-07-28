#include "kernel.h"

void planificador_CortoPlazo(){
    char* algoritmo = config_kernel.algoritmo_planificacion;
    pthread_create(&administradorBloqueosHilo, NULL, administrar_bloqueos, NULL);
    // int ciclosTotales=0; NO SE SI VA


    if(string_contains("FIFO", algoritmo)){
    	log_info(loggerKernel, "INICIANDO ALGORITMO FIFO");
    	pthread_create(&ejecucionAlgoritmoHilo, NULL, algoritmo_FIFO, NULL);
        //algoritmo_FIFO();
    }else{
    	log_info(loggerKernel, "INICIANDO ALGORITMO SRT");
    	pthread_create(&ejecucionAlgoritmoHilo, NULL, algoritmo_SRT, NULL);
        //algoritmo_SRT();
    }


    pthread_join(administradorBloqueosHilo, NULL);
    pthread_join(ejecucionAlgoritmoHilo, NULL);
}

void administrar_bloqueos(){
	PCB* procesoBlocked;
	int tiempo;
	int tiemposBlockedSuspended;
    while(1){
        sem_wait(&procesoBloqueado); // Arranca en 0

        pthread_mutex_lock(&mutexBloqueo);
        procesoBlocked = list_remove(procesosBlocked, 0);
        tiempo = list_remove(tiemposBlocked, 0);
        pthread_mutex_unlock(&mutexBloqueo);

        if(tiempo > config_kernel.tiempo_maximo_bloqueado){
            tiemposBlockedSuspended = tiempo - config_kernel.tiempo_maximo_bloqueado;
            tiempo = config_kernel.tiempo_maximo_bloqueado;
        }

        log_info(loggerKernel, "BLOQUEANDO PROCESO ID %i - TIEMPO BLOQUEO %i",procesoBlocked -> id,tiempo);
        sleep(tiempo/1000);
        log_info(loggerKernel, "FINALIZO BLOQUEO PROCESO ID %i", procesoBlocked -> id);

        if(tiemposBlockedSuspended > 0){
        	pthread_mutex_lock(&mutexBloqueoSuspendido);
            list_add(procesosSuspendedBlocked, procesoBlocked);
            list_add(tiemposBlockedSuspendMax, tiemposBlockedSuspended);
            pthread_mutex_unlock(&mutexBloqueoSuspendido);
            sem_post(&bloqueoMax); // AVISO A M
        }else{
        	pthread_mutex_lock(&mutexReady);
            list_add(procesosReady,procesoBlocked);
            pthread_mutex_unlock(&mutexReady);
            sem_post(&nuevoProcesoReady);
        }

    }

 }
