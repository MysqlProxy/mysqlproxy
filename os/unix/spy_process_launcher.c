#include <spy_config.h>
#include <spy_core.h>
#include <spy_event.h>

spy_pid_t spy_pid;
sig_atomic_t spy_terminate;
sig_atomic_t spy_quit;
sig_atomic_t spy_reconfigure;
sig_atomic_t spy_reopen;

static spy_global_t spy_exit_global;
static spy_log_t spy_exit_log;
static spy_open_file_t spy_exit_log_file;

static void spy_master_process_exit(spy_global_t *global);

void spy_single_process_launch(spy_global_t *global) {
	//ngx_uint_t i;

	// 事件初始化
	if (spy_event_init(global) != SPY_OK) {
		exit(2);
	}

	for (;;) {
		spy_log_debug(SPY_LOG_DEBUG_EVENT, global->log, 0, "%s", "worker cycle");

		// 处理事件
		spy_process_events_and_timers(global);

		// 退出
		if (spy_terminate || spy_quit) {

			for (i = 0; ngx_modules[i]; i++) {
				if (ngx_modules[i]->exit_process) {
					ngx_modules[i]->exit_process(cycle);
				}
			}

			spy_master_process_exit(cycle);
		}

		// 重新配置
		if (spy_reconfigure) {
			spy_reconfigure = 0;
			spy_log_error(SPY_LOG_NOTICE, global->log, 0, "reconfiguring");

			global = spy_init_global(global);
			if (global == NULL) {
				global = (spy_global_t *) spy_global;
				continue;
			}

			spy_global = global;
		}

		// 重新打开文件
		if (spy_reopen) {
			spy_reopen = 0;
			spy_log_error(SPY_LOG_NOTICE, global->log, 0, "reopening logs");
			//spy_reopen_files(global, (spy_uid_t) - 1);
		}
	}
}

static void spy_master_process_exit(spy_global_t *global) {
	spy_uint_t i;

	//ngx_delete_pidfile(cycle);

	spy_log_error(SPY_LOG_NOTICE, global->log, 0, "exit");

	/* 關閉套接字 */
	spy_close_listening_sockets(global);

	/* 日志转移 */
	spy_exit_log_file.fd = spy_global->log->file->fd;

	spy_exit_log = *spy_global->log;
	spy_exit_log.file = &spy_exit_log_file;

	spy_exit_global.log = &spy_exit_log;
	spy_global = &spy_exit_global;

	/* 消费全局global */
	spy_destroy_pool(global->pool);

	exit(0);
}
