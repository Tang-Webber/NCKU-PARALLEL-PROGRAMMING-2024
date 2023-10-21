#include "mpi.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

short Adj[50000][3][2];            //adjacency matrix
short count[50000];


void custom_min(void *in, void *inout, int *len, MPI_Datatype *datatype) {
    int *in_array = (int *)in;
    int *inout_array = (int *)inout;
    if (in_array[1] == inout_array[1] && inout_array[0] > in_array[0]){
        inout_array[0] = in_array[0];
    }
    else if (in_array[1] < inout_array[1]) {
        inout_array[1] = in_array[1];
        inout_array[0] = in_array[0];
    }
}

int main( int argc, char *argv[]){
    int n, myid, numprocs;
    char input[50];
    int size;
    bool selected[50000];
    int dist[50000];  
    int final[50000]; 
    int min[2];         //(index, dist)
    int global_min[2];

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    MPI_Op custom_op;
    MPI_Op_create((MPI_User_function *)custom_min, 1, &custom_op);
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
            for(int j=0;j<3;j++){
                for(int k=0;k<2;k++){
                    Adj[i][j][k] = -1;
                }
            } 
            count[i] = 0;
        }

        int x;
        short y, temp;
        while (!feof(input_file)) {
            fscanf(input_file, "%d %hd %hd", &x, &y, &temp);
            Adj[x][count[x]][0] = y;
            Adj[x][count[x]][1] = temp;
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
                dist[Adj[0][i][0]] = Adj[0][i][1];
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
                    if(!selected[Adj[min[0]][j][0]] && dist[Adj[min[0]][j][0]] > dist[min[0]] + Adj[min[0]][j][1]){
                        dist[Adj[min[0]][j][0]] = dist[min[0]] + Adj[min[0]][j][1];
                    }
                }
            }
            for(int i = 0; i < n; i++){
                printf("%d ", dist[i]);
            }
        }
    }
/*
    if(size > 70){                      //1000 50000
        //each process calculate size = n / numprocs  
        for(int i=0;i<n;i++){
            if(myid == 0){
                for(int j=1;j<numprocs;j++){
                    MPI_Send(&Adj[i][j*size], size, MPI_SHORT, j, 0, MPI_COMM_WORLD);
                }
            }
            else{
                MPI_Recv(Adj[i], size, MPI_SHORT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            //MPI_Scatter(Adj[i], size, MPI_SHORT, local_adj[i], size, MPI_SHORT, 0, MPI_COMM_WORLD);    
        }        
        //short *temp = (short *)malloc(size * sizeof(short));
        int start = myid * size;
        //initialize
        for(int i = 0; i < size; i++){
            dist[i] = 99999;
            selected[i] = false;
        }
        if(myid == 0){
            selected[0] = true;
            dist[0] = 0;            
        }        
        //MPI_Scatter(Adj[0], size, MPI_SHORT, temp, size, MPI_SHORT, 0, MPI_COMM_WORLD);              
        for(int j = 0; j < size; j++){ 
            if(Adj[0][j] != -1){
                dist[j] = Adj[0][j];
            }
        }
        //loop 49999 times
        for(int i = 1; i < n; i++){
            min[1] = global_min[1] = 99999;
            for(int j = 0; j < size; j++){
                if(!selected[j] && dist[j] < min[1]){
                    min[0] = start + j;
                    min[1] = dist[j];
                }
            }        

            MPI_Allreduce(min, global_min, 2, MPI_INT, custom_op, MPI_COMM_WORLD);
            //MPI_Scatter(Adj[global_min[0]], size, MPI_SHORT, temp, size, MPI_SHORT, 0, MPI_COMM_WORLD);              
            if(global_min[0] == min[0])
                selected[min[0] - start] = true;
            
            for(int j = 0; j < size; j++){
                if(!selected[j] && Adj[global_min[0]][j] != -1 && dist[j] > global_min[1] + Adj[global_min[0]][j]){
                    dist[j] = global_min[1] + Adj[global_min[0]][j];
                }
            }
        }
        MPI_Gather(dist, size, MPI_INT, final, size, MPI_INT, 0, MPI_COMM_WORLD);
        if(myid == 0){
            for(int i=0;i<n;i++){
                printf("%d ", final[i]);
            }
        }
    }
*/
    MPI_Op_free(&custom_op);
    MPI_Finalize();
    return 0;
}