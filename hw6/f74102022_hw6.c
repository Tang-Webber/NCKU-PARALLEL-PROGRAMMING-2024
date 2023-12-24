#include "mpi.h"
#include <omp.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <time.h>

//Hyper_parameter : 
//α = 1, β = 2, ρ = 0.7, Q = 100

int main( int argc, char *argv[]){
    srand(5);
    char input[50];
    int myid, numprocs;
    int n, m, t;    //n : cities m : ants t : iterations
    int weight[50][50];
    int global_route[50];

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

        fscanf(input_file, "%d %d %d", &n, &m, &t);
        for (int i = 0; i < n ; i++) {
            for (int j = 0; j < n ; j++) {
                fscanf(input_file, "%d", &weight[i][j]);
            }
        }      
        fclose(input_file);
    }
    //boardcast   
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&t, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(weight, 2500, MPI_INT, 0, MPI_COMM_WORLD);
    //Iniitialize
    double pheromone[50][50];
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            pheromone[i][j] = 0;
        }
    }
    double alpha = 0.5;
    double beta = 1.0;
    int local_min = 999999;
    int global_min = 999999;

    int ant_count = n / numprocs;
    int rest = 0;
    if(myid == numprocs - 1)
        rest = n % numprocs;
    //Iteration t
    for(int w = 0; w < t; w++){
        //Initialize
        double temp_p[50][50];
        for(int i = 0; i < n; i++){
            for(int j = 0; j < n; j++){
                temp_p[i][j] = 0;
            }
        }
        //#pragma omp parallel for
        for(int x = 0; x < ant_count + rest; x++){
            //every ant has its own 
            bool picked[50];
            int route[50];
            int pij[50];
            int pij_sum;
            int start, next, pre;
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
                        pij[z] = pow(pheromone[start][z], alpha) + pow((1 / weight[start][z]), beta);
                    }
                    else{
                        pij[z] = 0.0;
                    }
                    pij_sum += pij[z];
                }
                //randomly choose next
                double random = ((double)rand() / RAND_MAX) * pij_sum;
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
            //critical
            //#pragma omp critical {}
            if(sum <= local_min){
                local_min = sum;
            }
                
            for(int i = 1; i < n; i++){
                //Q = 100
                temp_p[route[i-1]][route[i]] += 100 / sum;
            }
        } 
        //Update Phenomone Matrix Using MPI
        if(myid != 0){
            MPI_Send(temp_p, 2500, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        }
        else{
            //#pragma omp parallel for
            for(int j = 0; j < n; j++){
                //#pragma omp parallel for
                for(int k = 0; k < n; k++){
                    pheromone[j][k] *= 0.7;
                    pheromone[j][k] += temp_p[j][k];
                }
            }
            for(int i = 1; i < numprocs; i++){
                MPI_Recv(temp_p, 2500, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
                //#pragma omp parallel for
                for(int j = 0; j < n; j++){
                    //#pragma omp parallel for
                    for(int k = 0; k < n; k++){
                        pheromone[j][k] += temp_p[j][k];
                    }
                }
            }
        }
        MPI_Bcast(pheromone, 2500, MPI_DOUBLE, 0, MPI_COMM_WORLD);      
    }
    MPI_Reduce(&local_min, &global_min, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
    if(myid == 0){
        printf("%d", global_min);
    }
    MPI_Finalize();
    return 0;
}