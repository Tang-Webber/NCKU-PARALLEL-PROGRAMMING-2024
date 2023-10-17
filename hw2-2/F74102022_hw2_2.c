#include "mpi.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

short Adj[50000][50000];

int main( int argc, char *argv[]){
    int n, myid, numprocs;
    char input[50];
    
    short **A;
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
        fscanf(input_file, "%d", &n);
        A = (short**)malloc(n  * sizeof(short*));
        for (int i = 0; i < n; i++) {
            A[i] = (short*)malloc(n  * sizeof(short*));
        }    
        int x, y;
        short temp;
        while (!feof(input_file)) {
            fscanf(input_file, "%d %d %hd", &x, &y, &temp);
printf("%d %d %hd\n", x, y, temp);
            Adj[x][y] = temp;
        }
        fclose(input_file);
    }
    if(myid != 0){
        A = (short**)malloc(n  * sizeof(short*));
        for (int i = 0; i < n; i++) {
            A[i] = (short*)malloc(n  * sizeof(short*));
        }  
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    //MPI_Bcast(A, 25000000, MPI_INT, 0, MPI_COMM_WORLD);
printf("%hd\n", Adj[0][2]);
    MPI_Finalize();
    return 0;
}