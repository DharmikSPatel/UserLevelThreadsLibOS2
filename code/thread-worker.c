// File:	thread-worker.c

// List all group member's name:
/*
 */
// username of iLab:
// iLab Server:

#include "queue.h"
#include "thread-worker.h"
#include "thread_worker_types.h"

//CHANGE: back to SIGSTKSZ 
#define STACK_SIZE sysconf (_SC_SIGSTKSZ)
#define QUANTUM 10 * 1000 //10 milliseconds (time that it runs)
//misc data structs
tcb* running = NULL;
Queue* ready_q = NULL;

worker_t __next_tid = 0;

ucontext_t sch_ctx;
// INITIALIZE ALL YOUR OTHER VARIABLES HERE
int init_scheduler_done = 0;

void my_error(const char* msg, char* file, int line){
    perror("PERROR");
    fprintf(stderr,"MY LIB ERROR %s %s@%d\n", msg, file, line);
    exit(1);
}
#define error(p) my_error(p, __FILE__, __LINE__)
/* scheduler */

static void sched_rr();
static void sched_mlfq();
static void schedule()
{
    printf("IN SCEDUALE FUNC-----\n");
// - every time a timer interrupt occurs, your worker thread library
// should be contexted switched from a thread context to this
// schedule() function

// - invoke scheduling algorithms according to the policy (RR or MLFQ)

// - schedule policy
#ifndef MLFQ
    // Choose RR
    sched_rr();
#else
    // Choose MLFQ
    
#endif
}

static void sched_rr()
{
    
    int status = running->thread_status;
    if (status == READY){
        tcb* old_running = running;
        if((running = dequeue(ready_q)) == NULL){
            error("can not dequeue");
        }
        enqueue(ready_q, old_running);
    }
    else if (status == BLOCKED){
        if((running = dequeue(ready_q) )== NULL){
            error("can not dequeue");
        }
    }
    else if (status == DONE) {
        tcb* thread;
        while((thread = dequeue(running->tcbs_waiting)) != NULL){
            thread->thread_status = READY;
            enqueue(ready_q, thread);
        }
        if((running = dequeue(ready_q)) == NULL){
            error("can not dequeue");
        }

    }
    running->thread_status = RUNNING;
    //makecontext(&sch_ctx, (void*)&schedule, 0);
    printf("about to switch out of scd_ctx TO id(%d)\n", running->thread_id);
    if(setcontext(&(running->thread_context)) < 0){
        error("switch to running ctx failed");
    }
    printf("1. THIS LINE SHOULD NEVER RUN\n");
}

/* Preemptive MLFQ scheduling algorithm */
static void sched_mlfq()
{
    // - your own implementation of MLFQ
    // (feel free to modify arguments and return types)

}


worker_t get_next_tid(){
    __next_tid++;
    return __next_tid;
}

void make_set_stack(ucontext_t* ctx){
    void *stack=malloc(STACK_SIZE);
	if (stack == NULL){
		error("stack allocation failed");
	}
      
	/* Setup context that we are going to use */
	ctx->uc_link=NULL;
	ctx->uc_stack.ss_sp=stack;
	ctx->uc_stack.ss_size=STACK_SIZE;
	ctx->uc_stack.ss_flags=0;
}

void set_up_timer(){
    
}

void set_up_scheduler_context(){
    if(getcontext(&sch_ctx) < 0){
        error("");
    }
    make_set_stack(&sch_ctx);
    makecontext(&sch_ctx, (void*)&schedule, 0);
}

void set_up_run_main_tcb(){
    running = (tcb *) malloc(sizeof(tcb));
    running->thread_status = RUNNING;
    running->thread_id = get_next_tid();
    running->next = NULL;
    running->tcbs_waiting = init_queue();
    if(getcontext(&(running->thread_context)) < 0){
        error("");
    }
    make_set_stack(&(running->thread_context));
    // if(setcontext(&(running->thread_context)) < 0){
    //     error("");
    // }
    // from this point on, treat main tcb as any other thread
}

