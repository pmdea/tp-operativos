#include "kernel.h"

void planificador_CortoPlazo(){
    char* algoritmo = config_kernel.algoritmo_planificacion;
    pthread_create(&administradorBloqueosHilo, NULL, administrar_bloqueos, NULL);
    // int ciclosTotales=0; NO SE SI VA


    if(string_contains("FIFO", algoritmo)){
    	pthread_create(&ejecucionAlgoritmoHilo, NULL, algoritmo_FIFO, NULL);
        //algoritmo_FIFO();
    }else{
    	//pthread_create(&ejecucionAlgoritmoHilo, NULL, algoritmo_SRT, NULL);
        //algoritmo_SRT();
    }


    pthread_join(administradorBloqueosHilo, NULL);
    pthread_join(ejecucionAlgoritmoHilo, NULL);
}

void administrar_bloqueos(){ // hilo
    while(1){
        sem_wait(&procesoBloqueado);

        pthread_mutex_lock(&mutexBloqueo);
        PCB* procesoBloqueado = list_remove(procesosBlocked, 0);
        int tiempo = list_remove(tiemposBlocked, 0);
        pthread_mutex_unlock(&mutexBloqueo);

        int tiemposBlockedSuspended;

        if(tiempo > config_kernel.tiempo_maximo_bloqueado){
            tiemposBlockedSuspended = tiempo - config_kernel.tiempo_maximo_bloqueado;
            tiempo = config_kernel.tiempo_maximo_bloqueado;
        }

        log_info(loggerKernel, "Iniciando bloqueo del Proceso de ID %i por un tiempo de %d........",procesoBloqueado -> id,tiempo);
        sleep(tiempo/1000);
        log_info(loggerKernel, "Finalizando bloqueo del Proceso de ID %i", procesoBloqueado -> id);

        if(tiemposBlockedSuspended > 0){
        	pthread_mutex_lock(&mutexBloqueoSuspendido);
            list_add(procesosSuspendedBlocked, procesoBloqueado);
            list_add(tiemposBlockedSuspendMax, tiemposBlockedSuspended);
            pthread_mutex_unlock(&mutexBloqueoSuspendido);
            sem_post(&bloqueoMax); // le digo al mediano q hay un proceso en procesosSupendedBlocked
        }else{
        	pthread_mutex_lock(&mutexReady);
            list_add(procesosReady,procesoBloqueado);
            pthread_mutex_unlock(&mutexReady);
            sem_post(&nuevoProcesoReady);
        }

    }

 }
