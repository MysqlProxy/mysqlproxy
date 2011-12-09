#ifndef _SPY_LOG_H_INCLUDED_
#define _SPY_LOG_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>


#define SPY_LOG_STDERR            0
#define SPY_LOG_EMERG             1
#define SPY_LOG_ALERT             2
#define SPY_LOG_CRIT              3
#define SPY_LOG_ERR               4
#define SPY_LOG_WARN              5
#define SPY_LOG_NOTICE            6
#define SPY_LOG_INFO              7
#define SPY_LOG_DEBUG             8

#define SPY_MAX_ERROR_STR	2048

struct spy_log_s {
	spy_uint_t log_level;
	spy_log_file_t *file;

	//ngx_log_handler_pt   handler;
	void *data;
};

/*
 * 日志初始化函数，prefix设置日志的文件名
 */
spy_log_t *
spy_log_init(u_char *filename);

u_char *
spy_log_errno(u_char *buf, u_char *last, spy_err_t err);

void
spy_log_core(spy_uint_t level, spy_log_t *log, spy_err_t err, const char *fmt,
		va_list args);

void spy_cdecl
spy_log_stderr(spy_err_t err, const char *fmt, ...);


/*
 * 打印错误到标准错误输出
 * 错误码，格式，多参
 */
void spy_cdecl
spy_log_stderr(spy_err_t err, const char *fmt, ...);

/*
 * 打印错误到标准输出
 * 格式，多参
 */
void spy_cdecl
spy_log_stdout(const char *fmt, ...);

void spy_cdecl
spy_log_debug(spy_log_t *log, spy_err_t err, const char *fmt, ...);

#endif
