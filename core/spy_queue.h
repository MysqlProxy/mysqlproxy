#include <spy_config.h>
#include <spy_core.h>

#ifndef _SPY_QUEUE_H_INCLUDED_
#define _SPY_QUEUE_H_INCLUDED_

typedef struct spy_queue_s spy_queue_t;

struct spy_queue_s {
	spy_queue_t *prev;
	spy_queue_t *next;
};

#define spy_queue_init(q)                                                     \
    (q)->prev = q;                                                            \
    (q)->next = q

#define ngx_queue_empty(h)                                                    \
    (h == (h)->prev)

#define spy_queue_insert_head(h, x)                                           \
    (x)->next = (h)->next;                                                    \
    (x)->next->prev = x;                                                      \
    (x)->prev = h;                                                            \
    (h)->next = x

#define ngx_queue_insert_after   ngx_queue_insert_head

#define ngx_queue_insert_tail(h, x)                                           \
    (x)->prev = (h)->prev;                                                    \
    (x)->prev->next = x;                                                      \
    (x)->next = h;                                                            \
    (h)->prev = x

#define spy_queue_head(h)                                                     \
    (h)->next

#define spy_queue_last(h)                                                     \
    (h)->prev

#define spy_queue_sentinel(h)                                                 \
    (h)

#define spy_queue_next(q)                                                     \
    (q)->next

#define spy_queue_prev(q)                                                     \
    (q)->prev

#define spy_queue_remove(x)                                                   \
    (x)->next->prev = (x)->prev;                                              \
    (x)->prev->next = (x)->next

#define spy_queue_add(h, n)                                                   \
    (h)->prev->next = (n)->next;                                              \
    (n)->next->prev = (h)->prev;                                              \
    (h)->prev = (n)->prev;                                                    \
    (h)->prev->next = h;

#define spy_queue_data(q, type, link)                                         \
    (type *) ((u_char *) q - offsetof(type, link))

#endif
