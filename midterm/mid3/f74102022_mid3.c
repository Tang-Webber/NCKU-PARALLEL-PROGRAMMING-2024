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
    int passs[100000] = {0};   
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
            passs[i] = pass[i];
        }
        fclose(input_file);
        //qsort(pass, n, sizeof(int), compare);
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    //MPI_Bcast(pass, n, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(passs, n, MPI_INT, 0, MPI_COMM_WORLD);
    int local_count = n / numprocs;
    //MPI_Scatter(pass, local_count, MPI_INT, local, local_count,MPI_INT, 0, MPI_COMM_WORLD);
    //qsort(local, n, sizeof(int), compare);         //sort
    qsort(&passs[myid * local_count], local_count, sizeof(int), compare);
    if(myid % 2 != 0){
        MPI_Send(&passs[myid * local_count], local_count, MPI_INT, myid - 1, 0, MPI_COMM_WORLD);
    }
    else{
        MPI_Recv(&passs[(myid + 1) * local_count], local_count, MPI_INT, myid + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
printf("receive from %d!! myid = %d || size = %d \n", myid + 1, myid, local_count);
    }   

    //MPI_Allgather(local, local_count, MPI_INT, passs, local_count, MPI_INT, MPI_COMM_WORLD);
    
    //Merge sort
    int temp;    
    for(int x = 1, y = 2; y <= numprocs; x *= 2, y *= 2){
        if(myid % y == 0){
            int front = myid * local_count;
            int back = front + x * local_count;
            int z = 0;
//printf("test, ID = %d; front = %d; back = %d\n", myid, front, back);
            while(1){
//if(myid == 0) printf("test : z = %d\n", z);
                if(passs[front] <= passs[back]){
                    local[z++] = passs[front++];
                }
                else{
                    local[z++] = passs[back++];
                }
                if(front == (myid + x) * local_count){
                    while(z != local_count * y){
                        local[z++] = passs[back++];
                    }
                    break;
                }
                if(back == (myid + 2 * x) * local_count){
                    while(z != local_count * y){
                        local[z++] = passs[front++];
                    }                  
                    break;
                }             
            }     
            if(y == numprocs)
                break;
            if(myid % (2*y) != 0){
                MPI_Send(local, z, MPI_INT, myid - y, 0, MPI_COMM_WORLD);
            }
            else{
                MPI_Recv(&passs[(myid + y) * x * local_count], z, MPI_INT, myid + y, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
printf("receive from %d!! myid = %d || size = %d \n", myid + y, myid, (myid + y) * x * local_count);
            }    
        }
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