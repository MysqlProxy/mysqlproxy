
#ifndef _SPY_CORE_H_INCLUDED_
#define _SPY_CORE_H_INCLUDED_

#include <spy_string.h>
#include <spy_math.h>

#define LF     (u_char) 10
#define CR     (u_char) 13
#define CRLF   "\x0d\x0a"

#define spy_abs(value)       (((value) >= 0) ? (value) : - (value))
#define spy_min(val1, val2)  ((val1 > val2) ? (val2) : (val1))
#define spy_ldiff(val1, val2) ((val1 >= val2) ? (val1 - val2) : 0)

#endif