/* create a new thread */
int worker_create(worker_t *thread, pthread_attr_t *attr,
                  void *(*function)(void *), void *arg)
{
    printf("Worker Create started\n");
    if(init_scheduler_done == 0){
        //allocate mem for ready_q
        ready_q = init_queue();
        // Create main context
        set_up_run_main_tcb();
        // Create scheduler context and timer
        set_up_scheduler_context();
    }
    //create a tcb here 
    tcb *new_thread = (tcb *)malloc(sizeof(tcb));
    new_thread->thread_status = READY;
    new_thread->thread_id = get_next_tid();
    new_thread->next = NULL;
    new_thread->tcbs_waiting = init_queue();
    if(getcontext(&(new_thread->thread_context)) < 0){
        error("");
    }
    make_set_stack(&(new_thread->thread_context));
    //figure out what type of func to pass
    makecontext(&(new_thread->thread_context), (void*)function, 1, arg); 
    enqueue(ready_q, new_thread);
    *thread = new_thread->thread_id;
    printf("Done Creatig new thread id(%d)\n", new_thread->thread_id);
    if(init_scheduler_done == 0){
        // start timer after the first tcb has been created and pushed
        set_up_timer();
        init_scheduler_done = 1;
    }
    return 0;
}
    

/* give CPU possession to other user-level worker threads voluntarily */
int worker_yield()
{
    // Change worker thread's state from Running to Ready
    running->thread_status = READY;
    // - save context of this thread to its thread control block
    // - switch from thread context to scheduler context
    if(swapcontext(&(running->thread_context), &sch_ctx) < 0) {
        error("swap to sch_ctx failed");
    }
    return 0;
};

/* terminate a thread */
void worker_exit(void *value_ptr)
{
    printf("worker exit starting to exit\n");
    running->thread_status = DONE;
    running->rtr_val = value_ptr;
    printf("worker exit swaping to sch_ctx\n");
    if(swapcontext(&(running->thread_context), &sch_ctx) < 0){
        error("swap to sch_ctx failed");
    }
    printf("2. THIS LINE SHOULD NEVER RUN\n");
}

/* Wait for thread termination */
int worker_join(worker_t thread, void **value_ptr)
{ 
    printf("join started. (%d) is waiting on (%d) to finish\n", running->thread_id,thread);
    running->thread_status = BLOCKED;

    //TODO find the tcb related to the thread_id
    //ISSUE: checks only the READY tcbs, need to check all tcbs
    //so use a HASHMAP. the key of the hasmap would be the thread_id, 
    //Need to sepereate the next ptr from the tcb, so the same tcb can be inserted into multiple queus.
    //so the queue needs to hold struct node{tcb*, next*}; instead of the queue directly holding a tcb.
    // can not just use a tcb struct bc the next ptrs in tcb->next are used by ready_q
    tcb* found = ready_q->head;
    while(found != NULL && found->thread_id != thread){
        found = found->next;
    }
    if(found == NULL){
        error("thread_id not found in worker_join");
    }
    printf("join: tcb found id(%d)\n",found->thread_id);
    
    // - wait for a specific thread to terminate
    enqueue(found->tcbs_waiting, running);
    printf("join: swap context to sch_ctx FROM id(%d)\n", running->thread_id);
    if(swapcontext(&(running->thread_context), &sch_ctx) < 0){
        error("swap to sch_ctx failed");
    }
    printf("join: waiting done\n");
    //if value_ptr is provided, retrieve return value from joining thread
    if(value_ptr != NULL){
        *value_ptr = found->rtr_val;
    }
    //TODO: need to fix
    //if mutiple threads were waiting on 'found' then the first thread to run woudl
    //free found, so the other threads that were waiting would refer to a null object
    //another option is to store the rtr_val somehwere else, and instead free everything in worker_exit
    //de-allocate any dynamic memory created by the joining thread if no one
    free(found->tcbs_waiting);
    free(found->thread_context.uc_stack.ss_sp);
    free(found);
    printf("join: freed waiting done returned to user code\n");
    return 0;
};

/* initialize the mutex lock */
int worker_mutex_init(worker_mutex_t *mutex,
                      const pthread_mutexattr_t *mutexattr)
{
    //- initialize data structures for this mutex
    return 0;

};

/* aquire the mutex lock */
int worker_mutex_lock(worker_mutex_t *mutex)
{

    // - use the built-in test-and-set atomic function to test the mutex
    // - if the mutex is acquired successfully, enter the critical section
    // - if acquiring mutex fails, push current thread into block list and
    // context switch to the scheduler thread
    return 0;

};

/* release the mutex lock */
int worker_mutex_unlock(worker_mutex_t *mutex)
{
    // - release mutex and make it available again.
    // - put one or more threads in block list to run queue
    // so that they could compete for mutex later.

    return 0;
};

/* destroy the mutex */
int worker_mutex_destroy(worker_mutex_t *mutex)
{
    // - make sure mutex is not being used
    // - de-allocate dynamic memory created in worker_mutex_init

    return 0;
};


// Feel free to add any other functions you need.
// You can also create separate files for helper functions, structures, etc.
// But make sure that the Makefile is updated to account for the same.