#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "mpi.h"


//      ./task2 M N
// argv     0   1 2
void matrix_sum(double *A, double *B, double *C, int m, int n)
{
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < n; ++j)
        {   
            *(C + i * n + j) = *(A + i * n + j) + *(B + i * n + j);
        }
    }
}

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Request request[2];
    
    int matrix_size[2] = {atoi(argv[1]), atoi(argv[2])};
    double *A = (double *) malloc(matrix_size[0] * matrix_size[1] * sizeof(double));
    double *sum = (double *) calloc(matrix_size[0] * matrix_size[1], sizeof(double));
    for (int i = 0; i < matrix_size[0]; ++i)
    {
        for (int j = 0; j < matrix_size[1]; ++j)
        {   
            *(A + i * matrix_size[1] + j) = rank + (i % 5) * (j % 8) * rand();
            *(sum + i * matrix_size[1] + j) = *(A + i * matrix_size[1] + j);
        }
    }
    
    double *buf = (double *) malloc(matrix_size[0] * matrix_size[1] * sizeof(double));
    
    double start_time = MPI_Wtime();
    if (rank == 0)
    {
        MPI_Recv_init(buf, matrix_size[0] * matrix_size[1], MPI_DOUBLE, size - 1, 0, MPI_COMM_WORLD, &request[1]);
    }
    else
    {
        MPI_Recv_init(buf, matrix_size[0] * matrix_size[1], MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &request[1]);
    }
    MPI_Send_init(sum, matrix_size[0] * matrix_size[1], MPI_DOUBLE, (rank + 1) % size, 0, MPI_COMM_WORLD, &request[0]);
    for (int i = 0; i < size; ++i)
    {
        if (rank % 2 == 0)
        {
            MPI_Start(&request[0]);   
            MPI_Start(&request[1]); 
        }
        else
        {
            MPI_Start(&request[1]); 
            MPI_Start(&request[0]); 
        }
        MPI_Waitall(2, request, MPI_STATUS_IGNORE); 
        matrix_sum(buf, A, sum, matrix_size[0], matrix_size[1]);
    }
    double end_time = MPI_Wtime();
    end_time -= start_time;
    if (rank == 0)
    {
        
        printf("TIME = %f, ", end_time);
    }

    MPI_Finalize();
 
    return 0;
}
