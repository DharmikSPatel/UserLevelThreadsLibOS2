#ifndef MTX_TYPES_H
#define MTX_TYPES_H

#include "thread_worker_types.h"
#include "queue.h"
#include <stdatomic.h>

/* mutex struct definition */
typedef struct worker_mutex_t
{
    atomic_flag __lock;
    Queue* blocked_threads;

} worker_mutex_t;

#endif
