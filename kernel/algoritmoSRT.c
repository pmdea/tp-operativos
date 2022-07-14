/*
void algoritmo_SRT(){

	 // a mati pone un proceso en ready

	 * join thread desalojo_PCB()
	 * join thread respuesta_cpu()

}
*/


/* ************************** NUEVO SRT ********************************

void desalojo_PCB() {
	while(1) {
        sem_wait(algo) //  Me bloquea si el cpu me manda un mensaje : )

		//sem_wait(agregarAReady)  mientras la lee no quiero q cambie pensar si es necesario
		int enReady = list_size(procesosReady); // Procesos en ready [ B , J ]
		//sem_post(agregarAReady)

        //ejecutando variable global un bool  [ a , b]
         *
		wait(nuevoProcesoReady) binario

        if(enReady == 1 && ejecutando == 0){ // la primera vez q ejecuta [ ]

            //EJECUTANDO...
            sem_wait(agregarAReady);
            pcb *pcbExecute = list_remove(procesosReady, 0);
            sem_post(agregarAReady);

			// MANDO EL PCB POR CONEXION dispatch unProceso
            enviar_paquete(pcbExecute, cpu_Dispatch); // serializar
            ejecutando = 1;
            //EJECUTANDO... del cpu

            free(pcbExecute);

            sem_post(algo) // 1
        }else if (enReady >= 1 && ejecutando == 1){ // si llega un nuevo proceso y esta ejecutando

            enviar_paquete('nuevo proceso', INTERRUP);
            // wait(respuestaCpu)
			paquete = recibirPaquete(dispatch); // deserializar
            int rafagas_executed = paquete->rafagas_executed;
            pcb *pcbExecuted = paquete->pcb;

            estimador(pcbExecuted, 0,5, rafagas_executed); // actualizo pcb

            sem_wait(agregarAReady);
            list_add(procesosReady, pcbExecuted);
            sem_post(agregarAReady);

            list_sort(procesosReady, ordenarSRT); // tomo lista de ready y ordeno con todos los procesos

            free(pcbExecuted);

            //EJECUTANDO...
            sem_wait(agregarAReady);
            pcb *pcbExecute = list_remove(procesosReady, 0);
            sem_post(agregarAReady);

			// MANDO EL PCB POR CONEXION dispatch unProceso
            enviar_paquete(pcbExecute, cpu_Dispatch); // serializar
            ejecutando = 1;
            //EJECUTANDO...

            free(pcbExecute);

            sem_post(algo)
        }

        sem_post(algo)
    }
}

void respuesta_cpu(){
	administrar_bloqueos(); // HILO
    while(1){
        sem_wait(respuestaCpu);
        sem_wait(algo); // no me acuerdo para q era esto @Mati
        pcb * pcb;
        paquetedeCPU = recibir_paquete(conexion_dispatch); // PCB + Tiempo
        pcb = list_get(paquetedeCPU, 1);
        char instruccion =  list_get(paquetedeCPU, 2);
        int tiempo =  list_get(paquetedeCPU, 3);

        if(instruccion == "EXIT"){
            finalizar_pcb(paquetedeCPU);
        }
        if(instruccion == "I/O"){
            list_add(procesosBlocked, paquete->pcb); // EL PRIMERO ES EL Q ESTA BLOQUEADO
			list_add(tiemposBlocked, tiempo);
        }
        sem_post(algo)
    }
}

************************** NUEVO SRT FIN ******************************** */