#ifndef __SIGNALS_H__
# define __SIGNALS_H__

#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

void traitement_signal();

void initialiser_signaux(void);

#endif
