#include "mpi.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

short Adj[50000][50000];            //adjacency matrix         

int main( int argc, char *argv[]){
    int n, myid, numprocs;
    char input[50];
    int size;
    int choose[50000] = {0};  
    bool selected[50000];
    int dist[50000];   
    int min[2];         //(index, dist)

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
  
        for(int i=0; i<n;i++){
            for(int j=0;j<n;j++){
                Adj[i][j] = 1000;
            }
            choose[i] = 1000;
            selected[i] = false;
        }
        choose[0] = 0;

        int x, y;
        short temp;
        while (!feof(input_file)) {
            fscanf(input_file, "%d %d %hd", &x, &y, &temp);
            Adj[x][y] = temp;
        }
        fclose(input_file);
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(Adj, 25000000, MPI_INT, 0, MPI_COMM_WORLD);
    size = n / numprocs;

    if(size >= 0){                      //6
        if(myid == 0){
for(int z=0;z<n;z++){
    printf("%d ", dist[z]);
}
printf("\n");
            selected[0] = true;
            dist[0] = 0;
            for(int i=0;i<n;i++){           //initialize
                if(Adj[0][i] != 1000){
                    dist[i] = Adj[0][i];
                }
            }
            for(int i = 1; i < n; i++){
                min[1] = 1000;
                for(int j = 0; j < n; j++){
                    if(!selected[j] && dist[j] < min[1]){
                        min[0] = j;
                        min[1] = dist[j];
                    }
                }             
                selected[min[0]] = true;
                for(int j = 0; j < n; j++){
                    if(!selected[j] && dist[j] > dist[min[0]] + Adj[min[0]][j]){
                        dist[j] = dist[min[0]] + Adj[min[0]][j];
                    }
                }
            }
        }
    }
    else{                       //1000 50000

    }

//printf("%hd\n", Adj[0][2]);
    if(myid == 0){
        for(int i=0;i<n;i++){
            printf("%d ", dist[i]);
        }
    }



    MPI_Finalize();
    return 0;
}