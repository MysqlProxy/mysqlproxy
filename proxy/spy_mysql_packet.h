#ifndef _SPY_MYSQL_PACKET_H_INCLUDED_
#define _SPY_MYSQL_PACKET_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>
#include <spy_event.h>
#include <spy_proxy.h>

typedef struct spy_proxy_request_s spy_proxy_request_t;

struct spy_proxy_request_s {

	spy_buf_t *init_packet;
	spy_connection_t *connection;

	unsigned handshake :1;
};

struct spy_packet_header_s {

	char packet_length[3];
	char packet_number;

};

#if 0
struct spy_init_packet_s {
	spy_packet_header_t header;
	char protocol_version;
	char *server_version;
	spy_uint_t thread_id;
	uint64_t scramble_buff;
	char terminator_1;
	char server_capabilities_lower[2];
	char server_language;
	char server_status[2];
	char server_capabilities_upper[2];
	u_char scramble_length;
	char filler;
	char *plugin_data;
	char terminator_2;
};
#endif

void spy_proxy_init_packet(spy_connection_t *c);

#endif
