#ifndef QUEUE_H
#define QUEUE_H

#include "thread_worker_types.h"


typedef struct node {
    struct TCB* data;  
    struct node* next;
    struct node* prev;
} node;

typedef struct Queue
{
    struct node* head;
    struct node* tail;
} Queue;



Queue* init_queue();
void enqueue(Queue* queue, struct TCB* temp);
struct TCB* dequeue(Queue* queue);
struct TCB* removeNode(Queue* queue, node* n);
struct TCB* removeTCB(Queue* queue, struct TCB* data);
void printQueue(Queue* queue);
#endif
