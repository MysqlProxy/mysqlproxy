#ifndef _SPY_RAND_H_INCLUDED_
#define _SPY_RAND_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>

#define spy_rand() rand() + (rand() << 31)

#endif
