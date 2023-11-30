#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int thread_count;
int flag, sum;
int n;
int neural[50];
pthread_mutex_t mutex;

void *multiply(void* rank);

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
    for(int i=0; i<n; i++){
        fscanf(input_file, "%d", &neural[i]);
    }
    fclose(input_file);

    pthread_mutex_init(&mutex, NULL);
    //cauculate
    //sum = 0;
    flag = 0;
    for(thread = 0; thread < thread_count; thread++){
        pthread_create(&thread_handles[thread], NULL, multiply, (void*) thread);
    }

    for(thread = 0; thread < thread_count; thread++){
        pthread_join(thread_handles[thread], NULL);
    }    

    //output
    printf("%d", sum);
    pthread_mutex_destroy(&mutex);
    return 0;
}

void* multiply(void* rank){
    long my_rank = (long)rank;
    int local_count = (n - 1) / thread_count;
    int first = my_rank * local_count;
    int rest = 0;
    if(my_rank == thread_count - 1){
        rest = (n - 1) % thread_count;
    }
    int local_sum = 0;
    for(int i = first; i < first + local_count + rest; i++){
        local_sum += neural[i] * neural[i+1];
    }
    //busy waiting
    //while(my_rank != flag);
    //sum += local_sum;
    //flag++;

    //mutex
    pthread_mutex_lock(&mutex);
    sum += local_sum;
    pthread_mutex_unlock(&mutex);

    return NULL;
}