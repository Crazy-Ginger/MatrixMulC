#include <stdlib.h>
#include <pthread.h>

#ifndef MAT_HEADER
#define MAT_HEADER

#define START_CLOCK(X) clock_t X = clock()
#define STOP_CLOCK(X) printf("%s\t%fs\t %li ticks\n", (#X), (double) (clock() - X) / CLOCKS_PER_SEC, clock() - X)

struct Matrix
{
    long long *ptr;
    size_t col;
    size_t row;
};

struct Passer
{
    size_t id;
    struct Matrix *mat1;
    struct Matrix *mat2;
    struct Matrix *result;
    size_t row;
    size_t col;
};

void mat_print(struct Matrix *mat);
int mat_read(struct Matrix *mat, char *file_name);
void mat_write(struct  Matrix *mat, char *file_name);
void mat_create(struct Matrix *mat);
void mat_mul(struct Matrix *mat1, struct Matrix *mat2, struct Matrix *result);
void mat_mul_threaded(struct Matrix *mat1, struct Matrix *mat2, struct Matrix *result);
void *mul_threaded(void *pass);

// old useless functions
// void *mat_create_thread(void *);

#endif