#ifndef _SPY_ERRNO_H_INCLUDED_
#define _SPY_ERRNO_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>

#define SPY_EADDRINUSE    EADDRINUSE

#define spy_errno		errno
#define spy_socket_errno           errno

typedef int spy_err_t;

#define SPY_SYS_NERR  	135

spy_uint_t
spy_strerror_init(void);

u_char *
spy_strerror(spy_err_t err, u_char *errstr, size_t size);

#endif
