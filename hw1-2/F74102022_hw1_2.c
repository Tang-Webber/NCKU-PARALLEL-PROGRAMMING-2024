#include "mpi.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

int compare(const void *a, const void *b) {
    return ((const int(*)[2])a)[0][1] - ((const int(*)[2])b)[0][1];
}

int main( int argc, char *argv[])
{
    int n, myid, numprocs;
    int point[12000][3];
    char input[50];

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);

    if (myid == 0) {
        scanf("%s", input);
        FILE *input_file = fopen(input, "r");
        if(input_file == NULL){
            printf("could not open file %s\n", input);
            fclose(input_file);
            return 1;
        }
        fscanf(input_file, "%d", &n);
        for (int i = 0; i < n; i++) {
            fscanf(input_file, "%d %d", &point[i][1], &point[i][2]);
            point[i][0] = (i + 1);
        }
        fclose(input_file);
        //sort by x
        qsort(point, n, sizeof(point[0]), compare);
    }

    MPI_Datatype int2DType;
    MPI_Type_contiguous(3, MPI_INT, &int2DType);
    MPI_Type_commit(&int2DType);
    MPI_Bcast(point, 12000, int2DType, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD); 
 
    //small convex hull calculate
    
    MPI_Barrier(MPI_COMM_WORLD); 
    //MPI_Reduce(&count, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);      
    
    
    if (myid == 0){
        for(int i=0; i<n;i++){
            printf("id = %d (x, y) = (%d, %d)", point[i][0], point[i][1], point[i][2]);
        }
    } 

    MPI_Type_free(&int2DType);
    MPI_Finalize();
    return 0;
}