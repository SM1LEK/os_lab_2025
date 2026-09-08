CC=gcc
CFLAGS=-I.

all : parallel_min_max process_memory psum

parallel_min_max : parallel_min_max.c utils.o find_min_max.o
	$(CC) $(CFLAGS) parallel_min_max.c utils.o find_min_max.o -o parallel_min_max

process_memory : process_memory.c
	$(CC) process_memory.c -o process_memory

psum : parallel_sum.c libsum.a utils.o
	$(CC) $(CFLAGS) parallel_sum.c utils.o -L. -lsum -lpthread -o psum

libsum.a : sum.o
	ar rcs libsum.a sum.o

sum.o : sum.c sum.h
	$(CC) $(CFLAGS) -c sum.c

utils.o : utils.c utils.h
	$(CC) $(CFLAGS) -c utils.c

find_min_max.o : find_min_max.c find_min_max.h
	$(CC) $(CFLAGS) -c find_min_max.c

clean :
	rm -f utils.o find_min_max.o sum.o libsum.a parallel_min_max process_memory psum
