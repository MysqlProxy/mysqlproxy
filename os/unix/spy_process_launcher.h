#ifndef _SPY_PROCESS_LAUNCHER_H_INCLUDED_
#define _SPY_PROCESS_LAUNCHER_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>

#define spy_getpid   getpid

extern spy_pid_t spy_pid;

void spy_single_process_launch(spy_global_t *global);

#endif

