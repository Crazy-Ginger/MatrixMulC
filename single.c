#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix.h"

int main(int argc, char *argv[])
{
    // gets nanoseconds into second and sets srand
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME_COARSE, &tp);
    srand(tp.tv_nsec);

    int dim = 2;
    //argp_parse(0, argc, argv, 0, 0, 0);

    // if user passes parameter use to set max matrix size
    // can't go over 835?
    if (argc > 1)
    {
        char *endptr;
        /*char *ptr = argv[1];*/
         dim =(unsigned int) strtol(argv[1], &endptr, 10);
    }

    //initilise Matricies with 0 in all fields
    struct Matrix fst = {0};
    struct Matrix sec = {0};
    struct Matrix mul = {0};
    fst.row = fst.col = sec.row = sec.col = dim;

    // starts program counter clock
    

    mat_create(&fst, 10);
    mat_create(&sec, 10);
    
    mat_write(&fst, "data1.txt");
    mat_write(&sec, "data2.txt");
    
    START_CLOCK(program);
    mat_mul(&fst, &sec, &mul);
    STOP_CLOCK(program);
    
    if (dim < 10)
    {
        printf("\nfst:\n");
        mat_print(&fst);
        printf("Sec:\n");
        mat_print(&sec);
        printf("Mul:\n");
        mat_print(&mul);
    }

    mat_write(&mul, "res.txt");

    free(fst.ptr);
    free(sec.ptr);
    free(mul.ptr);
    return 0;
}
