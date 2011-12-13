#include <spy_core.h>
#include <spy_config.h>

#ifdef _SPY_UNIT_TEST_

#define SPY_MAX_OPTIONS_LEN 10

spy_str_t spy_options[] = { spy_string("help"), spy_string("version"),
		spy_string("config"), spy_null_string };

spy_int_t spy_show_help = 0;

static spy_int_t spy_get_options(int argc, char * const *argv);

int main(int argc, char * const *argv) {

	// 日志
	spy_log_t *log;

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

	// 初始化日志
	log = spy_log_init((u_char *) "/home/terry/github/spy-0.0.01/spy.log");
	if (log == NULL) {
		spy_log_stderr(0, "%s\n", "init log failed.");
		return SPY_ERROR;
	}

	// 初始化时间
	spy_time_init();

	//spy_log_error(SPY_LOG_EMERG, log, 0, "%s", "test");

	// 主干测试
	int listen_fd, client_fd, mysql_fd, pack_num;
	size_t pack_len, trc;
	ssize_t rc, wc;
	struct sockaddr_in server_addr, client_addr, mysql_addr;
	socklen_t len;
	u_char buf[2048];

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	spy_memzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(3307);
	if (bind(listen_fd, (struct sockaddr *) &server_addr, sizeof(server_addr))
			!= 0) {
		spy_log_stderr(spy_errno, "%s\n", "Spy bind failed.");
		return SPY_ERROR;
	}
	if (listen(listen_fd, 5) != 0) {
		spy_log_stderr(spy_errno, "%s\n", "Spy listen failed.");
		return SPY_ERROR;
	}

	for (;;) {
		len = sizeof(client_addr);
		// 接受到了Client的连接
		client_fd = accept(listen_fd, (struct sockaddr *) &client_addr, &len);
		// 请求mysqld握手
		mysql_fd = socket(AF_INET, SOCK_STREAM, 0);
		spy_memzero(&mysql_addr, sizeof(mysql_addr));
		mysql_addr.sin_family = AF_INET;
		mysql_addr.sin_port = htons(3306);
		inet_pton(AF_INET, "10.20.139.157", &mysql_addr.sin_addr);

		// 和mysql连接
		if (connect(mysql_fd, (struct sockaddr *) &mysql_addr,
				sizeof(mysql_addr)) != 0) {
			spy_log_stderr(spy_errno, "%s\n", "Connect mysql failed.");
			return SPY_ERROR;
		}

		// 读取初始化握手包
		rc = read(mysql_fd, buf, 4);
		if (rc != 4) {
			spy_log_stderr(spy_errno, "%s\n", "Handshake package failed.");
			return SPY_ERROR;
		}
		pack_len = (size_t) buf[0];
		pack_num = (int) buf[3];

		spy_log_stdout("Init package len : %d, package num : %d", pack_len,
				pack_num);

		// 读取指定长度的包
		rc = read(mysql_fd, buf + 4, pack_len);
		if (rc != pack_len) {
			spy_log_stderr(0, "%s\n", "Handshake package failed.");
			return SPY_ERROR;
		}

		// 写回给Client
		wc = write(client_fd, buf, pack_len + 4);
		spy_log_stdout("Send handshake package to client : %d", wc);

		// 读取client 身份验证包
		rc = read(client_fd, buf, 4);
		if (rc != 4) {
			spy_log_stderr(0, "%s\n", "Auth package failed.");
			return SPY_ERROR;
		}

		pack_len = (size_t) buf[0];
		pack_num = (int) buf[3];

		spy_log_stdout("Auth package len : %d, package num : %d", pack_len,
				pack_num);

		// 读取指定长度的包
		rc = read(client_fd, buf + 4, pack_len);
		if (rc != pack_len) {
			spy_log_stderr(0, "%s\n", "Auth package failed.");
			return SPY_ERROR;
		}

		// 写回给mysql
		wc = write(mysql_fd, buf, pack_len + 4);
		spy_log_stdout("Send auth package to mysql : %d", wc);

		// 读取OK或者ERROR 包
		while ((rc = read(mysql_fd, buf + trc, 2048 - trc)) != 0) {
			trc += rc;
		}

		// 写回给client
		wc = write(client_fd, buf, trc);
		spy_log_stdout("Send ok or error package %d to client : %d", trc, wc);

		close(client_fd);

	}
	return 0;
}

static spy_int_t spy_get_options(int argc, char * const *argv) {

	u_char *p, *buf, *last;
	spy_int_t i, j;
	u_char option[SPY_MAX_OPTIONS_LEN];
	last = option + SPY_MAX_OPTIONS_LEN;

	for (i = 1; i < argc; i++) {
		p = (u_char *) argv[i];
		buf = option;

		if (*p++ != '-') {
			spy_log_stderr(0, "invalid option: \"%s\"", argv[i]);
			return SPY_ERROR;
		}

		if (*p++ != '-') {
			switch (*p) {
			case 'h':
				spy_show_help = 1;

			default:
				spy_log_stderr(0, "invalid option: \"%s\"", argv[i]);
				return SPY_ERROR;
			}

		} else {
			while (buf < last && (*buf++ = *p++) != '=') {
			}
			*++buf = '\0';

			for (j = 0; spy_options[j].len; j++) {
				if (spy_strcmp(spy_options[j].data, option) == 0) {
					break;
				}
			}

			switch (j) {
			case 0:
				spy_show_help = 1;
				break;

			default:
				spy_log_stderr(0, "invalid option: \"%s\"", argv[i]);
				return SPY_ERROR;
			}
		}

	}

	return SPY_OK;
}

#endif
