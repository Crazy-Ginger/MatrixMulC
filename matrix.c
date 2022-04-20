#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "matrix.h"

void mat_read(struct Matrix *mat, char *file_name)
{
    /**
     *  Reads in matrix from file in the format:
     *  1, 2, 3
     *  3, 4, 5
     *  6, 7, 8
     *
     *  @param Matrix* mat: matrix to have values assigned
     *  @param char* file_name: name of file to be read
    **/
    
    FILE *fp;
    fp = fopen(file_name, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    // gets size of matrix to allocate memory
    unsigned char checker;
    size_t rows = 0, commas = 0;
    while(!feof(fp))
    {
        checker = fgetc(fp);
        if (checker == '\n') 
            rows++;
        else if (checker ==',') 
            commas++;
    }
    mat->row = ++rows;
    mat->col = (commas/rows)+1;
    
    // allocate memory to matrix
    free(mat->ptr);
    mat->ptr = (double *) malloc(sizeof(double) * mat->row * mat->col);
    
    // resets file pointer to top
    rewind(fp);

    // read file and write into matrix
    size_t index = 0;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    

    while((read = getline(&line, &len, fp)) != -1)
    {
        char *numb = strtok(line, ",");
        while (numb != NULL)
        {
            sscanf(numb, "%lf", (mat->ptr +index));
            index++;
            numb = strtok(NULL, ",");
        }
    }
    fclose(fp);
}

void mat_create(struct Matrix *mat, int n)
{
    /**
     * Creates a random matrix of values n, where 0 < n < 100
     *
     *  @param Matrix* mat: matrix to have values written to
     *  @param int n: integer setting max value of matrix elements
    **/
    // can't trust pointers these days, better to do it yourself
    if (mat->ptr != NULL) 
        free(mat->ptr);

    mat->ptr = (double *) malloc(sizeof(double) * mat->col * mat->row);
    #pragma omp parallel for
    for (size_t i = 0; i < mat->col*mat->row; i++)
    {
        *(mat->ptr + i) = rand() % n + 1;
    }
}


void mat_print(struct Matrix *mat)
{
    /**
     *  Prints matrix to commandline, formatted 
     *
     *  @param Matrix* mat: matrix to be displayed
    **/
    for (size_t row = 0; row < mat->row; row++)
    {
        for (size_t col= 0; col < mat->col; col++)
        {
            printf("%lf, ", *(mat->ptr + (mat->col * row) + col));
            if (mat->row < 23) 
                printf("\t");
        }
        printf("\n");
    }
}

void mat_write(struct Matrix *mat, char *file_name)
{
    /**
     * Write the passed matrix to a file, in the format:
     * 1, 2, 3
     * 3, 4, 5
     * 5, 6, 7
     *
     * @param Matrix* mat matrix to be written
     * @param char* file_name name of file to be created and written to
    **/
    FILE *fp;
    fp = fopen(file_name, "w");
    if (fp == NULL) 
        exit(EXIT_FAILURE);
    int line_counter = 1;
    for (size_t i = 0; i < mat->col*mat->row; i++)
    {
        fprintf(fp, "%lf", *(mat->ptr + i));
        if (line_counter == mat->col)
        {
            fprintf(fp, "\n");
            line_counter = 1;
        }
        else
        {
            fprintf(fp, ", ");
            line_counter++;
        }
    }
    fclose(fp);
}


void mat_add(struct Matrix *mat1, struct Matrix *mat2, struct Matrix *result)
{
    /**
     *   Adds two matricies together
     * 
     *  @param Matrix* mat1: first matrix to be added
     *  @param Matrix* mat2: second matrix to be added
     *  @param Matrix* result: matrix for addition result to be stored to
    **/

    // check validity
    if (mat1->col != mat2->col && mat1->row != mat2->row)
        exit(EXIT_FAILURE);
    
    // sets dimensions of result and allocates memory
    result->col = mat1->col;
    result->row = mat1->row;
    free(result->ptr);
    result->ptr = (double *) malloc(sizeof(double) * result->col * result->row);
    
    // loops over each element storing addition
    #pragma omp parallel for
    for (size_t i = 0; i < mat1->col * mat1->row; i++)
    {
        *(result->ptr + i) = *(mat1->ptr + i) + *(mat2->ptr + i);
    }
}

void mat_sub(struct Matrix *mat1, struct Matrix *mat2, struct Matrix *result)
{
    /**
     *   Subtracts two matricies each other
     * 
     *  @param Matrix* mat1: matrix to be subtracted from
     *  @param Matrix* mat2: matrix to be subtracted
     *  @param Matrix* result: matrix for subtraction result to be stored to
    **/

    // check validity
    if (mat1->col != mat2->col && mat1->row != mat2->row)
        exit(EXIT_FAILURE);
    
    // sets dimensions of result and allocates memory
    result->col = mat1->col;
    result->row = mat1->row;
    free(result->ptr);
    result->ptr = (double *) malloc(sizeof(double) * mat1->col * result->row);
    
    // loops over each element storing addition
    #pragma omp parallel for
    for (size_t i = 0; i < mat1->col * mat1->row; i++)
    {
        *(result->ptr + i) = *(mat1->ptr + i) - *(mat2->ptr + i);
    }
}


void mat_sca(struct Matrix *mat, double n, struct Matrix *result)
{
    /**
     *   Subtracts two matricies each other
     * 
     *  @param Matrix* mat1: matrix to be multiplied
     *  @param double n: rational number to be multiplied by
     *  @param Matrix* result: matrix for scalar product to be stored to
    **/

    // sets dimensions of result and allocates memory
    result->col = mat->col;
    result->row = mat->row;
    free(result->ptr);
    result->ptr = (double *) malloc(sizeof(double) * result->col * result->row);
    
    // loops over each element storing addition
    #pragma omp parallel for
    for (size_t i = 0; i < mat->col * mat->row; i++)
    {
        *(result->ptr + i) = *(mat->ptr + i) *n;
    }
}

void mat_div(struct Matrix *mat, double n, struct Matrix *result)
{
    /**
     *   Subtracts two matricies each other
     * 
     *  @param Matrix* mat1: matrix to be multiplied
     *  @param double n: rational number to be multiplied by
     *  @param Matrix* result: matrix for scalar product to be stored to
    **/

    // sets dimensions of result and allocates memory
    result->col = mat->col;
    result->row = mat->row;
    free(result->ptr);
    result->ptr = (double *) malloc(sizeof(double) * result->col * result->row);
    
    // loops over each element storing addition
    #pragma omp parallel for
    for (size_t i = 0; i < mat->col * mat->row; i++)
    {
        *(result->ptr + i) = *(mat->ptr + i) / n;
    }
}

void mat_trans(struct Matrix *mat, struct Matrix *trans)
{
   /**
     *  Takes one matrix and writes it's transpose to the 2nd
     *
     *  @param Matrix* mat: matrix to take transpose of
     *  @param Matrix* mat2: matrix to store transpose
    **/ 
   
   // set size of result and allocate memory
   trans->col = mat->row;
   trans->row = mat->col;
   free(trans->ptr);
   trans->ptr = (double *) malloc(sizeof(double) * trans->col * trans->row);
    
    // #pragma omp parallel for
    for (size_t i = 0; i < trans->col * trans->row; i++)
    {
        *(trans->ptr + i) = *(mat->ptr + mat->col* (i%mat->row) + (i/mat->row));
    }
}

void mat_mul(struct Matrix *mat1, struct Matrix *mat2, struct Matrix *result)
{
    /**
     *  Multiplies mat1 and mat2 together and stores the product to result
     *
     *  @param Matrix* mat1: first matrix used in multiplication
     *  @param Matrix* mat2: second matrix used in multiplication
     *  @param Matrix* result: product of multiplcation
    **/

    //checks validity of multiplication
    if (mat1->col != mat2->row)
    {
        result->col = result->row = -1;
        exit(EXIT_FAILURE);
    }

    // sets dimensions of result and allocates memory
    result->col = mat2->col;
    result->row = mat1->row;
    free(result->ptr);
    result->ptr = (double *) malloc(sizeof(double) * result->col * result->row);
    
    double tot = 0;
    for (size_t row = 0; row < mat1->row; row++)
    {
        for (size_t col = 0; col < mat2->col; col++)
        {
            for (size_t k = 0; k < mat1->col; k++)
            {
                tot += *(mat1->ptr + (row * mat1->col) + k) * *(mat2->ptr + (mat2->col * k) + col);
            }
            *(result->ptr + (row * result->row) + col) = tot;
            tot = 0;
        }
    }
}

void mat_mul_threaded(struct Matrix *mat1, struct Matrix *mat2, struct Matrix *result)
{
    /**
     *  Multiplies mat1 and mat2 together within threads and stores the result into result
     *  Creates n-1 threads, where n is threads of the CPU
     *
     *  @param Matrix* mat1: first matrix used in multiplication
     *  @param Matrix* mat2: second matrix used in multiplication
     *  @param Matrix* result: product of multiplcation
    **/

    //checks validity of multiplication
    if (mat1->col != mat2->row)
    {
        result->col = result->row = -1;
        return;
    }

    // sets dimensions of result and allocates memory
    result->col = mat2->col;
    result->row = mat1->row;
    free(result->ptr);
    result->ptr = (double *) malloc(sizeof(double) * result->col * result->row);


    // get number of threads, create array for passing data to threads
    int status = -1;
    size_t running_threads = 0;
    const size_t max_threads = sysconf(_SC_NPROCESSORS_ONLN);
    pthread_t threads[max_threads];
    struct Passer passers[max_threads];
    // #pragma omp parallel for
    for (size_t row = 0; row < mat1->row; row++)
    {
        
        for (size_t col = 0; col < mat2->col; col++)
        {
            
            if (running_threads < max_threads)
            {
                // set values to be passed
                passers[running_threads].col = col;
                passers[running_threads].row = row;
                passers[running_threads].mat1 = mat1;
                passers[running_threads].mat2 = mat2;
                passers[running_threads].result = (result->ptr + (row * result->row) + col);
                passers[running_threads].id = running_threads;
                
                status = pthread_create(&threads[running_threads], NULL, mul_threaded, &passers[running_threads]);
                if (status != 0)
                {
                    fprintf(stderr, "Thread %zu failed to initialize correctly\n", running_threads);
                    exit(EXIT_FAILURE);
                }
                ++running_threads;
            }
            else
            {
                // join all running threads
                for (size_t i = 0; i < running_threads; i++)
                {
                    status = pthread_join(threads[i], NULL);
                    if (status != 0)
                    {
                        fprintf(stderr, "Thread %zu failed to join correctly on %zu x %zu\n", i, row, col);
                        exit(EXIT_FAILURE);
                    }
                    passers[i].id = -1;
                    
                    
                }
                running_threads = 0;
                
                // set values to be passed
                passers[running_threads].col = col;
                passers[running_threads].row = row;
                passers[running_threads].mat1 = mat1;
                passers[running_threads].mat2 = mat2;
                passers[running_threads].result = (result->ptr + (row * result->row) + col);
                passers[running_threads].id = running_threads;

                pthread_create(&threads[running_threads], NULL, mul_threaded, &passers[running_threads]);
                ++running_threads;
            }
        }
    }
    // join all left over threads
    for (size_t i = 0; i < running_threads; i++)
    {
        status = pthread_join(threads[i], NULL);
        if (status != 0)
        {
            fprintf(stderr,"Thread %zu finaly failed to join correctly\n", i);
            exit(EXIT_FAILURE);
        }
    }
}

void *mul_threaded(void *passed)
{
    /**
     *  Finds the value of a single element in the result matrix
     *
     *  @param void* passed: struct containing details for multiplication
    **/

    struct Passer *pass = (struct Passer *) passed;

    double tot = 0;
    for (size_t k = 0; k < pass->mat1->col; k++)
    {
        double tmp = *(pass->mat1->ptr + (pass->row * pass->mat1->col) + k) * *(pass->mat2->ptr + (pass->mat2->col * k) + pass->col);
        tot += tmp;
    }
    *(pass->result) = tot;
    pthread_exit(0);
}

// old functions
void *mat_create_thread(void *Matptr)
{
    // do not use, much slower than serial
    struct Matrix *mat = (struct Matrix *) Matptr;
    for (size_t c = 0; c < mat->col; c++)
    {
        for (size_t d = 0; d < mat->row; d++)
        {
            (mat->ptr)[mat->row * c + d] = rand() % 100 +1;
        }
    }
    return NULL;
}
