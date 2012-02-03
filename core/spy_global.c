#include <spy_config.h>
#include <spy_core.h>
#include <spy_event.h>
#include <spy_proxy.h>

volatile spy_global_t *spy_global;

spy_int_t spy_global_init(spy_global_t *global) {

	spy_int_t res;

	// 初始化配置文件
	res = spy_conf_init(global);
	if (res != SPY_OK) {
		spy_log_error(4, global->log, 0, "%s", "init conf failed!");
		return SPY_ERROR;
	}

	// 代理初始化
	res = spy_proxy_init(global);
	if (res != SPY_OK) {
		spy_log_error(4, global->log, 0, "%s", "init proxy failed!");
		return SPY_ERROR;
	}

	// 打开监听
	res = spy_open_listening_sockets(global);
	if (res != SPY_OK) {
		spy_log_error(4, global->log, 0, "%s", "open listening sockets failed!");
		return SPY_ERROR;
	}

	// 事件初始化
	res = spy_event_init(global);
	if (res != SPY_OK) {
		spy_log_error(4, global->log, 0, "%s", "init event failed!");
		return SPY_ERROR;
	}

	return SPY_OK;
}
