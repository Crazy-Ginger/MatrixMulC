#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "matrix.h"

void mat_print(struct Matrix *mat)
{
    for (size_t row = 0; row < mat->row; row++)
    {
        for (size_t col= 0; col < mat->col; col++)
        {
            printf("%lld, ", *(mat->ptr + (mat->col * row) + col));
            if (mat->row < 23) 
                printf("\t");
        }
        printf("\n");
    }
}

int mat_read(struct Matrix *mat, char *file_name)
{
    if (mat->ptr != NULL) 
        free(mat->ptr);
    
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
    
    //printf("rows: %zu\tcols: %zu\n", mat->row, mat->col);
    mat->ptr = (long long *) malloc(sizeof(long long) * mat->row * mat->col);
    
    // resets file pointer to top
    rewind(fp);

    size_t index = 0;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    

    while((read = getline(&line, &len, fp)) != -1)
    {
        //printf("line_len: %zu\tline: %s\n", read, line);

        char *numb = strtok(line, ",");
        while (numb != NULL)
        {
            sscanf(numb, "%lld", (mat->ptr +index));
            index++;
            numb = strtok(NULL, ",");
        }
    }
    fclose(fp);
    return 0;
}

void mat_write(struct Matrix *mat, char *file_name)
{
    FILE *fp;
    fp = fopen(file_name, "w");
    if (fp == NULL) 
        exit(EXIT_FAILURE);
    int line_counter = 1;
    for (size_t i = 0; i < mat->col*mat->row; i++)
    {
        fprintf(fp, "%lli", *(mat->ptr + i));
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

void mat_create(struct Matrix *mat)
{
    // can't trust pointers these days, better to do it yourself
    if (mat->ptr != NULL) 
        free(mat->ptr);

    mat->ptr = (long long *) malloc(sizeof(long long) * mat->col * mat->row);
    for (size_t c = 0; c < mat->col; c++)
    {
        for (size_t d = 0; d < mat->row; d++)
        {
            *(mat->ptr +mat->row * c + d) = rand() % 100 + 1;
        }
    }
}

void mat_mul(struct Matrix *mat1, struct Matrix *mat2, struct Matrix *result)
{
    //checks validity of multiplication
    if (mat1->col != mat2->row)
    {
        result->col = result->row = -1;
        return;
    }

    result->col = mat2->col;
    result->row = mat1->row;

    // if result->ptr not initilised allocate memory
    if (result->ptr != NULL)
        free(result->ptr);
    result->ptr = (long long *) malloc(sizeof(long long) * result->col * result->row);
    
    long long tot = 0;
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

void *mul_threaded(void *passed)
{
    struct Passer *pass = (struct Passer *) passed;
    // printf("Thread called with id:%zu  on r:%zu  c:%zu\n", pass->id, pass->row, pass->col);

    long long tot = 0;
    for (size_t k = 0; k < pass->mat1->col; k++)
    {
        size_t m1_ind = (pass->row * pass->mat1->col) + k;
        size_t m2_ind = (pass->mat2->col * k) + pass->col;

        long long tmp = *(pass->mat1->ptr + m1_ind) * *(pass->mat2->ptr + m2_ind);

        printf("id: %zu :~\tk: %zu\t r_ind: %zu = %lli\t c_ind: %zu = %lli\t =tmp: %lli\n", pass->id, k, m1_ind, *(pass->mat1->ptr + m1_ind),  m2_ind, *(pass->mat2->ptr + m2_ind), tmp);
        tot += tmp;
    }
    printf("id: %zu\ttot: %lli\n", pass->id, tot);
    *(pass->result->ptr + (pass->row * pass->result->row) + pass->col) = tot;
    pthread_exit(0);
}

void mat_mul_threaded(struct Matrix *mat1, struct Matrix *mat2, struct Matrix *result)
{
    //checks validity of multiplication
    if (mat1->col != mat2->row)
    {
        result->col = result->row = -1;
        return;
    }
    // if result ptr not initilised allocate memory
    if (result->ptr == NULL)
    {
        result->ptr = (long long *) malloc(sizeof(long long) * result->col * result->row);
    }

    result->col = mat2->col;
    result->row = mat1->row;

    //gets number of threads (leaves one for other operations)
    int status;
    size_t running_threads = 0;
    size_t max_threads = sysconf(_SC_NPROCESSORS_ONLN) - 1;
    pthread_t threads[max_threads];
    
    for (size_t row = 0; row < mat1->row; row++)
    {
        for (size_t col = 0; col < mat2->col; col++)
        {
            struct Passer pass;
            pass.col = col;
            pass.row = row;
            pass.mat1 = mat1;
            pass.mat2 = mat2;
            pass.result = result;
            printf("Opening thread id: %li upon: %zu %zu\n", running_threads, row, col);
            if (running_threads < max_threads)
            {
                // issues passing the same block of memery to be editted?
                
                pass.id = running_threads;
                status = pthread_create(&threads[running_threads], NULL, mul_threaded, &pass);
                if (status != 0)
                {
                    printf("Thread %zu failed to initialize correctly\n", running_threads);
                    exit(EXIT_FAILURE);
                }
                ++running_threads;
            }
            else
            {
                printf("\t Running else\n");
                // join all threads
                for (size_t i = 0; i < max_threads; i++)
                {
                    status = pthread_join(threads[i], NULL);
                    if (status != 0)
                    {
                        printf("Thread %zu failed to join correctly\n", i);
                        exit(EXIT_FAILURE);
                    }
                    printf("Joined thread %zu", i);
                    --running_threads;
                }

                if (running_threads != 0)
                {
                    printf("Not all threads closed\n");
                    exit(EXIT_FAILURE);
                }
                // then run for current selection
                pass.id = running_threads;
                pthread_create(&threads[running_threads], NULL, mul_threaded, (void *) &pass);
                ++running_threads;
            }
        }
    }
    // end all existing threads
    for (size_t i = 0; i < running_threads; i++)
    {
        status = pthread_join(threads[i], NULL);
        if (status != 0)
        {
            printf("Thread %zu failed to join correctly\n", i);
            exit(EXIT_FAILURE);
        }
    }
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
