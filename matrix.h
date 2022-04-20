#include <stdlib.h>
#include <pthread.h>

#ifndef MAT_HEADER
#define MAT_HEADER

#define START_CLOCK(X) clock_t X = clock()
#define STOP_CLOCK(X) printf("%s\t%fs\t %li ticks\n", (#X), (double) (clock() - X) / CLOCKS_PER_SEC, clock() - X)


struct Matrix
{
    double *ptr;
    ssize_t col;
    ssize_t row;
};

struct Passer
{
    ssize_t id;
    struct Matrix *mat1;
    struct Matrix *mat2;
    double *result;
    size_t row;
    size_t col;
};

// Input
void mat_read(struct Matrix *mat, char *file_name);
void mat_create(struct Matrix *mat, int n);

// Output
void mat_print(struct Matrix *mat);
void mat_write(struct  Matrix *mat, char *file_name);

// Add/Sub
void mat_add(struct Matrix *mat1, struct Matrix *mat2, struct Matrix *result);
void mat_sub(struct Matrix *mat1, struct Matrix *mat2, struct Matrix *result);

// Scalar Mul/Div
void mat_sca(struct Matrix *mat, double n, struct Matrix *result);
void mat_div(struct Matrix *mat, double n, struct Matrix *result);

void mat_trans(struct Matrix *mat, struct Matrix *trans);

// Matrix Mul
void mat_mul(struct Matrix *mat1, struct Matrix *mat2, struct Matrix *result);
void mat_mul_threaded(struct Matrix *mat1, struct Matrix *mat2, struct Matrix *result);
void *mul_threaded(void *pass);


// old useless functions
// void *mat_create_thread(void *);

#endif
