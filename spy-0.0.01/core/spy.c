#include <spy_core.h>
#include <spy_config.h>

#ifdef _SPY_UNIT_TEST_

int main() {

	spy_log_init("spy.log");
	exit(EXIT_SUCCESS);
}
#endif
