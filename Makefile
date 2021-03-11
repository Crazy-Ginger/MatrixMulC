CC = gcc
.PHONY = clean

all: single thread

single: matrix.c single.c
	${CC} -g -pthread -Wall single.c matrix.c -o single

thread: matrix.c thread.c
	${CC} -g -fopenmp -pthread -Wall thread.c matrix.c -o thread

test: matrix.c thread.c
	${CC} -g -fopenmp -pthread -Wall tmp.c matrix.c -o test	

clean:
	rm -rfv thread single test
