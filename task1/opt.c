#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <time.h>

// the format of the command line: ./Task1 n m
// argv                               0    1 2

// (n - number of rows of the matrix W, 
//  m - number of the columns of the matrix A and also the length of the vector b)

double sigmoid(double input)
{
    return (double) 1.0 / (1.0 + exp(-1 * input));
}

int main(int argc, char *argv[])
{
    srand(42);
    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    double *W = (double *) malloc(n * m * sizeof(double));
    double *b = (double *) malloc(m * sizeof(double));
    double *c = (double *) malloc(n * sizeof(double));
    double mean_activation = 0.0;
    double local_result;
    
    if ((W != NULL) && (b != NULL) && (c != NULL))
    {
        
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < m; ++j)
            {
                *(W + i * m + j) = (i % 5) * (j % 8) * rand();
            }
            *(c + i) = 0.0;
        }
        
        for (int i = 0; i < m; ++i)
        {
            *(b + i) = rand();
        }
        
        
        double start, end;
        start = omp_get_wtime();
        #pragma omp parallel private(local_result)
        {
            for (int i = 0; i < n; ++i)
            {
                local_result = 0.0;
                #pragma omp for nowait 
                for (int j = 0; j < m; ++j)
                {
                    local_result += *(W + i * m + j) * *(b + j);
                }
                #pragma omp critical 
                {
                    *(c + i) += local_result;
                }
            }
            
            #pragma omp for
            for (int j = 0; j < n; ++j)
            {
                *(c + j) = sigmoid(*(c + j) + rand());
                #pragma omp critical
                {
                    mean_activation += *(c + j) / n;
                }
            }
        }
        end = omp_get_wtime();
        //printf("Computations took %f seconds", end - start); 
        printf("%f, ", end - start); 
    }
    else
    {
        printf("Memory allocation failed");
    }
    
    if (W != NULL)
    {
        free(W);
    }
    if (b != NULL)
    {
        free(b);
    }
    if (c != NULL)
    {
        free(c);
    }
    
    return 0;
}

