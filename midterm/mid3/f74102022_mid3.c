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
    int passs[70002] = {0};   
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
            passs[i] = pass[i];
        }
        fclose(input_file);
        //qsort(pass, n, sizeof(int), compare);
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(pass, n, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(passs, n, MPI_INT, 0, MPI_COMM_WORLD);
    int local_count = n / numprocs;
    //MPI_Scatter(pass, local_count, MPI_INT, local, local_count,MPI_INT, 0, MPI_COMM_WORLD);
    //qsort(local, n, sizeof(int), compare);         //sort
    qsort(&passs[myid * local_count], local_count, sizeof(int), compare);
/*
if(myid==0){
for(int i=0; i<n; i++){
    printf("%d ", passs[i]);
} 
printf("\n==================================\n");   
}
*/
    //MPI_Allgather(local, local_count, MPI_INT, passs, local_count, MPI_INT, MPI_COMM_WORLD);
    //Merge sort
    int temp;    
    for(int x = 1, y = 2; y <= numprocs; x *= 2, y *= 2){
        if(myid % y == 0){
            int front = myid * x * local_count;
            int back = (myid + 1) * x *local_count;
            int z = 0;
printf("test, ID = %d; front = %d; back = %d\n", myid, front, back);
            while(1){
//if(myid == 0) printf("test : z = %d\n", z);
                if(passs[front] <= passs[back]){
                    local[z++] = passs[front++];
                }
                else{
                    local[z++] = passs[back++];
                }
                if(front == (myid + 1) * x * local_count){
                    while(z != local_count * y){
                        local[z++] = passs[back++];
                    }
                    break;
                }
                if(back == (myid + 2) * x * local_count){
                    while(z != local_count * y){
                        local[z++] = passs[front++];
                    }                    
                    break;
                }             
            }     
printf("stuck!? id = %d\n", myid); 
            if(myid % (2*y) != 0){
                MPI_Send(local, z, MPI_INT, myid - (myid % (2*y)), 0, MPI_COMM_WORLD);
            }
            else{
                MPI_Recv(&passs[(myid + 2 * y) * x * local_count], z, MPI_INT, myid + (myid % (2*y)), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }    
printf("@@@stuck!? id = %d\n", myid);  
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