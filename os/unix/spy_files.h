#ifndef _SPY_FILES_H_INCLUDED_
#define _SPY_FILES_H_INCLUDED_

#include <spy_core.h>
#include <spy_config.h>

typedef int spy_fd_t;

#define spy_stderr					STDERR_FILENO
#define spy_stdout					STDOUT_FILENO
#define SPY_INVALID_FILE			-1

#define SPY_FILE_RDONLY				O_RDONLY
#define SPY_FILE_WRONLY         	O_WRONLY
#define SPY_FILE_RDWR           	O_RDWR
#define SPY_FILE_CREATE_OR_OPEN 	O_CREAT
#define SPY_FILE_OPEN            	0
#define SPY_FILE_TRUNCATE        	O_CREAT|O_TRUNC
#define SPY_FILE_APPEND          	O_WRONLY|O_APPEND
#define SPY_FILE_NONBLOCK        	O_NONBLOCK

#define SPY_FILE_DEFAULT_ACCESS  0644
#define SPY_FILE_OWNER_ACCESS    0600

#define SPY_LINEFEED_SIZE        1

#define spy_open_file(name, create, access)                            \
    open((const char *) name, create, access)

#define spy_open_file_n			"open()"

/*
 static ngx_inline ssize_t
 ngx_write_fd(ngx_fd_t fd, void *buf, size_t n)
 {
 return write(fd, buf, n);
 }
 */

#define spy_write_fd(fd, buf, n)										\
	write(fd, buf, n)

#define spy_console_write				spy_write_fd

#endif
