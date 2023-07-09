CC=			gcc
CFLAGS=		-g -Wall -O3

mergesort-bench:bench.o
		$(CC) -o $@ $<

bench.o:bench.c
		$(CC) -c $(CFLAGS) $< -o $@

clean:
		rm -fr *.o a.out $(PROG) *~ *.a *.dSYM mergesort-bench
