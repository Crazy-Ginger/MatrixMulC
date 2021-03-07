#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "matrix.h"

void mat_print(struct Matrix *mat)
{
    /**
     *  Prints matrix to commandline, formatted 
     *
     *  @param Matrix* matrix to be displayed
    **/
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
    /**
     *  Reads in matrix from file in the format:
     *  1, 2, 3
     *  3, 4, 5
     *  6, 7, 8
     *
     *  @param Matrix* mat matrix to have values assigned
     *  @param char* file_name name of file to be read
    **/
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
    /**
     * Creates a random matrix of values n, where 0 < n < 100
     *
     * @param Matrix* mat matrix to have values written to
    **/
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
    /**
     * Multiplies mat1 and mat2 together and stores the result into result
     *
     * @param Matrix* mat1 first matrix to multiply
     * @param Matrix* mat2 second matrix to multiply
     * @param Matrix* result matrix of result
    **/
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
    /**
     * Finds the value of a single element in the result matrix
     *
     * @param void* passed struct containing details for multiplication
    **/
    struct Passer *pass = (struct Passer *) passed;
    // printf("Thread called with id: %zi\n", pass->id);
    // printf("Thread called with id:%zu  on r:%zu  c:%zu\n", pass->id, pass->row, pass->col);

    long long tot = 0;
    for (size_t k = 0; k < pass->mat1->col; k++)
    {
        size_t m1_ind = (pass->row * pass->mat1->col) + k;
        size_t m2_ind = (pass->mat2->col * k) + pass->col;
        // printf("id: %zu mult: %zu x %zu\t", pass->id, *(pass->mat1->ptr + m1_ind), *(pass->mat2->ptr + m2_ind));

        long long tmp = *(pass->mat1->ptr + m1_ind) * *(pass->mat2->ptr + m2_ind);

        // printf("id: %zu :~\tk: %zu\t r_ind: %zu = %lli\t c_ind: %zu = %lli\t =tmp: %lli\n", pass->id, k, m1_ind, *(pass->mat1->ptr + m1_ind),  m2_ind, *(pass->mat2->ptr + m2_ind), tmp);
        tot += tmp;
    }
    // printf("id: %zi\ttot: %lli\n", pass->id, tot);
    // pthread_mutex_lock(&locker);

    // *(pass->result->ptr + (pass->row * pass->result->row) + pass->col) = tot;
    *(pass->result) = tot;
    // pthread_mutex_unlock(&locker);
    
    printf("id: %zi closed\n", pass->id);
    pthread_exit(0);
}

void mat_mul_threaded(struct Matrix *mat1, struct Matrix *mat2, struct Matrix *result)
{
    /**
     * Multiplies mat1 and mat2 together within threads and stores the result into result
     * Creates n-1 threads, where n is threads of the CPU
     *
     * @param Matrix* mat1 first matrix to multiply
     * @param Matrix* mat2 second matrix to multiply
     * @param Matrix* result matrix of result
    **/

    //checks validity of multiplication
    if (mat1->col != mat2->row)
    {
        result->col = result->row = -1;
        return;
    }

    result->col = mat2->col;
    result->row = mat1->row;

    // if result ptr initilised reallocate memory
    if (result->ptr != NULL)
        free(result->ptr);
        
    result->ptr = (long long *) malloc(sizeof(long long) * result->col * result->row);


    // gets number of threads (leaves one for other operations)
    int status = -1;
    size_t running_threads = 0;
    const size_t max_threads = sysconf(_SC_NPROCESSORS_ONLN) - 1;
    pthread_t threads[max_threads];
    struct Passer passers[max_threads];

    // initialise mutex and thread attributes
    // pthread_mutex_init(&locker, NULL);
    // pthread_attr_t thread_att;
    // pthread_attr_init(&thread_att);

    for (size_t row = 0; row < mat1->row; row++)
    {
        for (size_t col = 0; col < mat2->col; col++)
        {
            passers[running_threads].col = col;
            passers[running_threads].row = row;
            passers[running_threads].mat1 = mat1;
            passers[running_threads].mat2 = mat2;
            passers[running_threads].result = (result->ptr + (row * result->row) + col);
            
            if (running_threads < max_threads)
            {
                // issues passing the same block of memery to be editted?
                
                passers[running_threads].id = running_threads;
                printf("Opening thread id: %li upon: %zi %zu\n", running_threads, row, col);
                status = pthread_create(&threads[running_threads], NULL, mul_threaded, &passers[running_threads]);
                if (status != 0)
                {
                    fprintf(stderr, "Thread %zu failed to initialize correctly\n", running_threads);
                    perror("");
                    exit(EXIT_FAILURE);
                }
                ++running_threads;
            }
            else
            {
                // join all threads
                for (size_t i = 0; i < running_threads; i++)
                {
                    fprintf(stderr, "Rerunners: about to join %zu\n", i);
                    status = pthread_join(threads[i], NULL);
                    // fprintf(stderr, "'Joined' %zu\n", i);
                    if (status != 0)
                    {
                        fprintf(stderr, "Thread %zu failed to join correctly\n", i);
                        exit(EXIT_FAILURE);
                    }
                    else
                        printf("Thread %zu joined , can be rerun\n", i);
                    passers[i].id = -1;
                    --running_threads;
                    
                }
                if (running_threads != 0)
                {
                    fprintf(stderr, "Not all threads closed\n");
                    exit(EXIT_FAILURE);
                }
                // then run for current selection
                passers[running_threads].id = running_threads;
                printf("Opening thread id: %zu upon: %zi %zu\n", running_threads, row, col);
                pthread_create(&threads[running_threads], NULL, mul_threaded, &passers[running_threads]);
                ++running_threads;
            }
        }
    }
    // end all existing threads
    for (size_t i = 0; i < running_threads; i++)
    {
        // fprintf(stderr, "Ending: Attempting to join %zu\n", i);
        status = pthread_join(threads[i], NULL);
        // fprintf(stderr, "'Joined': %zu\n", i);
        if (status != 0)
        {
            fprintf(stderr,"Thread %zu failed to join correctly\n", i);
            exit(EXIT_FAILURE);
        }
        else
            printf("Thread %zu joined at last\n", i);
    }
    // pthread_mutex_destroy(&locker);
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
