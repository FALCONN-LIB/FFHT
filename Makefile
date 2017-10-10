CC = gcc
CFLAGS = -O3 -march=native -std=c99 -pedantic -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes

all: test_float test_double fast_copy.o fht.o

fht.o: fht.c
	$(CC) fht.c -c $(CFLAGS)

fast_copy.o: fast_copy.c
	$(CC) fast_copy.c -c $(CFLAGS)

test_float: test_float.c fast_copy.o fht.o
	$(CC) test_float.c fast_copy.o fht.o -o test_float $(CFLAGS)

test_double: test_double.c fast_copy.o fht.o
	$(CC) test_double.c fast_copy.o fht.o -o test_double $(CFLAGS)

clean:
	rm -f fht.o test_float test_double fast_copy.o
