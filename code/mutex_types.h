#ifndef MTX_TYPES_H
#define MTX_TYPES_H

#include "thread_worker_types.h"
#include "queue.h"

#define UNLOCKED 0
#define LOCKED 1
/* mutex struct definition */
typedef struct worker_mutex_t
{
    int __lock;
    int __malloced;
    Queue* blocked_threads;

} worker_mutex_t;

#endif
