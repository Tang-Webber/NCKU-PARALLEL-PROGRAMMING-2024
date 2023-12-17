#include <omp.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>

struct Point {
    int x, y;
}P[40], Q[40];

struct Edge {
    int x, y;
    float w;
}E[400];

int cross(struct Point o, struct Point a, struct Point b) {
    return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}
int compare(const void* a, const void* b){
    return ((struct Point*)a)->x - ((struct Point*)b)->x;
}

void min(float* a, float* b){
    if(*a > *b)
        *a = *b;
}

float final;

int main( int argc, char *argv[])
{
    int n, num;
    int count = 0;
    float sum = 0;
    final = 99999;
    char input[50];

    //scan the input
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

    //sort
    qsort(P, n, sizeof(struct Point), compare); 

    struct Point *vertex = (struct Point*)malloc(n * sizeof(struct Point)); 
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
    //reorder P to vertex
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

    struct Edge temp;
    int index;
    bool pick[50];
    //consider inside point
    int inner = n - num;
    //#pragma omp parallel for private(E, Q, pick, qIndex, temp, sum)
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
        //Cauculate Edges
        count = 0;
        for(int i = 1; i < qIndex; i++){
            for(int j = 0; j < i; j++){
                E[count].x = i;
                E[count].y = j;
                E[count].w = sqrt((pow((float)(Q[i].x - Q[j].x), 2) + pow((float)(Q[i].y - Q[j].y), 2)));
                count++;
            }
        }
        for(int i = 1; i < qIndex; i++){
            pick[i] = false;
        }
        pick[0] = true;     //pick start vertex
        sum = 0;    
        for(int i = 0; i < qIndex - 1; i++){
            temp.w = 100;
            #pragma omp parallel for 
            for(int j = 0; j < count; j++){
                if( ((pick[E[j].x] && !pick[E[j].y]) || (!pick[E[j].x] && pick[E[j].y]))){
                    #pragma omp critical{
                        if(E[j].w < temp.w){
                            temp = E[j];                      
                        }
                    }
                }
            }
            
            pick[temp.x] = true;
            pick[temp.y] = true;
            sum += floor(temp.w * 10000) / 10000;  
        }
        //#pragma omp critical
        min(&final, &sum);
    }
   
    printf("%.4f", final);
    
    free(vertex);
    free(upper);
    free(lower);
    return 0;
}