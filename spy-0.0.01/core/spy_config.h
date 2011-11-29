
#ifndef _SPY_CONFIG_INCLUDE_H_
#define _SPY_CONFIG_INCLUDE_H_

#include <spy_linux_config.h>



typedef intptr_t		spy_int_t;
typedef uintptr_t       spy_uint_t;
typedef intptr_t        spy_flag_t;

#define spy_cdecl
#define SPY_LONG_SIZE		  (sizeof(long))


#define SPY_MAX_UINT32_VALUE  	(uint32_t) 0xffffffff
#define SPY_INT64_LEN   		sizeof("-9223372036854775808") - 1
#define SPY_DOUBLE_FRAC_LEN 	(size_t) 16


#ifndef spy_inline
#define spy_inline      inline
#endif

#endif
