#ifndef _MY_SIGNAL_H
#define _MY_SIGNAL_H 1
#include <signal.h>
typedef void    Sigfunc(int);
extern Sigfunc *my_signal(int signo, Sigfunc *func);
#endif
