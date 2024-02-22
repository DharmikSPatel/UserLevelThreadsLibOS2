CC = gcc
CFLAGS = -g
all: getcontext makecontext swapcontext timer yield_swapcontext
getcontext: getcontext.c
	$(CC) $(CFLAGS) $^ -o $@ 
makecontext: makecontext.c
	$(CC) $(CFLAGS) $^ -o $@ 
swapcontext: swapcontext.c
	$(CC) $(CFLAGS) $^ -o $@ 
timer: timer.c
	$(CC) $(CFLAGS) $^ -o $@ 
yield_swapcontext: yield_swapcontext.c
	$(CC) $(CFLAGS) $^ -o $@ 
testdharmik: testdharmik.c
	$(CC) $(CFLAGS) $^ -o $@ 
clean:
	rm -rf getcontext makecontext swapcontext timer yield_swapcontext testdharmik