#ifndef QUEUE_H
#define QUEUE_H

#include "thread_worker_types.h"

typedef struct Queue
{
    struct TCB* head;
    struct TCB* tail;
} Queue;

Queue* init_queue();
void enqueue(Queue* queue, struct TCB* temp);
struct TCB* dequeue(Queue* queue);

#endif
