#include "mpi.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

int main( int argc, char *argv[]){
    int t, D, n, m, myid, numprocs;
    int count = 0;
    char input[50];
    int** A;
    int** K;
    int** temp;
    int size;
    int rest = 0;
    int k = 1;
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
        fscanf(input_file, "%d", &t);
        fscanf(input_file, "%d %d", &n, &m);
        size = n / numprocs;

printf("t:%d n:%d m:%d size:%d\n", t, n, m, size);

        A = (int**)malloc((n + 2 * numprocs) * sizeof(int*));
        for (int i = 0; i < n + 2 * numprocs; i++) {
            A[i] = (int*)malloc(m * sizeof(int));
        }
        k = 1;    
        for (int i = 1; i < n ; i++) {
            for (int j = 0; j < m ; j++) {
                fscanf(input_file, "%d", &A[i+k][j]);
printf("%d ", i+k);
            }
            if((i+1) % size == 0 && k < 15)
                k += 2;
        }      
printf("test\n");  
        size += 2;      //actual size = n / numprocs + 2
        for(int i = 1; i < numprocs ; i++){
            for (int j = 0; j < m ; j++) {
                A[i * size - 1][j] = A[i * size + 1][j];
                A[i * size][j] = A[i * size - 2][j];
            }                  
        }   
printf("test\n");
        for (int j = 0; j < m ; j++) {
            A[0][j] = A[n + 2 * numprocs - 2][j];
            A[n + 2 * numprocs - 1][j] = A[1][j];
        }  
printf("get A\n");
for(int i=0 ;i <n + 2 * numprocs;i++){
for(int j=0;j<m;j++){
printf("%d ", A[i][j]);
}
printf("\n");
} 
        fscanf(input_file, "%d", &D);
        K = (int**)malloc(D * sizeof(int*));
        for(int i = 0; i < D; i++) {
            K[i] = (int*)malloc(D * sizeof(int));
        }
        for(int i = 0; i < D; i++) {
            for(int j = 0; j < D; j++) {
                fscanf(input_file, "%d", &K[i][j]);
            }
        }
        fclose(input_file);
    }
    MPI_Bcast(&t, 1, MPI_INT, 0, MPI_COMM_WORLD);
    //MPI_Bcast(&D, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
printf("Bcast done!\n");
    rest = n % numprocs;
    size = n / numprocs + 2;
    int** local_A;
    int** local_B;

    //scatter
    if(rest == 0){
        local_A = (int**)malloc(size * sizeof(int*));
        local_B = (int**)malloc(size * sizeof(int*));
        for(int i = 0; i < size; i++) {
            local_A[i] = (int*)malloc(m * sizeof(int));
            local_B[i] = (int*)malloc(m * sizeof(int));
        }
        MPI_Scatter(A, size * m * sizeof(int), MPI_BYTE, local_A, size * m * sizeof(int), MPI_BYTE, 0, MPI_COMM_WORLD);              
    }
    else{       //rest
        int *recv_counts = (int*)malloc(numprocs * sizeof(int));
        int *displacements = (int*)malloc(numprocs * sizeof(int));      
        for(int i = 0; i < numprocs; i++) {
            recv_counts[i] = size * m;
            if (i == numprocs - 1) {
                recv_counts[i] += rest * m;
            }
            displacements[i] = i * recv_counts[i];
        }  
        size = recv_counts[myid] / m; 

        local_A = (int**)malloc(size * sizeof(int*));
        local_B = (int**)malloc(size * sizeof(int*));
        for(int i = 0; i < size; i++) {
            local_A[i] = (int*)malloc(m * sizeof(int));
            local_B[i] = (int*)malloc(m * sizeof(int));
        }
        MPI_Scatterv(A, recv_counts, displacements, MPI_INT, local_A, recv_counts[myid], MPI_INT, 0, MPI_COMM_WORLD);
    }
