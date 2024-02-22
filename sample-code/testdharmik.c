
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <ucontext.h>
#define STACK_SIZE SIGSTKSZ
#define TRUE 1
#define FALSE 0

ucontext_t foo_ctx, bar_ctx, main_ctx;
ucontext_t running_ctx, next_ctx; //running is whats running rn. and next is head of ready queue
int flag = TRUE;
void timer_handler(int signum){
    printf("1 sec\n");
    ucontext_t temp = running_ctx;
    running_ctx = next_ctx;
    next_ctx = temp;
    swapcontext(&next_ctx, &running_ctx);

}

void foo(){
    printf("foo start\n");
    int i = 0;
    while(1){
        i++;
        printf("foo %d\n", i);
    }
}
void bar(){
    printf("bar start\n");
    int i = 0;
    while(1){
        i++;
        printf("bar %d\n", i);
    }
}
void make_set_stack(ucontext_t* ctx){
    void *stack=malloc(STACK_SIZE);
	if (stack == NULL){
		perror("Failed to allocate stack");
		exit(1);
	}
      
	/* Setup context that we are going to use */
	ctx->uc_link=NULL;
	ctx->uc_stack.ss_sp=stack;
	ctx->uc_stack.ss_size=STACK_SIZE;
	ctx->uc_stack.ss_flags=0;
}
int main(){
    struct sigaction sa;
	memset (&sa, 0, sizeof (sa));
	sa.sa_handler = &timer_handler;
	sigaction (SIGPROF, &sa, NULL);
    struct itimerval timer;
	timer.it_interval.tv_usec = 0; 
	timer.it_interval.tv_sec = 1;
	timer.it_value.tv_usec = 0;
	timer.it_value.tv_sec = 1;

    
    if(getcontext(&foo_ctx) < 0){
        perror("getcontext"); 
        exit(1); 
    }
    if(getcontext(&bar_ctx) < 0){
        perror("getcontext"); 
        exit(1);  
    }
	make_set_stack(&foo_ctx);
    make_set_stack(&bar_ctx);
    makecontext(&foo_ctx, &foo, 0);
    makecontext(&bar_ctx, &bar, 0);
    printf("done setting up context. starting timer now\n");
    setitimer(ITIMER_PROF, &timer, NULL);
    
    running_ctx = foo_ctx;
    next_ctx = bar_ctx;

    swapcontext(&main_ctx, &running_ctx);
	while(1);
    return 0;
}
