#ifndef _SPY_LOG_H_INCLUDED_
#define _SPY_LOG_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>

/*
 * 日志模块，初始化直接调用init函数
 * 日志级别为一个int 00000000 00000000 00000000 11111111 头8个bit设置基本调式级别
 * 调用log_error记录错误信息
 * SPY_LOG_DEBUG最高级别，设置后，可以打印所有消息，类推下去
 * 自定义调试的级别，从第二个byte开始，也就是0x100，自己定义，调用log_debug函数即打印调试信息
 * 当设置定义级别后，调用debug函数，会对bit进行检查，如果bit = 1 就可以打印对应模块的调式信息
 * 主要调用 spy_log_debug,spy_log_error
 */

#define SPY_LOG_STDERR            0
#define SPY_LOG_EMERG             1
#define SPY_LOG_ALERT             2
#define SPY_LOG_CRIT              3
#define SPY_LOG_ERR               4
#define SPY_LOG_WARN              5
#define SPY_LOG_NOTICE            6
#define SPY_LOG_INFO              7
#define SPY_LOG_DEBUG             8

// 调式core核心模块
#define SPY_LOG_CORE			  0x100

// 日志记录的最大字符串长度
#define SPY_MAX_ERROR_STR	2048

// 日志结构体
struct spy_log_s {
	spy_uint_t log_level;
	spy_log_file_t *file;

	//ngx_log_handler_pt   handler;
	void *data;
};

/*
 * 日志初始化函数，filename设置日志的文件名
 */
spy_log_t *
spy_log_init(u_char *filename);

/*
 * 打印错误码详细信息
 */
u_char *
spy_log_errno(u_char *buf, u_char *last, spy_err_t err);

/*
 * 核心错误信息记录函数
 */
void
spy_log_error_core(spy_uint_t level, spy_log_t *log, spy_err_t err,
		const char *fmt, va_list args);

/*
 * 核心debug函数
 */
void spy_log_debug_core(spy_log_t *log, spy_err_t err, const char *fmt, ...);

/*
 * 错误记录函数
 */
void spy_cdecl
spy_log_error(spy_uint_t level, spy_log_t *log, spy_err_t err, const char *fmt,
		...);

/*
 * log 调试信息函数
 */
void spy_cdecl
spy_log_debug(spy_uint_t level, spy_log_t *log, spy_err_t err, const char* fmt,
		...);

/*
 * 打印到标准错误输出
 */
void
spy_log_stderr(spy_err_t err, const char *fmt, ...);

/*
 * 打印错误到标准输出
 */
void spy_cdecl
spy_log_stdout(const char *fmt, ...);

#endif
