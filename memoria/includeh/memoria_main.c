#include "includeh/main.h"

int main(void){


	if(!init() ||!cargar_config() || !cargar_memoria() || !ini_servidor()){
		finalizar_programa();
		return 1;
	}

	escuchar_server();

	finalizar_programa();
	return 0;

}
