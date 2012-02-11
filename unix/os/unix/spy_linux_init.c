#include <spy_config.h>
#include <spy_core.h>

spy_os_io_t spy_os_io = { spy_unix_recv, spy_unix_send, 0 };

spy_int_t spy_os_specific_init(spy_log_t *log) {

	return SPY_OK;
}
