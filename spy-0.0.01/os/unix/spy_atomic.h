#ifndef _SPY_ATOMIC_H_INCLUDED_
#define _SPY_ATOMIC_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>

typedef int32_t spy_atomic_int_t;
typedef uint32_t spy_atomic_uint_t;
typedef volatile spy_atomic_uint_t spy_atomic_t;

#include "spy_gcc_atomic_x86.h"

#define spy_trylock(lock)  (*(lock) == 0 && spy_atomic_cmp_set(lock, 0, 1))
#define spy_unlock(lock)    *(lock) = 0

#endif
