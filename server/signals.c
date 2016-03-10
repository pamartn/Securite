#include "signals.h"


void traitement_signal(){
	int status;
	int pid;
	while ((pid = waitpid(-1,&status,WNOHANG)) > 0); 
}

void initialiser_signaux(void){
	struct sigaction sa;
	sa.sa_handler = traitement_signal;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1){
		perror ("sigaction(SIGCHLD)");
	}
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR){	
		perror("signal");
	}
}
