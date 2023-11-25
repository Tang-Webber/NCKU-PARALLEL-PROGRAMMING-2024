#include "mpi.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

int compare(const void* a, const void* b){
    return *(int*)a - *(int*)b;
}

int main(int argc, char *argv[]){
    int n, myid, numprocs;
    char input[50];
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    int pass[100000] = {0}; 
    int local[100000] = {0};
    if(myid == 0){
        scanf("%s", input);
        FILE *input_file = fopen(input, "r");
        if(input_file == NULL){
            printf("could not open file %s\n", input);
            return 1;
        }
        fscanf(input_file, "%d", &n);
        for(int i=0; i<n; i++){
            fscanf(input_file, "%d", &pass[i]);
        }
        fclose(input_file);
        //qsort(pass, n, sizeof(int), compare);
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(pass, n, MPI_INT, 0, MPI_COMM_WORLD);
    int local_count = n / numprocs;
    //MPI_Scatter(pass, local_count, MPI_INT, local, local_count,MPI_INT, 0, MPI_COMM_WORLD);
    //qsort(local, n, sizeof(int), compare);         //sort
    qsort(pass + myid * local_count, local_count * 2, sizeof(int), compare);
    if(myid % 2 != 0){
        MPI_Send(&pass[myid * local_count], local_count, MPI_INT, myid - 1, 0, MPI_COMM_WORLD);
    }
    else{
        MPI_Recv(&pass[(myid + 1) * local_count], local_count, MPI_INT, myid + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }   

    //MPI_Allgather(local, local_count, MPI_INT, pass, local_count, MPI_INT, MPI_COMM_WORLD);
    //Merge sort
    int temp;    
    for(int x = 1, y = 2; y <= numprocs; x *= 2, y *= 2){
        if(myid % y == 0){
            int front = myid * local_count;
            int back =  (myid + x) * local_count;
            int z = 0;
printf("test, ID = %d; front = %d; back = %d\n", myid, front, back);
            while (front < (myid + x) * local_count && back < (myid + 2 * x) * local_count) {
                if(pass[front] <= pass[back]){
                    local[z++] = pass[front++];
                }
                else{
                    local[z++] = pass[back++];
                }
            }
            while (front < (myid + x) * local_count) {
                local[z++] = pass[front++];
            }
            while (back < (myid + 2 * x) * local_count) {
                local[z++] = pass[back++];
            }
                 
if(myid == 0){
printf("z = %d\n", z);
    for(int i=0;i< z / 2;i++){
        printf("%d ",local[i]);
    }
    printf("\n斷\n");
    for(int i=z / 2;i< z;i++){
        printf("%d ",local[i]);
    }    
    printf("\n============================================================================\n");
}               
            if(myid % (2*y) != 0){
                MPI_Send(local, z, MPI_INT, myid - y, 0, MPI_COMM_WORLD);
            }
            else{
                MPI_Recv(&pass[(myid + y) * local_count], z, MPI_INT, myid + y, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }    
        }

        MPI_Barrier(MPI_COMM_WORLD);
    }
    if(myid == 0){
        for(int i=0; i < n; i++){
            printf("%d ", local[i]);
        }
//printf("\n==================================\n");
//qsort(pass, n, sizeof(int), compare);
//for(int i=0; i<n; i++){
//    printf("%d ", pass[i]);
//}
    }

    
    MPI_Finalize();
    return 0;
}