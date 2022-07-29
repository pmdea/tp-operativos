#include "includeh/main.h"

int main(int argc, char **argv){
//git commit -m "Cambié que solo haya delays para swap cuando hay swapeo de pag, agregado del

	char* path_config;
	if(argc < 2){
		path_config = "memoria.config";
	}else{
		path_config = argv[1];
	}

	if(!init(path_config) ||!cargar_config() || !cargar_memoria() || !ini_servidor()){
		finalizar_programa();
		return 1;
	}

	while(escuchar_server());

	finalizar_programa();
	return 0;

}
