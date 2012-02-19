#ifndef _SPY_PROCESS_H_INCLUDED_
#define _SPY_PROCESS_H_INCLUDED_

typedef pid_t spy_pid_t;

typedef struct {
	int signo;
	char *signame;
	char *name;
	void (*handler)(int signo);
} spy_signal_t;

extern spy_pid_t spy_pid;

#endif
