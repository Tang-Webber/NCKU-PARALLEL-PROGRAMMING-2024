#include "mpi.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>

struct Point {
    int x, y;
}P[20], Q[20];
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
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    struct Point *vertex = (struct Point*)malloc(n * sizeof(struct Point)); 
    if(myid == 0){
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
        for(int i = 0; i < up;i++){
            vertex[i] = upper[i];
        }
        for(int j = 0; j < down - 2; j++){
            vertex[up + j] = lower[down - 2 - j];
        }
        num = up + down - 2;
        int s = num;
        for(int i=0;i<n;i++){
            for(int j=0;j<num;j++){
                if(P[i].x == vertex[j].x && P[i].y == vertex[j].y)
                    break;
                if(j==num - 1){
                    vertex[s] = P[i];
                    s++;
                }
            }
        }
    }
    MPI_Bcast(&num, 1, MPI_INT, 0, MPI_COMM_WORLD);
    //MPI_Bcast(&count, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(point, n * sizeof(bool), MPI_BYTE, 0, MPI_COMM_WORLD);
    //MPI_Bcast(E, count * sizeof(struct Edge), MPI_BYTE, 0, MPI_COMM_WORLD);
    MPI_Bcast(P, n * sizeof(struct Point), MPI_BYTE, 0, MPI_COMM_WORLD);
    MPI_Bcast(vertex, n * sizeof(struct Point), MPI_BYTE, 0, MPI_COMM_WORLD);

    int local_count = count / numprocs;
    int rest = 0;
    if(myid == numprocs - 1)
        rest = count % numprocs;     
    struct Edge temp;
    struct Edge result;
    int index;
    bool pick[20];      //vertex

    /*
    for(int i = 0; i < num; i++){
        pick[i] = false;
    }
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

    */
for(int z=0;z<n;z++){
    printf("(%d, %d)  ", vertex[z].x, vertex[z].y);
}
printf("\n");
    //consider inside point
    int inner = n - num;
    for (int x = 0; x < (1 << inner); x++) {
        int qIndex = 0;
        for (int i = 0; i < num; i++) {
            Q[qIndex++] = vertex[i];
        }       
        for (int i = 0; i < inner; i++) {
            if ((x & (1 << i)) != 0) {
                Q[qIndex++] = vertex[num + i];
            }
        }   
if(myid==0) printf("index=%d\n", qIndex);       
for(int z=0;z<qIndex;z++){
    printf("(%d, %d)  ", Q[z].x, Q[z].y);
}
printf("\n");
        //Cauculate Edges
        count = 0;
        for(int i = 1; i < qIndex; i++){
            for(int j = 0; j < i; j++){
                E[count].x = i;
                E[count].y = j;
                E[count].w = sqrt((pow((double)(Q[i].x - Q[j].x), 2) + pow((double)(Q[i].y - Q[j].y), 2)));
                count++;
            }
        }
//if(myid==0) printf("count=%d\n", count);       
        for(int i = 1; i < qIndex; i++){
            pick[i] = false;
        }
        pick[0] = true;     //pick start vertex
        local_count = count / numprocs;
        if(myid == numprocs - 1)
            rest = count % numprocs;  
        sum = 0;
//printf("id = %d : local_c=%d ; rest = %d\n",myid, local_count, rest);       
        for(int i=0; i < qIndex; i++){
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

    if(myid == 0){      
        printf("%.4f", final);
    }

    MPI_Finalize();
    return 0;
}