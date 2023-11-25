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
    int pass[70010] = {0};
    int local[70010] = {0};
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
    int local_count = n / numprocs;
    MPI_Scatter(pass, local_count, MPI_INT, local, local_count,MPI_INT, 0, MPI_COMM_WORLD);
    qsort(local, n, sizeof(int), compare);         //sort
    int passs[70002] = {0};
    int temp;
    MPI_Allgather(local, local_count, MPI_INT, passs, local_count, MPI_INT, 0, MPI_COMM_WORLD);
    for(int x = 1, y = 2; y == numprocs; x *= 2, y *= 2){
        if(myid % y == 0){
            int front = myid * x * local_count;
            int back = (myid + 1) * x *local_count;
            int z = 0;
            while(1){
                if(passs[front] <= passs[back])
                    local[z++] = passs[front++];
                else
                    local[z++] = passs[back++];
                if(front == (myid + 1) * x * local_count){
                    while(back != (myid + 2) * x * local_count){
                        local[z++] = passs[back++];
                    }
                    break;
                }
                if(back == (myid + 2) * x * local_count){
                    while(front != (myid + 1) * x * local_count){
                        local[z++] = passs[front++];
                    }                    
                    break;
                }                
            }      
            if(myid % (2*y) != 0){
                MPI_Send(local, z, MPI_INT, myid - (myid % (2*y)), 0, MPI_COMM_WORLD);
            }
            else{
                MPI_Recv(&passs[(myid + 2) * x * local_count], z, MPI_INT, myid + (myid % (2*y)), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }     
        }
    }
    if(myid == 0){
        for(int i=0; i<n; i++){
            printf("%d ", passs[i]);
        }
printf("\n==================================\n");
qsort(pass, n, sizeof(int), compare);
for(int i=0; i<n; i++){
    printf("%d ", pass[i]);
}
    }

    
    MPI_Finalize();
    return 0;
}