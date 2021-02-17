#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix.h"

int mult(int dim)
{
    // gets nanoseconds into second and sets srand
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME_COARSE, &tp);
    srand(tp.tv_nsec);

    struct Matrix fst = {0};
    struct Matrix sec = {0};
    struct Matrix mul = {0};
    fst.row = fst.col = sec.row = sec.col = dim;

    // starts program counter clock
    // START_CLOCK(program);
    clock_t program = clock();

    mat_create(&fst);
    mat_create(&sec);

    mat_mul(&fst, &sec, &mul);

    long ticks =  clock() - program;
    double seconds = (double) (clock() - program)/CLOCKS_PER_SEC;
    
    //append data to file
    FILE *data;
    data = fopen("data.txt", "a");

    fprintf(data, "%d, %f, %li\n", dim, seconds, ticks);

    fclose(data);
    free(fst);
    free(sec);
    free(mul);
    return 0;
}

int main()
{
    for (int i = 50; i < 1400; i= i+ 50)
    {
        printf("%d\n", i);
        mult(i);
    }
    return 0;
}
