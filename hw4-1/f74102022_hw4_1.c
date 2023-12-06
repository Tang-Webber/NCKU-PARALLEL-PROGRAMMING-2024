#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

void *CNN(void* rank);

int thread_count;
pthread_barrier_t barrier;

int t, n, m;
int count = 0;
char input[50];
int A[1000][1000];
int B[1000][1000];
int K[15][15];
int size;
int k, l;
int D1, D2, D1D2;

int main( int argc, char *argv[]){
    long thread;
    pthread_t* thread_handles;

    thread_count = strtol(argv[1], NULL, 10);
    thread_handles = malloc(thread_count * sizeof(pthread_t));
    pthread_barrier_init(&barrier, NULL, thread_count);

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
    fscanf(input_file, "%d %d", &D1, &D2);
    for(int i = 0; i < D1; i++) {
        for(int j = 0; j < D2; j++) {
            fscanf(input_file, "%d", &K[i][j]);
        }
    }
    fclose(input_file);

    size = n / thread_count;
    k = D1 / 2;             //5*3 -> k = 2, l = 1
    l = D2 / 2;
    D1D2 = D1 * D2 ;

    if(n > 16){
        for(thread = 0; thread < thread_count; thread++){
            pthread_create(&thread_handles[thread], NULL, CNN, (void*) thread);
        }

        for(thread = 0; thread < thread_count; thread++){
            pthread_join(thread_handles[thread], NULL);
        }        
        //print
        if(t % 2 == 0){         //A->B->A
            for(int i=0;i<n;i++){
                for(int j=0;j<m;j++){
                    printf("%d ", A[i][j]);
                }
            }
        }
        else{
            for(int i=0;i<n;i++){
                for(int j=0;j<m;j++){
                    printf("%d ", B[i][j]);
                }
            }
        }
    }
    else{       //serial
        size = n;
        int** local_A = (int**)malloc((size + 2 * k) * sizeof(int*));
        int** local_B = (int**)malloc((size + 2 * k) * sizeof(int*));
        for(int i = 0; i < size + 2 * k; i++) {
            local_A[i] = (int*)malloc(m * sizeof(int));
            local_B[i] = (int*)malloc(m * sizeof(int));   
        }  
        for(int i = 0; i < k; i++){
            for(int j = 0; j < m; j++){
                local_A[i][j] = A[n + i - k][j];
                local_B[i][j] = 0;
            }
        }
        for(int i = k; i < size + k ; i++) {
            for(int j = 0; j < m; j++){
                local_A[i][j] = A[i - k][j];
                local_B[i][j] = 0;
            }
        }    
        for(int i = size + k; i < size + 2 * k ; i++) {
            for(int j = 0; j < m; j++){
                local_A[i][j] = A[i - size - k][j];
                local_B[i][j] = 0;
            }
        }                
        for(int x = 0; x < t; x++) {
            if(x % 2 == 0) { //local_A -> local_B
                for(int y = k; y < size + k; y++) {
                    //-l m l 
                    for(int z = 0; z < l; z++) {
                        local_B[y][z] = 0;
                        for(int i = -k; i <= k ; i++){
                            for(int j = -l; j < -z ; j++){
                                local_B[y][z] += local_A[y + i][z + j + m] * K[i+k][j+l];
                            }
                            for(int j = -z; j <= l; j++){
                                local_B[y][z] += local_A[y + i][z + j] * K[i+k][j+l];
                            }
                        }
                        local_B[y][z] /= D1D2;
                    }
                    for(int z = l; z < m - l; z++) {
                        local_B[y][z] = 0;
                        for(int i = -k; i <= k ; i++){
                            for(int j = -l; j <= l; j++){
                                local_B[y][z] += local_A[y + i][z + j] * K[i+k][j+l];
                            }
                        }
                        local_B[y][z] /= D1D2;
                    }   
                    for(int z = m - l; z < m; z++) {
                        local_B[y][z] = 0;
                        for(int i = -k; i <= k ; i++){
                            for(int j = -l; j <= l; j++){
                                local_B[y][z] += local_A[y + i][(z + j) % m] * K[i+k][j+l];
                            }                  
                        }
                        local_B[y][z] /= D1D2;
                    }    
                }       
                for(int i = 0; i < k; i++){
                    for(int j = 0; j < m; j++){
                        local_B[i][j] = local_B[size+i][j];
                        local_B[size + k + i] = local_B[k+i];
                    }
                }                                            
            } 
            else {          //local_B -> local_A
                for(int y = k; y < size + k; y++) {
                    for(int z = 0; z < l; z++) {
                        local_A[y][z] = 0;
                        for(int i = -k; i <= k ; i++){
                            for(int j = -l; j < -z ; j++){
                                local_A[y][z] += local_B[y + i][z + j + m] * K[i+k][j+l];
                            }
                            for(int j = -z; j <= l; j++){
                                local_A[y][z] += local_B[y + i][z + j] * K[i+k][j+l];
                            }
                        }
                        local_A[y][z] /= D1D2;
                    }
                    for(int z = l; z < m - l; z++) {
                        local_A[y][z] = 0;
                        for(int i = -k; i <= k ; i++){
                            for(int j = -l; j <= l; j++){
                                local_A[y][z] += local_B[y + i][z + j] * K[i+k][j+l];
                            }
                        }
                        local_A[y][z] /= D1D2;
                    }
                    for(int z = m - l; z < m; z++) {
                        local_A[y][z] = 0;
                        for(int i = -k; i <= k ; i++){
                            for(int j = -l; j <= l; j++){
                                local_A[y][z] += local_B[y + i][(z + j) % m] * K[i+k][j+l];
                            }
                        }
                        local_A[y][z] /= D1D2;
                    }
                }    
                for(int i = 0; i < k; i++){
                    for(int j = 0; j < m; j++){
                        local_A[i][j] = local_A[size+i][j];
                        local_A[size + k + i] = local_A[k+i];
                    }
                }    
            }
        }
        
        if(t % 2 == 0){         //A->B->A
            for(int i=0;i<size;i++){
                for(int j=0;j<m;j++){
                    printf("%d ", local_A[i+k][j]);
                }
            }
        }
        else{
            for(int i=0;i<size;i++){
                for(int j=0;j<m;j++){
                    printf("%d ", local_B[i+k][j]);
                }
            }
        } 
    }

    pthread_barrier_destroy(&barrier);
    return 0;
}

