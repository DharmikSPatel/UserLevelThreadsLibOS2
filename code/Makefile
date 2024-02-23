CC = gcc
#CHANGE: remove all flags except -g -c
CFLAGS = -g -c -std=c99 -Wvla -Wall -fsanitize=address,undefined
AR = ar -rc
RANLIB = ranlib

all: clean thread-worker.a

thread-worker.a: thread-worker.o queue.o
	$(AR) libthread-worker.a thread-worker.o queue.o 
	$(RANLIB) libthread-worker.a

thread-worker.o: thread-worker.h thread_worker_types.h mutex_types.h
ifeq ($(SCHED), RR)
	$(CC) $(CFLAGS) -DRR thread-worker.c
else ifeq ($(SCHED), MLFQ)
	$(CC) $(CFLAGS) -DMLFQ thread-worker.c
else
	echo "no such scheduling algorithm"
endif

clean:
	rm -rf testfile *.o *.a
