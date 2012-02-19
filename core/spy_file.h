#ifndef _SPY_FILE_H_INCLUDED_
#define _SPY_FILE_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>

struct spy_file_s {
	spy_fd_t fd;
	spy_str_t name;
	spy_file_info_t info;

	off_t offset;
	off_t sys_offset;

	spy_log_t *log;

	unsigned valid_info :1;
	unsigned directio :1;
};

#define spy_path_separator(c)    ((c) == '/')

#endif
