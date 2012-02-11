#include <spy_config.h>
#include <spy_core.h>
#include <spy_event.h>
#include <spy_proxy.h>

#define SPY_MYSQL_PROTOCOL_VERSION 0x4A
#define SPY_MYSQL_SERVER_VERSION "5.5.12"
#define SPY_MYSQL_SRVER_CAPABILITIES_LOWER 0xFFF7
#define SPY_MYSQL_SERVER_LANGUAGE 0x21;
#define SPY_MYSQL_SERVER_STATUS 0x0200;
#define SPY_MYSQL_SRVER_CAPABILITIES_UPPER 0x0F80
#define SPY_MYSQL_SCRAMBLE_LENGTH 0x15

static void spy_proxy_send_acl(spy_event_t *e);
static void spy_proxy_free_request(spy_proxy_request_t *r);
static void spy_proxy_close_request(spy_proxy_request_t *r);
static void spy_proxy_close_connection(spy_connection_t *c);
static void spy_proxy_init_request(spy_event_t *rev);
static void spy_proxy_create_acl(spy_proxy_request_t *r);

void spy_proxy_init_packet(spy_connection_t *c) {

	spy_event_t *wev;

	wev = c->write;
	// 发送初始化握手包
	wev->handler = spy_proxy_send_acl;

	// 初始化proxy request
	spy_proxy_init_request(wev);

	wev->handler(wev);
}

static void spy_proxy_init_request(spy_event_t *wev) {

	spy_connection_t *c;
	spy_proxy_request_t *r;

	c = wev->data;
	r = c->data;

	if (r == NULL) {
		r = spy_alloc(sizeof(spy_proxy_request_t), c->log);
		if (r == NULL) {
			spy_proxy_close_connection(c);
			return;
		}
	}

	c->data = r;
	c->sent = 0;

}

static void spy_proxy_send_acl(spy_event_t *ev) {

	spy_proxy_request_t *r;
	spy_connection_t *c;
	ssize_t n, sent;

	c = ev->data;
	r = c->data;

	if (r->init_packet == NULL) {

		r->init_packet = spy_create_temp_buf(52);
		if (r->init_packet == NULL) {
			spy_proxy_close_request(r);
			return;
		}

		spy_proxy_create_acl(r);
	}

	n = SPY_AGAIN;

	for (;;) {

		if (!SPY_AGAIN)
			break;

		if (ev->ready) {
			sent = r->init_packet->end - r->init_packet->pos;
			n = c->send(c, r->init_packet->pos, sent);

		} else {
			n = SPY_AGAIN;
		}

		if (n == SPY_AGAIN) {

			if (spy_handle_write_event(ev, 0) != SPY_OK) {
				spy_proxy_close_request(r);
				return;
			}

			return;
		}

		if (n == 0) {
			spy_log_error(SPY_LOG_INFO, c->log, 0,
					"client closed prematurely connection");
		}

		if (n == 0 || n == SPY_ERROR) {
			c->error = 1;
			spy_proxy_close_request(r);
			return;
		}

		r->init_packet->pos += n;

		// handle client authention packet
		if (sent == n) {
			r->handshake = 1;
			c->read->handler = NULL;
			spy_add_event(c->read, SPY_READ_EVENT);
			return;
		} else
			n = SPY_AGAIN;
	}

	return;
}

static void spy_proxy_create_acl(spy_proxy_request_t *r) {
	u_char *p, term, filler[10], plugin_data[13];
	spy_uint_t endian, pack_len, pack_num, thread_id, srv_cap_l, srv_cap_u,
			lang, svr_status, buf_len, *i;
	uint64_t buf;

	endian = spy_global->endian;
	p = r->init_packet->start;

	buf = spy_rand();
	thread_id = 10;
	pack_len = 52;
	term = '\0';
	pack_num = 0;

	srv_cap_l = SPY_MYSQL_SRVER_CAPABILITIES_LOWER;
	srv_cap_u = SPY_MYSQL_SRVER_CAPABILITIES_UPPER;

	lang = SPY_MYSQL_SERVER_LANGUAGE;

	svr_status = SPY_MYSQL_SERVER_STATUS;

	buf_len = SPY_MYSQL_SCRAMBLE_LENGTH;

	spy_memzero(filler, 10);

	spy_memzero(plugin_data, 13);

	// PACKET LENGTH
	i = endian ? (&pack_len) + 1 : &pack_len;
	p = spy_cpymem(p, i , 3);

	// PACKET NUMBER
	i = endian ? (&pack_num) + 1 : &pack_num;
	p = spy_cpymem(p, i , 1);

	// MYSQL_SERVER_VERSION
	p = spy_cpymem(p, SPY_MYSQL_SERVER_VERSION,
			sizeof(SPY_MYSQL_SERVER_VERSION));

	// THREAD ID
	p = spy_cpymem(p, &thread_id, 4);

	// SCRAMBLE BUFF
	p = spy_cpymem(p, &buf, 8);

	// TERMINATOR
	p = spy_cpymem(p, &term, 1);

	// SRVER CAPABILITIES
	i = endian ? (&srv_cap_l) + 2 : &srv_cap_l;
	p = spy_cpymem(p, i, 2);

	// LANGUAGE
	i = endian ? (&lang) + 3 : &lang;
	p = spy_cpymem(p, i, 1);

	// SERVER STATUS
	i = endian ? (&svr_status) + 2 : &svr_status;
	p = spy_cpymem(p, i, 2);

	// SRVER CAPABILITIES
	i = endian ? (&srv_cap_u) + 2 : &srv_cap_u;
	p = spy_cpymem(p, i, 2);

	// SCRAMBLE LENGTH
	i = endian ? (&buf_len) + 3 : &buf_len;
	p = spy_cpymem(p, i, 1);

	// FILLER
	p = spy_cpymem(p, filler, 10);

	// PLUGIN DATA
	p = spy_cpymem(p, plugin_data, 13);

	// TERMINATOR
	p = spy_cpymem(p, &term, 1);

	r->init_packet->last = p;
}

static void spy_proxy_free_request(spy_proxy_request_t *r) {
	return;
}

static void spy_proxy_close_request(spy_proxy_request_t *r) {
	spy_connection_t *c;

	c = r->connection;
	spy_log_debug(SPY_LOG_DEBUG_PROXY, c->log, 0, "close http connection: %d",
			c->fd);
	spy_proxy_free_request(r);
	spy_close_connection(c);
}

static void spy_proxy_close_connection(spy_connection_t *c) {
	spy_close_connection(c);
}
