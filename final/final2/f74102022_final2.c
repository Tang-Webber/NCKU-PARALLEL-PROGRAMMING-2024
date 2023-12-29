#include <omp.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

int Adj[3000][2][3000];            //adjacency matrix
short count[3000];

int main( int argc, char *argv[]){
    int n;
    char input[50];
    int size;
    bool selected[50000];
    int dist[50000];  
    int final[50000]; 
    int min[2];         //(index, dist)
    
    int local_min[16][2];

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
    int temp;
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            fscanf(input_file, "%d", &temp);
            Adj[i][0][count[i]] = j;
            Adj[i][1][count[i]] = temp;
            count[i]++;
        }
    }
    fclose(input_file);

    //initialize
    selected[0] = true;
    dist[0] = 0;     
    #pragma omp parallel for       
    for(int i = 1; i < n; i++){
        dist[i] = 9999;
        selected[i] = false;
    }
    #pragma omp parallel for   
    for(int i = 0; i < count[0]; i++){           
        dist[Adj[0][0][i]] = Adj[0][1][i];
    }
    for(int i = 1; i < n; i++){
        min[1] = 9999;
        #pragma omp parallel for   
        for(int j = 0; j < n; j++){
            if(!selected[j]){
                #pragma omp critical
                {
                    if(dist[j] < min[1]){
                        min[0] = j;
                        min[1] = dist[j];
                    }
                }
            }
        }             
        selected[min[0]] = true;
        #pragma omp parallel for   
        for(int j = 0; j < count[min[0]]; j++){
            if(!selected[Adj[min[0]][0][j]] && dist[Adj[min[0]][0][j]] > min[1] + Adj[min[0]][1][j]){
                dist[Adj[min[0]][0][j]] = min[1] + Adj[min[0]][1][j];
            }
        }
    }
    for(int i = 0; i < n; i++){
        printf("%d ", dist[i]);
    }

    return 0;
}

