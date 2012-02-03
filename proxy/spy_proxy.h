#ifndef _SPY_PROXY_H_INCLUDED_
#define _SPY_PROXY_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>

typedef struct spy_packet_header_s spy_packet_header_t;
typedef struct spy_init_packet_s spy_init_packet_t;

#include <spy_mysql_packet.h>

spy_int_t spy_proxy_init(spy_global_t *global);

#endif