void* CNN(void* rank){
    long my_rank = (long)rank;
    int first = my_rank * size;
    int rest = 0;
    if(my_rank == thread_count - 1){
        rest = (n - 1) % thread_count;
    } 
    //calculate
    
    for(int x = 0; x < t; x++) {
        if(x % 2 == 0) { // A -> B
            for(int y = first; y < first + size + rest; y++) {
                //-l m l 
                for(int z = 0; z < l; z++) {
                    B[y][z] = 0;
                    for(int i = -k; i <= k ; i++) {
                        for(int j = -l; j < -z ; j++) {
                            B[y][z] += A[(y + i) < 0 ? (y + i + n) : ((y + i) % n)][z + j + m] * K[i+k][j+l];
                        }
                        for(int j = -z; j <= l; j++) {
                            B[y][z] += A[(y + i) < 0 ? (y + i + n) : ((y + i) % n)][z + j] * K[i+k][j+l];
                        }
                    }
                    B[y][z] /= D1D2;
                }
                for(int z = l; z < m - l; z++) {
                    B[y][z] = 0;
                    for(int i = -k; i <= k ; i++) {
                        for(int j = -l; j <= l; j++) {
                            B[y][z] += A[(y + i) < 0 ? (y + i + n) : ((y + i) % n)][z + j] * K[i+k][j+l];
                        }
                    }
                    B[y][z] /= D1D2;
                }
                for(int z = m - l; z < m; z++) {
                    B[y][z] = 0;
                    for(int i = -k; i <= k ; i++) {
                        for(int j = -l; j <= l; j++) {
                            B[y][z] += A[(y + i) < 0 ? (y + i + n) : ((y + i) % n)][(z + j) % m] * K[i+k][j+l];
                        }                  
                    }
                    B[y][z] /= D1D2;
                }       
            }     
        } 
        else { // B -> A
            for(int y = k; y < size + rest + k; y++) {
                for(int z = 0; z < l; z++) {
                    A[y][z] = 0;
                    for(int i = -k; i <= k ; i++) {
                        for(int j = -l; j < -z ; j++) {
                            A[y][z] += B[(y + i) < 0 ? (y + i + n) : ((y + i) % n)][z + j + m] * K[i+k][j+l];
                        }
                        for(int j = -z; j <= l; j++) {
                            A[y][z] += B[(y + i) < 0 ? (y + i + n) : ((y + i) % n)][z + j] * K[i+k][j+l];
                        }
                    }
                    A[y][z] /= D1D2;
                }
                for(int z = l; z < m - l; z++) {
                    A[y][z] = 0;
                    for(int i = -k; i <= k ; i++) {
                        for(int j = -l; j <= l; j++) {
                            A[y][z] += B[(y + i) < 0 ? (y + i + n) : ((y + i) % n)][z + j] * K[i+k][j+l];
                        }
                    }
                    A[y][z] /= D1D2;
                }
                for(int z = m - l; z < m; z++) {
                    A[y][z] = 0;
                    for(int i = -k; i <= k ; i++) {
                        for(int j = -l; j <= l; j++) {
                            A[y][z] += B[(y + i) < 0 ? (y + i + n) : ((y + i) % n)][(z + j) % m] * K[i+k][j+l];
                        }
                    }
                    A[y][z] /= D1D2;
                }
            } 
        }
        pthread_barrier_wait(&barrier); 
    }
    return NULL;
}