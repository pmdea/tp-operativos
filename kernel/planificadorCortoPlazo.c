#include "kernel.h"

void planificador_CortoPlazo(){
	usleep(1);
    char* algoritmo = config_kernel.algoritmo_planificacion;
    pthread_create(&administradorBloqueosHilo, NULL, administrar_bloqueos, NULL);
    // int ciclosTotales=0; NO SE SI VA


    if(string_contains("FIFO", algoritmo)){
    	pthread_create(&ejecucionAlgoritmoHilo, NULL, algoritmo_FIFO, NULL);
        //algoritmo_FIFO();
    }else{
    	//pthread_create(&ejecucionAlgoritmo, NULL, algoritmo_SRT, NULL);
        //algoritmo_SRT();
    }


    pthread_detach(administradorBloqueosHilo);
    pthread_detach(ejecucionAlgoritmoHilo);
}

void administrar_bloqueos(){ // hilo
    while(1){
        sem_wait(&procesoBloqueado);

        sem_wait(&mutexBloqueo);
        pcb* procesoBloqueado = list_remove(procesosBlocked, 0);
        int tiempo = list_remove(tiemposBlocked, 0);
        sem_post(&mutexBloqueo);

        if(tiempo > config_kernel.tiempo_maximo_bloqueado){
            tiemposBlockedSuspended = tiempo - config_kernel.tiempo_maximo_bloqueado;
            tiempo = config_kernel.tiempo_maximo_bloqueado;
        }

        log_info(loggerKernel, "Iniciando bloqueo de %d........",tiempo);
        usleep(tiempo);
        log_info(loggerKernel, "Finalizando bloqueo");

        if(tiemposBlockedSuspended > 0){
            list_add(procesosSuspendedBlocked, procesoBloqueado);
            sem_post(&bloqueoMax); // le digo al mediano q hay un proceso en procesosSupendedBlocked
        }else{
            sem_wait(&mutexReady);
            list_add(procesosReady,procesoBloqueado);
            sem_post(&mutexReady);
        }

    }

 }
