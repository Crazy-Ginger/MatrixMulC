CC = gcc
.PHONY = clean

all: single thread

single: matrix.c single.c
	${CC} -g -pthread single.c matrix.c -o single

thread: matrix.c thread.c
	${CC} -g -pthread thread.c matrix.c -o thread

clean:
	rm -rfv thread single test
