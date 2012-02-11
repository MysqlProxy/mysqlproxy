#ifndef _SPY_EVENT_POSTED_H_INCLUDED_
#define _NGX_EVENT_POSTED_H_INCLUDED_

#include <spy_config.h>
#include <spy_core.h>
#include <spy_event.h>

#define spy_post_event(ev, queue)                                             \
                                                                              \
    if (ev->prev == NULL) {                                                   \
        ev->next = (spy_event_t *) *queue;                                    \
        ev->prev = (spy_event_t **) queue;                                    \
        *queue = ev;                                                          \
                                                                              \
        if (ev->next) {                                                       \
            ev->next->prev = &ev->next;                                       \
        }                                                                     \
                                                                              \
        spy_log_debug(SPY_LOG_DEBUG_CORE, ev->log, 0, "post event %p", ev);   \
                                                                              \
    } else  {                                                                 \
        spy_log_debug(SPY_LOG_DEBUG_CORE, ev->log, 0,                         \
                       "update posted event %p", ev);                         \
    }

#define spy_delete_posted_event(ev)                                           \
                                                                              \
    *(ev->prev) = ev->next;                                                   \
                                                                              \
    if (ev->next) {                                                           \
        ev->next->prev = ev->prev;                                            \
    }                                                                         \
                                                                              \
    ev->prev = NULL;                                                          \
    spy_log_debug(SPY_LOG_DEBUG_CORE, ev->log, 0,                             \
                   "delete posted event %p", ev);

#endif

extern spy_event_t *spy_posted_accept_events;
extern spy_event_t *spy_posted_events;

void spy_event_process_posted(spy_global_t *global, spy_event_t **posted);
