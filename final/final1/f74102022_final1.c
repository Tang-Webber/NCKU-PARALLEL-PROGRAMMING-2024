#include <omp.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

int main( int argc, char *argv[])
{
    int n, max;
    int A[80000];
    int B[80000];
    int C[120000] = {0};
    char input[50];

    //scan the input
    scanf("%s", input);
    FILE *input_file = fopen(input, "r");
    if(input_file == NULL){
        printf("could not open file %s\n", input);
        fclose(input_file);
        return 1;
    }
    fscanf(input_file, "%d %d", &n, &max);
    for (int i = 0; i < n; i++) {
        fscanf(input_file, "%d", &A[i]);
    }
    fclose(input_file);

    #pragma omp parallel for
    for(int i = 0; i < max + 1; i++){
        C[i] = 0;
    }

    for(int j = 0; j < n; j++){
        C[A[j]] = C[A[j]] + 1;
    }
    
    for(int i = 1; i <= max; i++){
        C[i] = C[i] + C[i-1];
    }

    #pragma omp parallel for
    for(int j = n - 1; j >= 0; j--){
        if(C[A[j]] > 1){
            #pragma omp critical
            {
                B[C[A[j]]] = A[j];
                C[A[j]] = C[A[j]] - 1;
            }
        }
        else{
            B[C[A[j]]] = A[j];
            C[A[j]] = C[A[j]] - 1;
        }
    }

    for(int i = 1; i < n + 1; i++){
        printf("%d ", B[i]);
    }

    return 0;
}