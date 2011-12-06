
#ifndef _SPY_LOG_H_INCLUDED_
#define _SPY_LOG_H_INCLUDED_

#include <spy_core.h>
#include <spy_config.h>

#define SPY_LOG_STDERR		0
#define SPY_LOG_STDOUT		1
#define SPY_LOG_INFO		2
#define SPY_LOG_WARN		3
#define SPY_LOG_ERR			4
#define SPY_LOG_DEBUG		5


#define SPY_MAX_ERROR_STR	2048

struct spy_log_s {
	spy_uint_t log_level;
	spy_log_file_t *file;

	//ngx_log_handler_pt   handler;
	void *data;
};


spy_log_t *
spy_log_init(u_char *prefix);


void spy_cdecl
spy_log_stderr(spy_err_t err, const char *fmt, ...);

u_char *spy_log_errno(u_char *buf, u_char *last, spy_err_t err);



#endif
