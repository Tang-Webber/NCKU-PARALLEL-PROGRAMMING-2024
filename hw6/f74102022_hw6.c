#include "mpi.h"
#include <omp.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <time.h>

int main( int argc, char *argv[]){
    srand(time(NULL));
    char input[50];
    int myid, numprocs;
    int n, m, t;    //n : cities m : ants t : iterations
    int weight[100][100];
    //int global_route[100];

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);

    if (myid == 0) {
        //scanf("%s", input);
        //FILE *input_file = fopen(input, "r");
        //if(input_file == NULL){
        //    printf("could not open file %s\n", input);
        //    fclose(input_file);
        //    return 1;
        //}

        //fscanf(input_file, "%d %d %d", &n, &m, &t);
        scanf("%d %d %d", &n, &m, &t);
        for (int i = 0; i < n ; i++) {
            for (int j = 0; j < n ; j++) {
                //fscanf(input_file, "%d", &weight[i][j]);
                scanf("%d", &weight[i][j]);
            }
        }      
        //fclose(input_file);
    }
    //boardcast   
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&t, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(weight, 10000, MPI_INT, 0, MPI_COMM_WORLD);
    //Iniitialize
    double pheromone[100][100];
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            pheromone[i][j] = 0;
        }
    }
    double alpha = 1;
    double beta = 0.2;
    int local_min = 999999;
    int global_min = 999999;

    int ant_count = m / numprocs;
    int rest = 0;
    if(myid == numprocs - 1)
        rest = m % numprocs;
    //Iteration t
    for(int w = 0; w < t; w++){
        //Initialize
        double temp_p[100][100];
        for(int i = 0; i < n; i++){
            for(int j = 0; j < n; j++){
                temp_p[i][j] = 0;
            }
        }
        #pragma omp parallel for
        for(int x = 0; x < ant_count + rest; x++){
            //every ant has its own 
            bool picked[100];
            int route[100];
            double pij[100];
            double pij_sum;
            int start, next;
            double sum = 0.0;
            //Initialize
            for(int i = 0; i < n; i++){
                picked[i] = false;
            }
            //pick start point randomly
            start = rand() % n;
            picked[start] = true;
            route[0] = start;
            //Go throuth all city => y = 0 ~ (n-1)
            for(int y = 1; y < n; y++){
                //calculate Pij
                pij_sum = 0;
                for(int z = 0; z < n; z++){
                    if(!picked[z]){
                        pij[z] = pow(pheromone[start][z], alpha) + pow(weight[start][z], (-1) * beta);                      
                    }
                    else{
                        pij[z] = 0.0;
                    }
                    pij_sum += pij[z];
                }
                //randomly choose next
                int random_integer = rand();
                double random = (double)random_integer / RAND_MAX;
                random *= pij_sum;
 
                float cumulateP = 0.0;
                for (int i = 0; i < n; i++) {
                    cumulateP += pij[i];
                    if (random <= cumulateP) {
                        next = i;
                        break;
                    }
                }
                //go to next vertex
                sum += weight[start][next];
                route[y] = next;
                picked[next] = true;
                start = next;
            }
            //critical section
            if(sum <= local_min){
                #pragma omp critical 
                {
                    local_min = sum;
                }
            }
            #pragma omp critical 
            {
                for(int i = 1; i < n; i++){
                    //Q = 9000
                    temp_p[route[i-1]][route[i]] += 9000.0 / (double)sum;
                }
            }
        } 
        //Update Phenomone Matrix Using MPI_Send/Recv
        if(myid != 0){
            MPI_Send(temp_p, 10000, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        }
        else{
            //#pragma omp parallel for
            for(int j = 0; j < n; j++){
                for(int k = 0; k < n; k++){
                    pheromone[j][k] *= 0.7;
                    pheromone[j][k] += temp_p[j][k];
                }
            }
            for(int i = 1; i < numprocs; i++){
                MPI_Recv(temp_p, 10000, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
                //#pragma omp parallel for
                for(int j = 0; j < n; j++){
                    for(int k = 0; k < n; k++){
                        pheromone[j][k] += temp_p[j][k];
                    }
                }
            }
        }
        MPI_Bcast(pheromone, 10000, MPI_DOUBLE, 0, MPI_COMM_WORLD);      
    }
    MPI_Reduce(&local_min, &global_min, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
    if(myid == 0){
        printf("%d", global_min);
    }
    MPI_Finalize();
    return 0;
}