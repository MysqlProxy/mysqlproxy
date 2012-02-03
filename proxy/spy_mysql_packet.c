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

void spy_proxy_init_packet(spy_connection_t *c) {

	spy_event_t *wev;

	wev = c->write;
	wev->handler = spy_proxy_send_acl;

	spy_add_event(wev, SPY_WRITE_EVENT);
}

static void spy_proxy_send_acl(spy_event_t *ev) {

	spy_connection_t *c;
	u_char acl[52], *p, term, filler[10], plugin_data[13];
	spy_uint_t endian, pack_len, pack_num, thread_id, srv_cap_l, srv_cap_u,
			lang, svr_status, buf_len, *i;
	uint64_t buf;

	endian = spy_global->endian;
	p = acl;
	c = ev->data;
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

	/*
	 acl.protocol_version = SPY_MYSQL_PROTOCOL_VERSION;
	 acl.server_version = SPY_MYSQL_SERVER_VERSION;
	 acl.thread_id = t_id;
	 acl.scramble_buff = buf;
	 acl.terminator_1 = '\0';
	 acl.server_capabilities_lower = SPY_MYSQL_SRVER_CAPABILITIES_LOWER;
	 acl.server_language = SPY_MYSQL_SERVER_LANGUAGE;
	 acl.server_status = SPY_MYSQL_SERVER_STATUS;
	 acl.server_capabilities_upper = SPY_MYSQL_SRVER_CAPABILITIES_UPPER;
	 acl.scramble_length = SPY_MYSQL_SCRAMBLE_LENGTH;
	 acl.filler = '\0';
	 acl.plugin_data = "\0\0\0\0\0\0\0\0\0\0\0\0";
	 acl.terminator_2 = '\0';
	 */

	write(c->fd, acl, 52);

	// handle client authention packet
	//c->read->handler = NULL;
	//spy_add_event(c->read, SPY_READ_EVENT);

}
