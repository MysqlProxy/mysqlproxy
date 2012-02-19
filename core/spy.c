#include <spy_config.h>
#include <spy_core.h>
#include <spy_process_launcher.h>

#define SPY_MAX_OPTIONS_LEN 10
#define SPY_CONF_PATH "spy.lua"

static spy_int_t spy_show_help = 0;
static spy_int_t spy_show_version = 0;
static u_char *spy_conf_file;
static u_char *spy_prefix;

static spy_int_t spy_get_options(int argc, char * const *argv);
static spy_int_t spy_process_options(spy_global_t *global);

#if 1
int main(int argc, char * const *argv) {

	spy_log_t *log;
	spy_global_t *global, init_global;

	// 初始化错误码
	if (spy_strerror_init() != SPY_OK) {
		spy_log_stderr(0, "%s\n", "init strerror failed.");
		return SPY_ERROR;
	}

	// 初始化选项
	if (spy_get_options(argc, argv) != SPY_OK) {
		spy_log_stderr(0, "%s\n", "init options failed.");
		return SPY_ERROR;
	}

	// 初始化内置日志
	log = spy_log_init(spy_prefix);
	if (log == NULL) {
		spy_log_stderr(0, "%s\n", "init log failed.");
		return SPY_ERROR;
	}

	// 初始化时间
	spy_time_init();

	// 获取进程ID
	spy_pid = spy_getpid();

	spy_memzero(&init_global, sizeof(spy_global_t));
	init_global.log = log;
	spy_global = &init_global;

	// 初始化global pool
	init_global.pool = spy_create_pool(1024, log);
	if (init_global.pool == NULL) {
		return 1;
	}

	// 处理参数
	if (spy_process_options(&init_global) != SPY_OK) {
		return 1;
	}

	// os初始化
	if (spy_os_init(log) != SPY_OK) {
		return 1;
	}

	// 全局初始化
	global = spy_global_init(&init_global);
	spy_global = global;

#if 0
	// 创建PID文件
	if (spy_create_pidfile(&ccf->pid, cycle->log) != SPY_OK) {
		return 1;
	}
#endif

	if (global->log->file->fd != spy_stderr) {

		// 重定向stderr
		if (spy_set_stderr(global->log->file->fd) == SPY_FILE_ERROR) {
			spy_log_error(SPY_LOG_EMERG, global->log, spy_errno, "%s",
					spy_set_stderr_n " failed");
			return 1;
		}
	}

	// 关闭内置日志
	if (log->file->fd != spy_stderr) {
		if (spy_close_file(log->file->fd) == SPY_FILE_ERROR) {
			spy_log_error(SPY_LOG_ALERT, global->log, spy_errno,
					spy_close_file_n " built-in log failed");
		}
	}

	spy_use_stderr = 0;

	// 启动
	spy_single_process_launch(global);

	return SPY_OK;
}

static spy_int_t spy_get_options(int argc, char * const *argv) {

	u_char *p;
	spy_int_t i;

	for (i = 1; i < argc; i++) {

		p = (u_char *) argv[i];

		if (*p++ != '-') {
			spy_log_stderr(0, "invalid option: \"%s\"", argv[i]);
			return SPY_ERROR;
		}

		while (*p) {

			switch (*p++) {

			case 'h':
			case '?':
				spy_show_help = 1;
				break;
			case 'v':
				spy_show_version = 1;
				break;
			case 'l':
				if (*p) {
					SPY_LOG_ERR_PATH = p;
					goto next;
				}

				if (argv[++i]) {
					SPY_LOG_ERR_PATH = (u_char *) argv[i];
					goto next;
				}

				spy_log_stderr(0, "option \"-l\" requires file name");
				return SPY_ERROR;
			case 'p':
				if (*p) {
					spy_prefix = p;
					goto next;
				}

				if (argv[++i]) {
					spy_prefix = (u_char *) argv[i];
					goto next;
				}

				spy_log_stderr(0, "option \"-p\" requires directory name");
				return SPY_ERROR;
			case 'c':
				if (*p) {
					spy_conf_file = p;
					goto next;
				}

				if (argv[++i]) {
					spy_conf_file = (u_char *) argv[i];
					goto next;
				}

				spy_log_stderr(0, "option \"-c\" requires file name");
				return SPY_ERROR;

			default:
				spy_log_stderr(0, "invalid option: \"%s\"", argv[i]);
				return SPY_ERROR;
			}

		}

		next: continue;

	}

	return SPY_OK;
}

static spy_int_t spy_process_options(spy_global_t *global) {
	u_char *p;
	size_t len;

	// 设置spy prefix ，以/结尾
	if (spy_prefix) {
		len = spy_strlen(spy_prefix);
		p = spy_prefix;

		if (!spy_path_separator(*p)) {
			p = spy_pnalloc(global->pool, len + 1);
			if (p == NULL) {
				return SPY_ERROR;
			}

			spy_memcpy(p, spy_prefix, len);
			p[len++] = '/';
		}

		global->conf_prefix.len = len;
		global->conf_prefix.data = p;
		global->prefix.len = len;
		global->prefix.data = p;

	} else {

#ifndef SPY_PREFIX

		p = spy_pnalloc(global->pool, SPY_MAX_PATH);
		if (p == NULL) {
			return SPY_ERROR;
		}

		// 獲取工作目錄
		if (spy_getcwd(p, SPY_MAX_PATH) == 0) {
			spy_log_stderr(spy_errno, "[emerg]: " spy_getcwd_n " failed");
			return SPY_ERROR;
		}

		len = spy_strlen(p);

		p[len++] = '/';

		global->conf_prefix.len = len;
		global->conf_prefix.data = p;
		global->prefix.len = len;
		global->prefix.data = p;

#else
		spy_str_set(&global->conf_prefix, SPY_PREFIX);
		spy_str_set(&global->prefix, SPY_PREFIX);
#endif
	}

	if (spy_conf_file) {
		global->conf_file.len = spy_strlen(spy_conf_file);
		global->conf_file.data = spy_conf_file;

	} else {
		spy_str_set(&global->conf_file, SPY_CONF_PATH)
		;
	}

	if (spy_conf_full_name(global, &global->conf_file, 0) != SPY_OK) {
		return SPY_ERROR;
	}

	for (p = global->conf_file.data + global->conf_file.len - 1; p
			> global->conf_file.data; p--) {
		if (spy_path_separator(*p)) {
			global->conf_prefix.len = p - spy_global->conf_file.data + 1;
			global->conf_prefix.data = spy_global->conf_file.data;
			break;
		}
	}

	return SPY_OK;
}

#endif
