#include "cpu.h"

int main(void)
{
	loggerCpu = iniciar_logger_cpu();
	configCpu = iniciar_config_cpu();

	tlb = list_create();

}
