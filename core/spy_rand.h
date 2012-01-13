#ifndef _SPY_CORE_H_INCLUDED_
#define _SPY_CORE_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>

u_int64_t spy_rand() {
	return rand() + (rand() << 31);
}

#endif
