#ifndef _SPY_CORE_H_INCLUDED_
#define _SPY_CORE_H_INCLUDED_

#define SPY_OK					0
#define SPY_ERROR   			-1
typedef struct spy_log_s 		spy_log_t;
typedef struct spy_log_file_s 	spy_log_file_t;


#define LF     (u_char) 10
#define CR     (u_char) 13
#define CRLF   "\x0d\x0a"

#define spy_abs(value)       (((value) >= 0) ? (value) : - (value))
#define spy_min(val1, val2)  ((val1 > val2) ? (val2) : (val1))
#define spy_ldiff(val1, val2) ((val1 >= val2) ? (val1 - val2) : 0)


#include <spy_errno.h>
#include <spy_atomic.h>
#include <spy_string.h>
#include <spy_math.h>
#include <spy_files.h>
#include <spy_file.h>
#include <spy_log.h>



#endif