printf("scatter done !\n");
    //calculate
    size -= 2;
    for(int x = 0; x < t; x++) {
        if(x % 2 == 0) { //local_A -> local_B
            for(int y = 1; y <= size; y++) {
                local_B[y][0] = local_A[y - 1][m - 1] * K[0][0] + local_A[y - 1][0]     * K[0][1] + local_A[y - 1][1]     * K[0][2] +
                                local_A[y][m - 1]     * K[1][0] + local_A[y][0]         * K[1][1] + local_A[y][1]         * K[1][2] +
                                local_A[y + 1][m - 1] * K[2][0] + local_A[y + 1][0]     * K[2][1] + local_A[y + 1][1]     * K[2][2];
                local_B[y][m - 1] = local_A[y - 1][0] * K[0][0] + local_A[y - 1][m - 1] * K[0][1] + local_A[y - 1][m - 2] * K[0][2] +
                                    local_A[y][0]     * K[1][0] + local_A[y][m - 1]     * K[1][1] + local_A[y][m - 2]     * K[1][2] +
                                    local_A[y + 1][0] * K[2][0] + local_A[y + 1][m - 1] * K[2][1] + local_A[y + 1][m - 2] * K[2][2];
                for(int z = 1; z < m - 1; z++) {
                    local_B[y][z] = local_A[y - 1][z - 1] * K[0][0] + local_A[y - 1][z] * K[0][1] + local_A[y - 1][z + 1] * K[0][2] +
                                    local_A[y][z - 1]     * K[1][0] + local_A[y][z]     * K[1][1] + local_A[y][z + 1]     * K[1][2] +
                                    local_A[y + 1][z - 1] * K[2][0] + local_A[y + 1][z] * K[2][1] + local_A[y + 1][z + 1] * K[2][2];
                }
            }
            //send
            MPI_Send(local_B[1], m, MPI_INT, (myid - 1) % numprocs, 0, MPI_COMM_WORLD);
            MPI_Send(local_B[size], m, MPI_INT, (myid + 1) % numprocs, 0, MPI_COMM_WORLD);
            //receive
            MPI_Recv(local_A[0], m, MPI_INT, (myid - 1) % numprocs, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(local_A[size + 1], m, MPI_INT, (myid + 1) % numprocs, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else { //local_B -> local_A
            for(int y = 1; y <= size; y++) {
                local_A[y][0] = local_B[y - 1][m - 1] * K[0][0] + local_B[y - 1][0]     * K[0][1] + local_B[y - 1][1]     * K[0][2] +
                                local_B[y][m - 1]     * K[1][0] + local_B[y][0]         * K[1][1] + local_B[y][1]         * K[1][2] +
                                local_B[y + 1][m - 1] * K[2][0] + local_B[y + 1][0]     * K[2][1] + local_B[y + 1][1]     * K[2][2];
                local_A[y][m - 1] = local_B[y - 1][0] * K[0][0] + local_B[y - 1][m - 1] * K[0][1] + local_B[y - 1][m - 2] * K[0][2] +
                                    local_B[y][0]     * K[1][0] + local_B[y][m - 1]     * K[1][1] + local_B[y][m - 2]     * K[1][2] +
                                    local_B[y + 1][0] * K[2][0] + local_B[y + 1][m - 1] * K[2][1] + local_B[y + 1][m - 2] * K[2][2];
                for(int z = 1; z < m - 1; z++) {
                    local_A[y][z] = local_B[y - 1][z - 1] * K[0][0] + local_B[y - 1][z] * K[0][1] + local_B[y - 1][z + 1] * K[0][2] +
                                    local_B[y][z - 1]     * K[1][0] + local_B[y][z]     * K[1][1] + local_B[y][z + 1]     * K[1][2] +
                                    local_B[y + 1][z - 1] * K[2][0] + local_B[y + 1][z] * K[2][1] + local_B[y + 1][z + 1] * K[2][2];
                }
            }
            //send
            MPI_Send(local_A[1], m, MPI_INT, (myid - 1) % numprocs, 0, MPI_COMM_WORLD);
            MPI_Send(local_A[size], m, MPI_INT, (myid + 1) % numprocs, 0, MPI_COMM_WORLD);
            //receive
            MPI_Recv(local_B[0], m, MPI_INT, (myid - 1) % numprocs, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(local_B[size + 1], m, MPI_INT, (myid + 1) % numprocs, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    } 
printf("calculate done!\n");
    size += 2;
    if (myid != 0) {
        if(t % 2 == 0){         //A->B->A
            MPI_Send(local_A, size * m, MPI_BYTE, 0, 0, MPI_COMM_WORLD);
        }
        else{
            MPI_Send(local_B, size * m, MPI_BYTE, 0, 0, MPI_COMM_WORLD);
        }
    } 
    else {
        temp = (int**)malloc(n / numprocs + rest * sizeof(int*));
        for(int i = 0; i < size; i++) {
            temp[i] = (int*)malloc(m * sizeof(int));
        }

        if(t % 2 == 0){
            for(int i=1 ;i < size - 2;i++){
                for(int j=0;j<m;j++){
                    printf("%d ", local_A[i][j]);
                }
            }            
        }
        else{
            for(int i=1 ;i < size - 2;i++){
                for(int j=0;j<m;j++){
                    printf("%d ", local_B[i][j]);
                }
            } 
        }

        for (int v = 1; v < numprocs - 1; v++) {
            MPI_Recv(temp, size * m, MPI_INT, v, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for(int i=1 ;i < size - 2;i++){
                for(int j=0;j<m;j++){
                    printf("%d ", temp[i][j]);
                }
            } 
        }
        MPI_Recv(temp, (size + rest)* m, MPI_INT, numprocs - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for(int i=1 ;i < size + rest - 2;i++){
            for(int j=0;j<m;j++){
                printf("%d ", temp[i][j]);
            }
        } 
    }
    MPI_Finalize();
    return 0;
}