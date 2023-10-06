#include "mpi.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

struct Point {
    int id, x, y;
} P[12000];

int cross(struct Point o, struct Point a, struct Point b) {
    if((a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x) > 0 )
        return 1;
    return -1;
}
int lineDist(struct Point o, struct Point a, struct Point b){
    return abs((a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x));
}
int compare(const void* a, const void* b){
    return ((struct Point*)a)->x - ((struct Point*)b)->x;
}

int main( int argc, char *argv[])
{
    int n, myid, numprocs;
    int ind[8];
    int min, max;
    
    char input[50];
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
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
        ind[0] = ind[4] = 0;
        for (int i = 0; i < n; i++) {
            fscanf(input_file, "%d %d", &P[i].x, &P[i].y);
            P[i].id = i;
            if (P[i].x > P[ind[4]].x)
                ind[4] = i;
            if (P[i].x < P[ind[0]].x)
                ind[0] = i;            
        }
        fclose(input_file);
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(ind, 8, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(P, n * sizeof(struct Point), MPI_BYTE, 0, MPI_COMM_WORLD);
printf("IND[0] = %d, IND[4] = %d\n", ind[0], ind[4]);
    int side = (myid >= numprocs / 2) ? -1 : 1;;
    struct Point SP[6][12000]; 
    int num[6] = {0};
    if(myid == 0){
        ind[2] = -1;
        max = 0;
        for (int i=0; i<n; i++)
        {
            int temp = lineDist(P[ind[0]], P[ind[4]], P[i]);
            if (cross(P[ind[0]], P[ind[4]], P[i]) == side)
            {
                SP[0][num[0]++] = P[i];
                if(temp > max){
                    ind[2] = i;
                    max = temp;                    
                }
            }
        }
        MPI_Send(&ind[2], 1, MPI_INT, 2, 0, MPI_COMM_WORLD); 
        MPI_Send(&num[0], 1, MPI_INT, 2, 0, MPI_COMM_WORLD);          
        MPI_Send(SP[0], num[0] * sizeof(struct Point), MPI_BYTE, 2, 0, MPI_COMM_WORLD);
    }
    if(myid == 4){
        ind[6] = -1;
        max = 0;
        for (int i=0; i<n; i++)
        {
            int temp = lineDist(P[ind[0]], P[ind[4]], P[i]);
            if (cross(P[ind[0]], P[ind[4]], P[i]) == side)
            {
                SP[1][num[1]++] = P[i];
                if(temp > max){
                    ind[6] = i;
                    max = temp;                    
                }
            }
        }
        MPI_Send(&ind[6], 1, MPI_INT, 6, 0, MPI_COMM_WORLD); 
        MPI_Send(&num[1], 1, MPI_INT, 6, 0, MPI_COMM_WORLD);          
        MPI_Send(SP[1], num[1] * sizeof(struct Point), MPI_BYTE, 6, 0, MPI_COMM_WORLD);
    }     
    if(myid == 0){
        ind[1] = -1;
        max = 0;
        for (int i=0; i<num[0]; i++)
        {
            int temp = lineDist(P[ind[0]], P[ind[2]], SP[0][i]);
            if (cross(P[ind[0]], P[ind[2]], SP[0][i]) == side)
            {
                SP[2][num[2]++] = SP[0][i];
                if(temp > max){
                    ind[1] = SP[0][i].id;
                    max = temp;                    
                }
            }
        }
        MPI_Send(&ind[1], 1, MPI_INT, 1, 0, MPI_COMM_WORLD); 
        MPI_Send(&num[2], 1, MPI_INT, 1, 0, MPI_COMM_WORLD);          
        MPI_Send(SP[2], num[2] * sizeof(struct Point), MPI_BYTE, 1, 0, MPI_COMM_WORLD);
printf("IND[1] = %d\n", ind[1]);
    }
    if(myid == 2){
        MPI_Recv(&ind[2], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&num[0], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(SP[0], num[0] * sizeof(struct Point), MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        ind[3] = -1;
        max = 0;
        for (int i=0; i<num[0]; i++)
        {
            int temp = lineDist(P[ind[2]], P[ind[4]], SP[0][i]);
            if (cross(P[ind[2]], P[ind[4]], SP[0][i]) == side)
            {
                SP[3][num[3]++] = SP[0][i];
                if(temp > max){
                    ind[1] = SP[0][i].id;
                    max = temp;                    
                }
            }
        }
        MPI_Send(&ind[3], 1, MPI_INT, 3, 0, MPI_COMM_WORLD); 
        MPI_Send(&num[3], 1, MPI_INT, 3, 0, MPI_COMM_WORLD);          
        MPI_Send(SP[3], num[3] * sizeof(struct Point), MPI_BYTE, 3, 0, MPI_COMM_WORLD);
printf("IND[2] = %d, IND[3] = %d\n", ind[2], ind[3]);        
    }
    if(myid == 4){
        ind[5] = -1;
        max = 0;
        for (int i=0; i<num[1]; i++)
        {
            int temp = lineDist(P[ind[6]], P[ind[4]], SP[1][i]);
            if (cross(P[ind[6]], P[ind[4]], SP[1][i]) == side)
            {
                SP[4][num[4]++] = SP[1][i];
                if(temp > max){
                    ind[5] = SP[1][i].id;
                    max = temp;                    
                }
            }
        }
        MPI_Send(&ind[5], 1, MPI_INT, 5, 0, MPI_COMM_WORLD); 
        MPI_Send(&num[4], 1, MPI_INT, 5, 0, MPI_COMM_WORLD);          
        MPI_Send(SP[4], num[4] * sizeof(struct Point), MPI_BYTE, 5, 0, MPI_COMM_WORLD);
printf("IND[5] = %d\n", ind[5]);
    }    
    if(myid == 6){
        MPI_Recv(&ind[6], 1, MPI_INT, 4, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&num[1], 1, MPI_INT, 4, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(SP[1], num[1] * sizeof(struct Point), MPI_BYTE, 4, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);        

        ind[7] = -1;
        max = 0;
        for (int i=0; i<num[1]; i++)
        {
            int temp = lineDist(P[ind[0]], P[ind[6]], SP[1][i]);
            if (cross(P[ind[0]], P[ind[6]], SP[1][i]) == side)
            {
                SP[5][num[5]++] = SP[1][i];
                if(temp > max){
                    ind[7] = SP[1][i].id;
                    max = temp;                    
                }
            }
        }
        MPI_Send(&ind[7], 1, MPI_INT, 7, 0, MPI_COMM_WORLD); 
        MPI_Send(&num[5], 1, MPI_INT, 7, 0, MPI_COMM_WORLD);          
        MPI_Send(SP[5], num[5] * sizeof(struct Point), MPI_BYTE, 7, 0, MPI_COMM_WORLD);
printf("IND[6] = %d, IND[7] = %d\n", ind[6], ind[7]);
    }
    int use = myid / 2 + 2;
    if(myid % 2 == 1){
        MPI_Recv(&ind[myid], 1, MPI_INT, myid - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&num[use], 1, MPI_INT, myid - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(SP[use], num[use] * sizeof(struct Point), MPI_BYTE, myid - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);  
    }
    struct Point* q = (struct Point*)malloc(numprocs * sizeof(int));
    int c = 1;
    q[0] = P[ind[myid]];
    for (int i = 0; i < num[use]; i++){
        if(cross(P[ind[myid]], P[ind[(myid + 1) % 8]], SP[use][i]) == 1)
            q[c++] = SP[use][i];
    }       
    struct Point* local_ch = (struct Point*)malloc(c * sizeof(struct Point));
    //Andrew's monotone chain
    int count = 0;
    for (int i = 0; i < c; i++){
        while (count >= 2 && cross(local_ch[count-2], local_ch[count-1], q[i]) == side) count--;
        local_ch[count++] = q[i];
    }   
    if (myid != 0) {
        MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(local_ch, count * sizeof(struct Point), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
    }     
    else{
        struct Point **gathered = (struct Point**)malloc(numprocs * sizeof(struct Point*));
        for(int i = 0; i < numprocs ;i++){
            gathered[i] = (struct Point*)malloc(n * sizeof(struct Point));
        }
        int *counts = (int*)malloc(numprocs * sizeof(int));
        for(int i = 0;i < count; i++){
            printf("%d ", local_ch[i].id + 1);
        }                
        for (int i = 1; i < numprocs; i++) {
            MPI_Recv(&counts[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(gathered[i], counts[i] * sizeof(struct Point), MPI_BYTE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for(int j = 0;j < counts[i]; j++)
                printf("%d ", gathered[i][j].id + 1); 
        }
        for (int i = 1; i < numprocs; i++) {
            free(gathered[i]);
        }        
        free(gathered);
        free(counts);
    }    
    free(local_ch);
    free(q);
    MPI_Finalize();
    return 0;
}