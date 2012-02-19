#ifndef _SPY_CONFIG_INCLUDE_H_
#define _SPY_CONFIG_INCLUDE_H_

#include <spy_linux_config.h>

typedef intptr_t spy_int_t;
typedef uintptr_t spy_uint_t;
typedef intptr_t spy_flag_t;

#define spy_cdecl
#define SPY_LONG_SIZE		  (sizeof(long))

#define SPY_MAX_UINT32_VALUE  	(uint32_t) 0xffffffff
#define SPY_INT64_LEN   		sizeof("-9223372036854775808") - 1
#define SPY_DOUBLE_FRAC_LEN 	(size_t) 16
#define SPY_PRT_SIZE			(sizeof(uintptr_t))

#ifndef SPY_ALIGNMENT
#define SPY_ALIGNMENT   sizeof(unsigned long)    /* platform word */
#endif

#define spy_align(d, a)     (((d) + (a - 1)) & ~(a - 1))
#define spy_align_ptr(p, a)                                                   \
    (u_char *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))

#ifndef spy_inline
#define spy_inline      inline
#endif

#endif
