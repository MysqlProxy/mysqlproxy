#ifndef _SPY_CORE_H_INCLUDED_
#define _SPY_CORE_H_INCLUDED_

#define SPY_OK					0
#define SPY_ERROR   			-1
#define SPY_AGAIN      			-2
#define SPY_DECLINED            -5

typedef struct spy_pool_s spy_pool_t;
typedef struct spy_array_s spy_array_t;
typedef struct spy_open_file_s spy_open_file_t;
typedef struct spy_log_s spy_log_t;
typedef struct spy_file_s spy_file_t;
typedef struct spy_log_file_s spy_log_file_t;
typedef struct spy_connection_s spy_connection_t;
typedef struct spy_listening_s spy_listening_t;
typedef struct spy_global_s spy_global_t;
typedef struct spy_conf_s spy_conf_t;
typedef struct spy_event_s spy_event_t;

typedef void (*spy_connection_handler_pt)(spy_connection_t *c);
typedef void (*spy_event_handler_pt)(spy_event_t *ev);

#define LF     (u_char) 10
#define CR     (u_char) 13
#define CRLF   "\x0d\x0a"

#define spy_abs(value)       (((value) >= 0) ? (value) : - (value))
#define spy_min(val1, val2)  ((val1 > val2) ? (val2) : (val1))
#define spy_ldiff(val1, val2) ((val1 >= val2) ? (val1 - val2) : 0)

#include <spy_errno.h>
#include <spy_atomic.h>
#include <spy_string.h>
#include <spy_queue.h>
#include <spy_math.h>
#include <spy_palloc.h>
#include <spy_array.h>
#include <spy_list.h>
#include <spy_buf.h>
#include <spy_files.h>
#include <spy_file.h>
#include <spy_log.h>
#include <spy_alloc.h>
#include <spy_minheap.h>
#include <spy_rand.h>
#include <spy_time.h>
#include <spy_times.h>
#include <spy_lua.h>
#include <spy_inet.h>
#include <spy_conf_file.h>
#include <spy_rand.h>
#include <spy_socket.h>
#include <spy_global.h>
#include <spy_os.h>
#include <spy_connection.h>
#include <spy_process.h>

#endif
