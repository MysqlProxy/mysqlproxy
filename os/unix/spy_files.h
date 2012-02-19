#ifndef _SPY_FILES_H_INCLUDED_
#define _SPY_FILES_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>

typedef int spy_fd_t;
typedef struct stat spy_file_info_t;

#define SPY_MAX_PATH             PATH_MAX

#define spy_getcwd(buf, size)    (getcwd((char *) buf, size) != NULL)
#define spy_getcwd_n             "getcwd()"

#define spy_stderr					STDERR_FILENO
#define spy_stdout					STDOUT_FILENO

#define SPY_INVALID_FILE			-1
#define SPY_FILE_ERROR              -1

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

#define spy_open_file(name, mode, create, access)                            \
    open((const char *) name, mode|create, access)

#define spy_open_file_n			"open()"

#define spy_close_file           close
#define spy_close_file_n         "close()"

#define spy_set_stderr(fd)       dup2(fd, STDERR_FILENO)
#define spy_set_stderr_n         "dup2"

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

ssize_t
spy_write_file(spy_file_t *file, u_char *buf, size_t size, off_t offset);
ssize_t spy_read_file(spy_file_t *file, u_char *buf, size_t size, off_t offset);

#endif
