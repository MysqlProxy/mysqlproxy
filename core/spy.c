#include <spy_config.h>
#include <spy_core.h>
#include <spy_process_launcher.h>

#define SPY_MAX_OPTIONS_LEN 10

static spy_int_t spy_show_help = 0;
static spy_int_t spy_show_version = 0;
static char *spy_conf_path;

static spy_int_t spy_get_options(int argc, char * const *argv);
#if 1
int main(int argc, char * const *argv) {

	// 日志
	spy_log_t *log;
	spy_global_t *global;

	global = malloc(sizeof(spy_global_t));
	spy_global = global;

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

	global->conf_path = spy_conf_path;
	spy_log_stdout("%s", global->conf_path);

	// 初始化日志
	log = spy_log_init((u_char *) "/home/terry/github/spy-0.0.01/spy.log");
	if (log == NULL) {
		spy_log_stderr(0, "%s\n", "init log failed.");
		return SPY_ERROR;
	}

	global->log = log;

	// 初始化时间
	spy_time_init();

	// 全局初始化
	spy_global_init(global);

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
			case 'c':
				if (*p) {
					spy_conf_path = (char *) p;
					goto next;
				}

				if (argv[++i]) {
					spy_conf_path = (char *) argv[i];
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
#endif
