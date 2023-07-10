CC=			gcc
CFLAGS=		-Wall -O3

all:ms-bench1 ms-bench32

ms-bench1:bench.c
		$(CC) $(CFLAGS) -DSTEP0=1 $< -o $@

ms-bench32:bench.c
		$(CC) $(CFLAGS) -DSTEP0=32 $< -o $@

clean:
		rm -fr *.o a.out $(PROG) *~ *.a *.dSYM ms-bench*
