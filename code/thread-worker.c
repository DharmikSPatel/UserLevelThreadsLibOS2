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
#define MY_DEBUG 0
//misc data structs
tcb* running = NULL;
Queue* ready_q = NULL;
Queue* all_nodes_q = NULL;
worker_t __next_tid = 0;
ucontext_t sch_ctx;
int init_scheduler_done = 0;
struct itimerval timer;

void my_error(const char* msg, char* file, int line){
    perror("PERROR");
    fprintf(stderr,"MY LIB ERROR %s %s@%d\n", msg, file, line);
    exit(1);
}
#define error(p) my_error(p, __FILE__, __LINE__)
/* scheduler */
static void pause_timer();
static void start_timer();
static void sched_rr();
static void sched_mlfq();
static void schedule()
{
    if (MY_DEBUG) printf("IN SCEDUALE FUNC-----\n");
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
    pause_timer();
    int status = running->thread_status;
    if (status == READY){
        if (MY_DEBUG) printf("Ready in Secd b4 (%d)\n", running->thread_id);
        // tcb* old_running = running;
        // if((running = dequeue(ready_q)) == NULL){
        //     error("can not dequeue");
        // }
        // enqueue(ready_q, old_running);
        enqueue(ready_q, running);
        if((running = dequeue(ready_q)) == NULL){
            error("can not dequeue");
        }
    }
    else if (status == BLOCKED){
        if (MY_DEBUG) printf("Blocked in Secd b4 (%d)\n", running->thread_id);
        if((running = dequeue(ready_q) )== NULL){
            error("can not dequeue");
        }
    }
    else if (status == DONE) {
        if (MY_DEBUG) printf("Done in Secd b4 (%d)\n", running->thread_id);
        // printQueue(ready_q);
        if(running->tcb_waiting != NULL){
            if (MY_DEBUG) printf("Enqeue waiting thread from ()\n");
            running->tcb_waiting->thread_status = READY;
            enqueue(ready_q, running->tcb_waiting); 
        } else {
            // free
        }


        if((running = dequeue(ready_q)) == NULL){
            error("can not dequeue");
        }

    }
    else {
        if (MY_DEBUG) printf("UNKWNOWN STATUS\n");
    }
    if (MY_DEBUG) printf("Running Next: (%d)\n", running->thread_id);
    running->thread_status = RUNNING;

    if (MY_DEBUG) printf("made it rhouhg\n");
    //makecontext(&sch_ctx, (void*)&schedule, 0);
    if (MY_DEBUG) printf("about to switch out of scd_ctx TO id(%d)\n", running->thread_id);
    start_timer();
    if(setcontext(&(running->thread_context)) < 0){
        error("switch to running ctx failed");
    }
    if (MY_DEBUG) printf("1. THIS LINE SHOULD NEVER RUN\n");
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
void premptThread(int signum){
    if (MY_DEBUG) printf("PREMEPT____(%d)\n", running->thread_id);
    running->thread_status = READY;
    // - save context of this thread to its thread control block
    // - switch from thread context to scheduler context
    if(swapcontext(&(running->thread_context), &sch_ctx) < 0) {
        error("swap to sch_ctx failed");
    }
}
void start_timer(){
    timer.it_value.tv_usec = QUANTUM;
	timer.it_value.tv_sec = 0;    
    setitimer(ITIMER_PROF, &timer, NULL);
}
void pause_timer(){
    timer.it_value.tv_usec = 0;
	timer.it_value.tv_sec = 0;
    setitimer(ITIMER_PROF, &timer, NULL);
}
void set_up_timer(){
    struct sigaction sa;
	memset (&sa, 0, sizeof (sa));
	sa.sa_handler = &premptThread;
	sigaction (SIGPROF, &sa, NULL);
    timer.it_interval.tv_usec = QUANTUM; 
	timer.it_interval.tv_sec = 0;
    start_timer();
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
    if (MY_DEBUG) printf("ACTAULL SIZE of TCB*: %ld\n", sizeof(running));
    if (MY_DEBUG) printf("ACTAULL SIZE of TCB: %ld\n", sizeof(tcb));
    if (MY_DEBUG) printf("ACTAULL SIZE of cont: %ld\n", sizeof(ucontext_t));
    
    running->thread_status = RUNNING;
    running->thread_id = get_next_tid();
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
    if (MY_DEBUG) printf("Worker Create started\n");
    if(init_scheduler_done == 0){
        //allocate mem for ready_q
        ready_q = init_queue();
        all_nodes_q = init_queue();
        // Create main context
        set_up_run_main_tcb();
        // Create scheduler context and timer
        set_up_scheduler_context();
    }
    //create a tcb here 
    tcb *new_thread = (tcb *)malloc(sizeof(tcb));
    new_thread->thread_status = READY;
    new_thread->thread_id = get_next_tid();
    if(getcontext(&(new_thread->thread_context)) < 0){
        error("");
    }
    make_set_stack(&(new_thread->thread_context));
    makecontext(&(new_thread->thread_context), (void*)function, 1, arg); 
    enqueue(ready_q, new_thread);
    enqueue(all_nodes_q, new_thread); //just need to do once
    *thread = new_thread->thread_id;
    if (MY_DEBUG) printf("Done Creatig new thread id(%d)\n", new_thread->thread_id);
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
    if (MY_DEBUG) printf("worker exit starting to exit\n");
    running->thread_status = DONE;
    running->rtr_val = value_ptr;
    if (MY_DEBUG) printf("worker exit set to sch_ctx\n");
    if(setcontext(&sch_ctx) < 0){
        error("swap to sch_ctx failed");
    }
    if (MY_DEBUG) printf("2. THIS LINE SHOULD NEVER RUN\n");
}

/* Wait for thread termination */
int worker_join(worker_t thread, void **value_ptr)
{ 
    if (MY_DEBUG) printf("join started. (%d) is waiting on (%d) to finish\n", running->thread_id,thread);
    running->thread_status = BLOCKED;
    struct node* found = all_nodes_q->head;
    while(found != NULL && found->data->thread_id != thread){
        found = found->next;
    }
    if(found == NULL){
        error("thread_id not found in worker_join");
    }
    if(found->data->thread_status == DONE){
        if (MY_DEBUG) printf("join: tcb already joined\n");
        running->thread_status = RUNNING;
        if(value_ptr != NULL){
            *value_ptr = NULL;
        }
        return 0;
    }
    if (MY_DEBUG) printf("join: tcb found id(%d)\n",found->data->thread_id);
    
    // - wait for a specific thread to terminate
    found->data->tcb_waiting = running;


    if (MY_DEBUG) printf("join: swap context to sch_ctx FROM id(%d)\n", running->thread_id);
    if(swapcontext(&(running->thread_context), &sch_ctx) < 0){
        error("swap to sch_ctx failed");
    }
    if (MY_DEBUG) printf("join: waiting done\n");
    //if value_ptr is provided, retrieve return value from joining thread
    if(value_ptr != NULL){
        *value_ptr = found->data->rtr_val;
        if (MY_DEBUG) printf("VAL: (%d) %p|\n", found->data->thread_id,value_ptr);
        if (MY_DEBUG) printf("VAL: (%d) %p|\n", found->data->thread_id,*value_ptr);
        if (MY_DEBUG) printf("VAL: (%d) %d|\n", found->data->thread_id, *((int*)(*value_ptr)));
    }
    else {
        if (MY_DEBUG) printf("VAL: NUL\n");
        // *value_ptr = NULL;
    }
    tcb* data = removeNode(all_nodes_q, found);
    free(data->thread_context.uc_stack.ss_sp);
    free(data);
    if (MY_DEBUG) printf("join: freed waiting done returned to user code\n");
    return 0;
};

/* initialize the mutex lock */
int worker_mutex_init(worker_mutex_t *mutex,
                      const pthread_mutexattr_t *mutexattr)
{
    if (MY_DEBUG) printf("INIT MUTEX\n");
    //- initialize data structures for this mutex
    if (MY_DEBUG) printf("MUTEX INIT %p\n", mutex);
    // mutex = (worker_mutex_t*) malloc(sizeof(worker_mutex_t));
    atomic_flag_clear(&(mutex->__lock));
    mutex->blocked_threads = init_queue();
    if (MY_DEBUG) printf("MUTEX INIT %p\n", mutex);
    if (MY_DEBUG) printf("QUEUE INIT %p\n", mutex->blocked_threads);
    if (MY_DEBUG) printf("HEAD INIT %p\n", mutex->blocked_threads->head);
    if (MY_DEBUG) printf("INIT MUTEX DONE\n");
    return 0;

};

/* aquire the mutex lock */
int worker_mutex_lock(worker_mutex_t *mutex)
{
    while(atomic_flag_test_and_set(&(mutex->__lock))){
        //do not have lock
        running->thread_status = BLOCKED;
        enqueue(mutex->blocked_threads, running);
        if (MY_DEBUG) printf("m_lock: waiting on lock, swap context to sch_ctx FROM id(%d)\n", running->thread_id);
        if(swapcontext(&(running->thread_context), &sch_ctx) < 0){
            error("swap to sch_ctx failed");
        }
    }
    if (MY_DEBUG) printf("m_lock: waiting done(%d)\n", running->thread_id);
    // - use the built-in test-and-set atomic function to test the mutex
    // - if the mutex is acquired successfully, enter the critical section
    // - if acquiring mutex fails, push current thread into block list and
    // context switch to the scheduler thread
    return 0;

};

/* release the mutex lock */
int worker_mutex_unlock(worker_mutex_t *mutex)
{
    atomic_flag_clear(&(mutex->__lock));
    tcb* next = dequeue(mutex->blocked_threads);
    if(next != NULL){
        if (MY_DEBUG) printf("LOCK REASLED, ADDING new guy\n");
        next->thread_status = READY;
        enqueue(ready_q, next);
    }
    else{
        if (MY_DEBUG) printf("LOCK REASLED, NO ONE else wating\n");
    }

    // - release mutex and make it available again.
    // - put one or more threads in block list to run queue
    // so that they could compete for mutex later.

    return 0;
};

/* destroy the mutex */
int worker_mutex_destroy(worker_mutex_t *mutex)
{
    // TODO: make sure mutex is not being used
    // - de-allocate dynamic memory created in worker_mutex_init

    if(mutex->blocked_threads->head == NULL){
        free(mutex->blocked_threads);
        // free(mutex);
    }

    return 0;
};


// Feel free to add any other functions you need.
// You can also create separate files for helper functions, structures, etc.
// But make sure that the Makefile is updated to account for the same.