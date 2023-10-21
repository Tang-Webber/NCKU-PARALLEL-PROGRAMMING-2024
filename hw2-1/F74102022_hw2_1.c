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
    int A[1000][1000];
    int** K;
    int** temp;
    int size;
    int rest = 0;
    int k;
    int D2;
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

        for (int i = 0; i < n ; i++) {
            for (int j = 0; j < m ; j++) {
                fscanf(input_file, "%d", &A[i][j]);
            }
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
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&D, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if(myid != 0){
        K = (int**)malloc(D * sizeof(int*));
        for(int i = 0; i < D; i++) {
            K[i] = (int*)malloc(D * sizeof(int));
        }
    }
    for(int i = 0; i < D;i++){
       MPI_Bcast(K[i], D, MPI_INT, 0, MPI_COMM_WORLD);
    }  
    size = n / numprocs;
    k = D / 2;
    D2 = D * D ;
    for(int i = 0; i < n;i++){
       MPI_Bcast(A[i], m, MPI_INT, 0, MPI_COMM_WORLD); 
    }    
    if(myid == numprocs -1)
        rest = n % numprocs;
    int** local_A = (int**)malloc((size + rest + 2 * k) * sizeof(int*));
    int** local_B = (int**)malloc((size + rest + 2 * k) * sizeof(int*));
    for(int i = 0; i < size + rest + 2 * k; i++) {
        local_A[i] = (int*)malloc(m * sizeof(int));
        local_B[i] = (int*)malloc(m * sizeof(int));   
    }  
    if(myid != 0 && myid != numprocs - 1){
        for(int i = 0 ; i < size + rest + 2 * k ; i++) {
            for(int j = 0; j < m; j++){
                local_A[i][j] = A[myid * size + i - k][j];
                local_B[i][j] = 0;
            }
        }     
    }
    else if (myid == 0){
        for(int i = 0; i < k; i++){
            for(int j = 0; j < m; j++){
                local_A[i][j] = A[n * size + i - k][j];
                local_B[i][j] = 0;
            }
        }
        for(int i = k; i < size + rest + 2 * k ; i++) {
            for(int j = 0; j < m; j++){
                local_A[i][j] = A[i - k][j];
                local_B[i][j] = 0;
            }
        }   
    }
    else{
        for(int i = 0; i < size + rest + k ; i++) {
            for(int j = 0; j < m; j++){
                local_A[i][j] = A[myid * size + i - k][j];
                local_B[i][j] = 0;
            }
        }
        for(int i = 0; i < k; i++){
            for(int j = 0; j < m; j++){
                local_A[size + rest + k + i][j] = A[i][j];
                local_B[i][j] = 0;
            }
        }  
    }   
printf("test 3 %d\n", myid);
    int front = (myid + 1) % numprocs;
    int back = myid - 1;
    if(back < 0)
        back += numprocs;
    //calculate
    for(int x = 0; x < t; x++) {
        if(x % 2 == 0) { //local_A -> local_B
            for(int y = k; y < size + rest + k; y++) {
                for(int z = 0; z < k; z++) {
                    local_B[y][z] = 0;
                    for(int i = -k; i <= k ; i++){
                        for(int j = -k; j < -z ; j++){
                            local_B[y][z] += local_A[y + i][z + j + m] * K[i+k][j+k];
                        }
                        for(int j = -z; j <= k; j++){
                            local_B[y][z] += local_A[y + i][z + j] * K[i+k][j+k];
                        }
                    }
                    local_B[y][z] /= D2;
                }
                for(int z = k; z < m - k; z++) {
                    local_B[y][z] = 0;
                    for(int i = -k; i <= k ; i++){
                        for(int j = -k; j <= k; j++){
                            local_B[y][z] += local_A[y + i][z + j] * K[i+k][j+k];
                        }
                    }
                    local_B[y][z] /= D2;
                }   
                for(int z = m - k; z < m; z++) {
                    local_B[y][z] = 0;
                    for(int i = -k; i <= k ; i++){
                        for(int j = -k; j <= k; j++){
                            local_B[y][z] += local_A[y + i][(z + j) % m] * K[i+k][j+k];
                        }                  
                    }
                    local_B[y][z] /= D2;
                }       
            }     
            for(int i = 0; i < k; i++){
                //send
                MPI_Send(local_B[1], m, MPI_INT, back, 0, MPI_COMM_WORLD);
                MPI_Send(local_B[size + rest], m, MPI_INT, front, 0, MPI_COMM_WORLD);           
                //receive
                MPI_Recv(local_B[0], m, MPI_INT, back, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);          
                MPI_Recv(local_B[size + rest + 1], m, MPI_INT, front, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);                 
            }
printf("test 4\n");
        } 
        else {          //local_B -> local_A
            for(int y = k; y < size + rest + k; y++) {
                for(int z = 0; z < k; z++) {
                    local_A[y][z] = 0;
                    for(int i = -k; i <= k ; i++){
                        for(int j = -k; j < -z ; j++){
                            local_A[y][z] += local_B[y + i][z + j + m] * K[i+k][j+k];
                        }
                        for(int j = -z; j <= k; j++){
                            local_A[y][z] += local_B[y + i][z + j] * K[i+k][j+k];
                        }
                    }
                    local_A[y][z] /= D2;
                }
                for(int z = k; z < m - k; z++) {
                    local_A[y][z] = 0;
                    for(int i = -k; i <= k ; i++){
                        for(int j = -k; j <= k; j++){
                            local_A[y][z] += local_B[y + i][z + j] * K[i+k][j+k];
                        }
                    }
                    local_A[y][z] /= D2;
                }
                for(int z = m - k; z < m; z++) {
                    local_A[y][z] = 0;
                    for(int i = -k; i <= k ; i++){
                        for(int j = -k; j <= k; j++){
                            local_A[y][z] += local_B[y + i][(z + j) % m] * K[i+k][j+k];
                        }
                    }
                    local_A[y][z] /= D2;
                }
            }
            for(int i = 0; i < k; i++){
                MPI_Send(local_A[1], m, MPI_INT, back, 0, MPI_COMM_WORLD);
                MPI_Send(local_A[size + rest], m, MPI_INT, front, 0, MPI_COMM_WORLD);           
                //receive
                MPI_Recv(local_A[0], m, MPI_INT, back, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);          
                MPI_Recv(local_A[size + rest + 1], m, MPI_INT, front, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
            }        
        }
    }
printf("test 5\n");
    //size += 2;
    int *result = (int*)malloc((size + n / numprocs) * m * sizeof(int));
    if(t % 2 == 0){         //A->B->A
        for(int i=0;i<size+rest;i++){
            for(int j=0;j<m;j++){
                result[i*m + j] = local_A[i+k][j];
            }
        }
    }
    else{
        for(int i=0;i<size+rest;i++){
            for(int j=0;j<m;j++){
                result[i*m + j] = local_B[i+k][j];
            }
        }
    }
    if(myid != 0){
        MPI_Send(result, (size + rest) * m, MPI_INT, 0, 0, MPI_COMM_WORLD); 
    }
    else {
        for(int i = 0; i < size * m; i++){
            printf("%d ", result[i]);
        }
        for (int v = 1; v < numprocs - 1; v++) {
            MPI_Recv(result, size * m, MPI_INT, v, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for(int i = 0; i < size * m;i++){
                 printf("%d ", result[i]);
            }
        }
        MPI_Recv(result, (size + n % numprocs) * m, MPI_INT, numprocs - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for(int i = 0; i < (size + n % numprocs) * m; i++){
            printf("%d ", result[i]);
        }
    }
    MPI_Finalize();
    return 0;
}