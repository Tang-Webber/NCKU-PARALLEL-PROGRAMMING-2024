#include "mpi.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>

struct Point {
    int x, y;
}P[400];
struct Edge {
    int x, y;
    double w;
}E[400];

double Distance(struct Point p1, struct Point p2) {
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

void custom_min(void *in, void *inout, int *len, MPI_Datatype *datatype) {
    struct Edge* new = (struct Edge*)in;
    struct Edge* old = (struct Edge*)inout;
    if (new->w < old->w)
        *old = *new;
}

int cross(struct Point o, struct Point a, struct Point b) {
    return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}
int compare(const void* a, const void* b)
{
    return ((struct Point*)a)->x - ((struct Point*)b)->x;
}

int main( int argc, char *argv[])
{
    int n, myid, numprocs;
    int count = 0;
    int num;
    double sum = 0;
    double final = 1000;
    bool point[20];     //vertex
    char input[50];
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    MPI_Op custom_op;
    MPI_Op_create((MPI_User_function *)custom_min, 1, &custom_op);
    //scan the input
    if (myid == 0) {
        scanf("%s", input);
        FILE *input_file = fopen(input, "r");
        if(input_file == NULL){
            printf("could not open file %s\n", input);
            fclose(input_file);
            return 1;
        }
        fscanf(input_file, "%d", &n);
        for (int i = 0; i < n; i++) {
            fscanf(input_file, "%d %d", &P[i].x, &P[i].y);
        }
        fclose(input_file);
        qsort(P, n, sizeof(struct Point), compare);         //sort

        int up = 0;
        int down = 0;
        struct Point *upper = (struct Point*)malloc(n * sizeof(struct Point));
        struct Point *lower = (struct Point*)malloc(n * sizeof(struct Point));
        //Andrew's Monotone Chain
        for (int i = 0; i < n; i++){
            while (down >= 2 && cross(lower[down-2], lower[down-1], P[i]) <= 0) down--;
            lower[down++] = P[i];
            while (up >= 2 && cross(upper[up-2], upper[up-1], P[i]) >= 0) up--;
            upper[up++] = P[i];
        }    
        //Combine
        struct Point *vertex = (struct Point*)malloc(n * sizeof(struct Point)); 
        for(int i = 0; i < up;i++){
            vertex[i] = upper[i];
        }
        for(int j = 0; j < down - 2; j++){
            vertex[up + j] = lower[down - 2 - j];
        }
        num = up + down - 2;
        //edge_matrix
        for(int i = 0; i < num; i++){
            point[i] = false;
        }   
        for(int i = 1; i < num; i++){
            for(int j = 0; j < i; j++){
                E[count].x = i;
                E[count].y = j;
                E[count].w = sqrt((pow((double)(vertex[i].x - vertex[j].x), 2) + pow((double)(vertex[i].y - vertex[j].y), 2)));
                count++;
            }
            for(int k=0;k<n;k++){
                if(P[k].x == vertex[i].x && P[k].y == vertex[i].y){
                    point[k] = true;
                }
            }
        }
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&num, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&count, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&point, n, MPI_BOOL, 0, MPI_COMM_WORLD);
    MPI_Bcast(E, count * sizeof(struct Edge), MPI_BYTE, 0, MPI_COMM_WORLD);
    MPI_Bcast(P, n * sizeof(struct Point), MPI_BYTE, 0, MPI_COMM_WORLD);
    bool pick[20];      //vertex
    for(int i = 0; i < num; i++){
        pick[i] = false;
    }
    int local_count = count / numprocs;
    int rest = 0;
    if(myid == numprocs - 1)
        rest = count % numprocs;     
    struct Edge temp;
    struct Edge result;
    int index;
    pick[0] = true;     //pick start vertex
    for(int i=0; i < num-1; i++){
        temp.w = 100;
        for(int j=0;j<local_count + rest;j++){
            index = myid * local_count + j;
            if( ((pick[E[index].x] && !pick[E[index].y]) || (!pick[E[index].x] && pick[E[index].y])) && E[index].w < temp.w){
                temp = E[index];
            }
        }
        MPI_Allreduce(&temp, &result, sizeof(struct Edge), MPI_BYTE, custom_op, MPI_COMM_WORLD);
        pick[result.x] = true;
        pick[result.y] = true;
        if(myid == 0){
            sum += result.w;  
        } 
    }
    final = sum;
    //consider inside point
    for(int x=0; x < n;x++){
        if(!point[x]){
            for(int j = 0; j < num; j++){
                E[count].x = num;
                E[count].y = j;
                E[count].w = sqrt((pow((double)(P[x].x - vertex[j].x), 2) + pow((double)(P[x].y - vertex[j].y), 2)));
                count++;
            }
            for(int i = 0; i < num; i++){
                pick[i] = false;
            }
            local_count = count / numprocs;
            if(myid == numprocs - 1)
                rest = count % numprocs;  
            pick[0] = true;     //pick start vertex
            sum = 0;
            for(int i=0; i < num; i++){
                temp.w = 100;
                for(int j=0;j<local_count + rest;j++){
                    index = myid * local_count + j;
                    if( ((pick[E[index].x] && !pick[E[index].y]) || (!pick[E[index].x] && pick[E[index].y])) && E[index].w < temp.w){
                        temp = E[index];
                    }
                }
                MPI_Allreduce(&temp, &result, sizeof(struct Edge), MPI_BYTE, custom_op, MPI_COMM_WORLD);
                pick[result.x] = true;
                pick[result.y] = true;
                if(myid == 0){
                    sum += result.w;  
                } 
            }
            if(final > sum)
                final = sum;
        }
    }



    if(myid == 0){      
        printf("%.4f", final);
    }

    MPI_Finalize();
    return 0;
}