#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int thread_count;
pthread_mutex_t mutex;
pthread_barrier_t barrier;
int n;
int neural[50];
int dp[50][50];

void *minMultiMatrix(void* rank);

int main(int argc, char *argv[]){
    long thread;
    pthread_t* thread_handles;

    thread_count = strtol(argv[1], NULL, 10);
    thread_handles = malloc(thread_count * sizeof(pthread_t));
    //input
    char input[50];
    scanf("%s", input);
    FILE *input_file = fopen(input, "r");
    if(input_file == NULL){
        printf("could not open file %s\n", input);
        return 1;
    }
    fscanf(input_file, "%d", &n);
    neural[0] = 1;
    for(int i=1; i<=n; i++){
        fscanf(input_file, "%d", &neural[i]);
    }
    neural[n+1] = 0;
    n++;
    fclose(input_file);

    for(int i = 0; i < n; i++){
        for(int j = 0; j< n;j++){
            if(i = j)   
                dp[i][i] = 0;
            else        
                dp[i][j] = 99999;
        }
    }

    pthread_barrier_init(&barrier, NULL, thread_count);
    pthread_mutex_init(&mutex, NULL);
    for(thread = 0; thread < thread_count; thread++){
        pthread_create(&thread_handles[thread], NULL, minMultiMatrix, (void*) thread);
    }

    for(thread = 0; thread < thread_count; thread++){
        pthread_join(thread_handles[thread], NULL);
    }    

    //output
    printf("%d", dp[1][n - 1]);
    pthread_mutex_destroy(&mutex);
    pthread_barrier_destroy(&barrier);
    return 0;
}
/*
void* minMultiMatrix(void* rank){
    long my_rank = (long)rank;
    for (int len = 2; len < n; len++) {
        int count = (len - 1) / thread_count ;
        int start = my_rank * count;
        int rest = 0;
        if(my_rank == thread_count - 1)
            rest = (len - 1) % thread_count;        
        for (int i = 1; i < n - len + 1; i++) {
            int j = i + len - 1;
            for (int k = start; k < start + (count + rest); k++) {
                int cost = dp[i][i+k] + dp[i+k + 1][j] + neural[i - 1] * neural[i+k] * neural[j];
                pthread_mutex_lock(&mutex);
                if (cost < dp[i][j]) {
                    dp[i][j] = cost;
                }
                pthread_mutex_unlock(&mutex);
            }
            pthread_barrier_wait(&barrier);             
        }
    }

    return NULL;
}
*/

void* minMultiMatrix(void* rank){
    long my_rank = (long)rank;
    for (int len = 2; len < n; len++) {
        for (int i = 1; i < n - len + 1; i++) {
            int j = i + len - 1;
            dp[i][j] = 99999;
            for (int k = 0; k < len - 1; k++) {
                int cost = dp[i][i+k] + dp[i+k + 1][j] + neural[i - 1] * neural[i+k] * neural[j];
                if (cost < dp[i][j]) {
                    dp[i][j] = cost;
                }
            }
        }
    }

    return NULL;
}
