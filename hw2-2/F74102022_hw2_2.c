#include "mpi.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

int Adj[50000][2][100];            //adjacency matrix
short count[50000];

int main( int argc, char *argv[]){
    int n, myid, numprocs;
    char input[50];
    int size;
    bool selected[50000];
    int dist[50000];  
    int final[50000]; 
    int min[2];         //(index, dist)
    
    int local_min[16][2];

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
            count[i] = 0;
        }

        int x, y, temp;
        while (!feof(input_file)) {
            fscanf(input_file, "%d %d %d", &x, &y, &temp);
            Adj[x][0][count[x]] = y;
            Adj[x][1][count[x]] = temp;
            count[x]++;
        }
        fclose(input_file);
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    size = n / numprocs;
    
    if(size < 70){                      //6, 1000
        if(myid == 0){
            //initialize
            selected[0] = true;
            dist[0] = 0;            
            for(int i = 1; i < n; i++){
                dist[i] = 99999;
                selected[i] = false;
            }
            for(int i = 0; i < count[0]; i++){           
                dist[Adj[0][0][i]] = Adj[0][1][i];
            }
            for(int i = 1; i < n; i++){
                min[1] = 99999;
                for(int j = 0; j < n; j++){
                    if(!selected[j] && dist[j] < min[1]){
                        min[0] = j;
                        min[1] = dist[j];
                    }
                }             
                selected[min[0]] = true;
                for(int j = 0; j < count[min[0]]; j++){
                    if(!selected[Adj[min[0]][0][j]] && dist[Adj[min[0]][0][j]] > min[1] + Adj[min[0]][1][j]){
                        dist[Adj[min[0]][0][j]] = min[1] + Adj[min[0]][1][j];
                    }
                }
            }
            for(int i = 0; i < n; i++){
                printf("%d ", dist[i]);
            }
        }
    }

    if(size > 70){                      //1000 50000
        //each process calculate size = n / numprocs  
        MPI_Bcast(count, 50000, MPI_SHORT, 0, MPI_COMM_WORLD);       
        //for(int i=0;i<n;i++){
        //    MPI_Bcast(Adj[i][0], count[i], MPI_INT, 0, MPI_COMM_WORLD);
        //    MPI_Bcast(Adj[i][1], count[i], MPI_INT, 0, MPI_COMM_WORLD);
        //}
        
        for(int i=0;i<n;i++){
            if(myid == 0){
                for(int j=1;j<numprocs;j++){
                    MPI_Send(Adj[i][0], count[i], MPI_INT, j, 0, MPI_COMM_WORLD);
                    MPI_Send(Adj[i][1], count[i], MPI_INT, j, 0, MPI_COMM_WORLD);
                }
            }
            else{
                MPI_Recv(Adj[i][0], count[i], MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(Adj[i][1], count[i], MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }    
        
        //short *temp = (short *)malloc(size * sizeof(short));
        int start = myid * size;
        int end = start + size;
        //initialize
        for(int i = start; i < end; i++){
            dist[i] = 99999;
            selected[i] = false;
        }
        if(myid == 0){
            selected[0] = true;
            dist[0] = 0;            
        }                 
        for(int i = 0; i < count[0]; i++){ 
            dist[Adj[0][0][i]] = Adj[0][1][i];
        }
        //loop 49999 times
        for(int i = 1; i < n; i++){
            min[1] = 99999;
            for(int j = start; j < end; j++){
                if(!selected[j] && dist[j] < min[1]){
                    min[0] = j;
                    min[1] = dist[j];
                }
            }        
            MPI_Gather(min, 2, MPI_INT, local_min, 2, MPI_INT, 0, MPI_COMM_WORLD);
            if(myid == 0){
                for(int i=1;i< numprocs;i++){
                    if(local_min[i][1] < min[1]){
                        min[1] = local_min[i][1];
                        min[0] = local_min[i][0];
                    }
                }       
            }
            MPI_Bcast(min, 2, MPI_INT, 0, MPI_COMM_WORLD);
            //MPI_Scatter(Adj[global_min[0]], size, MPI_SHORT, temp, size, MPI_SHORT, 0, MPI_COMM_WORLD);
            //MPI_Allreduce(min, global_min, 2, MPI_INT, custom_op, MPI_COMM_WORLD);              
            selected[min[0]] = true;
            
            for(int j = 0; j < count[min[0]]; j++){
                if(!selected[Adj[min[0]][0][j]] && dist[Adj[min[0]][0][j]] > min[1] + Adj[min[0]][1][j]){
                    dist[Adj[min[0]][0][j]] = min[1] + Adj[min[0]][1][j];
                }
            }
        }
        MPI_Gather(&dist[start], size, MPI_INT, final, size, MPI_INT, 0, MPI_COMM_WORLD);
        if(myid == 0){
            for(int i=0;i<n;i++){
                printf("%d ", final[i]);
            }
        }
    }

    MPI_Finalize();
    return 0;
}