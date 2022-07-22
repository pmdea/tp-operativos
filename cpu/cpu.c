#include "cpu.h"


int main(void)
{
	loggerCpu = iniciar_logger_cpu();
	configCpu = iniciar_config_cpu();
	tlb = list_create();

	void init_cpu();

	pthread_t thread_dispath, thread_interrupt;
	pthread_mutex_init(&interrupcionVariable, NULL);
	log_info(loggerCpu, "Starting dispatch thread...");
	pthread_create(&thread_dispath, NULL, dispatch, NULL);
	log_info(loggerCpu, "Starting interrupt thread...");
	pthread_create(&thread_interrupt, NULL, interruption, NULL);

	pthread_join(thread_interrupt, NULL);
	pthread_join(thread_dispath, NULL);


}
