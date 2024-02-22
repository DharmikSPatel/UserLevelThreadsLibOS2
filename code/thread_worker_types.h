#ifndef TW_TYPES_H
#define TW_TYPES_H

#include <ucontext.h>

typedef unsigned int worker_t;

// Define the states a worker thread can be in
#define READY 0
#define SCHEDULED 1
#define BLOCKED 2
#define RUNNING 3

typedef struct TCB
{
    worker_t thread_id;         // Unique ID to identify each worker thread //correct
    int thread_status;          // Status of the thread (READY, SCHEDULED, BLOCKED, etc.) //correct and implemented
    ucontext_t thread_context;  // Context needed for running the thread on a CPU
    void* thread_stack;         // Stack for the thread
    int thread_priority;        // Priority of the thread (for MLFQ)
    // Add more fields as needed...

    // YOUR CODE HERE

} tcb;

#endif