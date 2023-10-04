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
    return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}

int compare(const void* a, const void* b)
{
    if (((struct Point*)a)->x != ((struct Point*)b)->x)
        return ((struct Point*)a)->x - ((struct Point*)b)->x;
    else
        return ((struct Point*)a)->y - ((struct Point*)b)->y;
}

int main( int argc, char *argv[])
{
    int n, myid, numprocs;
    int left, right;
    int u, d;
    char input[50];
    int rest = 0;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);

    MPI_Datatype PointType;
    MPI_Aint offsets[3];
    MPI_Datatype types[3] = {MPI_INT, MPI_INT, MPI_INT};
    offsets[0] = offsetof(struct Point, id);
    offsets[1] = offsetof(struct Point, x);
    offsets[2] = offsetof(struct Point, y);
    int block_lengths[3] = {1, 1, 1};    
    MPI_Type_create_struct(3, block_lengths, offsets, types, &PointType);
    MPI_Type_commit(&PointType);

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
            P[i].id = i + 1;
        }
        fclose(input_file);
        qsort(P, n, sizeof(struct Point), compare);  
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int *recv_counts = (int*)malloc(numprocs * sizeof(int));
    int *displacements = (int*)malloc(numprocs * sizeof(int));
    int base_count = n / numprocs;
    rest = n % numprocs;
    for (int i = 0; i < numprocs; i++) {
        recv_counts[i] = base_count;
        if (i == numprocs - 1) {
            recv_counts[i] += rest;
        }
        displacements[i] = i * base_count;
    }

    int local_count = recv_counts[myid];
    struct Point* local_P = (struct Point*)malloc(local_count * sizeof(struct Point));    
    //Data Boardcast
    struct Point* local_upper_ch = (struct Point*)malloc(local_count * sizeof(struct Point));
    struct Point* local_lower_ch = (struct Point*)malloc(local_count * sizeof(struct Point));
    MPI_Bcast(P, n, PointType, 0, MPI_COMM_WORLD);
    //MPI_Scatter(P, local_count, PointType, local_P, local_count, PointType, 0, MPI_COMM_WORLD);
    MPI_Scatterv(P, recv_counts, displacements, PointType, local_P, recv_counts[myid], PointType, 0, MPI_COMM_WORLD);

    //Local Calculation
    //Set Variable
    int up = 0;
    int down = 0;
    int* ups = NULL;
    int* downs = NULL;
    struct Point *final_up= NULL;
    struct Point *final_down= NULL;
    struct Point **gathered_up = NULL;
    struct Point **gathered_down = NULL;
    //malloc
    ups = (int*)malloc(numprocs * sizeof(int));
    downs = (int*)malloc(numprocs * sizeof(int));
    final_up = (struct Point*)malloc(n * sizeof(struct Point));
    final_down = (struct Point*)malloc(n * sizeof(struct Point));    
    gathered_up = (struct Point**)malloc(numprocs * sizeof(struct Point*));
    gathered_down = (struct Point**)malloc(numprocs * sizeof(struct Point*));  
    for(int i = 0; i < numprocs ;i++){
        gathered_up[i] = (struct Point*)malloc(base_count * sizeof(struct Point));
        gathered_down[i] = (struct Point*)malloc(base_count * sizeof(struct Point));
    }
    //Andrew's Monotone Chain
    if(myid == 0){       
        for (int i = 0; i < local_count; i++){
            while (down >= 2 && cross(final_down[down-2], final_down[down-1], local_P[i]) <= 0) down--;
            final_down[down++] = local_P[i];
        }
        for (int i = 0; i < local_count; i++){
            while (up >= 2 && cross(final_up[up-2], final_up[up-1], local_P[i]) >= 0) up--;
            final_up[up++] = local_P[i];
        }    
    }
    else{
        for (int i = 0; i < local_count; i++){
            while (down >= 2 && cross(local_lower_ch[down-2], local_lower_ch[down-1], local_P[i]) <= 0) down--;
            local_lower_ch[down++] = local_P[i];
        }
        for (int i = 0; i < local_count; i++){
            while (up >= 2 && cross(local_upper_ch[up-2], local_upper_ch[up-1], local_P[i]) >= 0) up--;
            local_upper_ch[up++] = local_P[i];
        }        
    }

    MPI_Gather(&up, 1, MPI_INT, ups, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(&down, 1, MPI_INT, downs, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (myid != 0) {
        MPI_Send(local_upper_ch, up * sizeof(struct Point), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
        MPI_Send(local_lower_ch, down * sizeof(struct Point), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
    } 
    else {
        for (int i = 1; i < numprocs; i++) {
            MPI_Recv(gathered_up[i], ups[i] * sizeof(struct Point), MPI_BYTE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(gathered_down[i], downs[i] * sizeof(struct Point), MPI_BYTE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    if(myid == 0){
        //MPI_Send(ups, numprocs + rest, MPI_INT, 1, 0, MPI_COMM_WORLD);  
        //MPI_Send(downs, numprocs + rest, MPI_INT, 1, 0, MPI_COMM_WORLD); 
        //MPI_Send(gathered_up, (numprocs + rest) * local_count * sizeof(struct Point), MPI_BYTE, 1, 0, MPI_COMM_WORLD); 
        //MPI_Send(gathered_down, (numprocs + rest) * local_count * sizeof(struct Point), MPI_BYTE, 1, 0, MPI_COMM_WORLD); 
        //MPI_Send(final_up, up * sizeof(struct Point), MPI_BYTE, 1, 0, MPI_COMM_WORLD); 
        //MPI_Send(final_down, down * sizeof(struct Point), MPI_BYTE, 1, 0, MPI_COMM_WORLD); 
    }
    if(myid == 1){
        //MPI_Recv(ups, numprocs + rest, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //MPI_Recv(downs, numprocs + rest, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //MPI_Recv(gathered_up, (numprocs + rest) * local_count * sizeof(struct Point), MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //MPI_Recv(gathered_down, (numprocs + rest) * local_count * sizeof(struct Point), MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //MPI_Recv(final_up, up * sizeof(struct Point), MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //MPI_Recv(final_down, down * sizeof(struct Point), MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);        
    }

    //Combine small convex hulls
    if (myid == 0){
        //Iteratvely add id = i to final
        //Lower
        left = downs[0] - 1;
        right = 0;
        for(int i = 1; i < numprocs; i++){
            //Gathered_[i] leftmost and final_[i]rightmost
            while(1){
                if(left == 0 || right == downs[i] - 1){
                    break;
                }                
                if(cross(final_down[left - 1], gathered_down[i][right], final_down[left]) <= 0 && cross(final_down[left], gathered_down[i][right + 1], gathered_down[i][right]) <= 0){
                    break;
                }                
                if(cross(gathered_down[i][right], final_down[left - 1], final_down[left]) < 0)
                    left--;
                if(cross(final_down[left], gathered_down[i][right + 1], gathered_down[i][right]) > 0)
                    right++;
            }
            //Combine the results to final_ch
            for(int j = 0; j < downs[i] - right; j++){
                final_down[left + j + 1] = gathered_down[i][j + right];
            }
            left += downs[i] - right;
            right = 0;
        }
        d = left;
    }
    if(myid == 0){
        //Upper
        left = ups[0] - 1;
        right = 0;
        for(int i = 1; i < numprocs; i++){
            //Gathered_[i] leftmost and final_[i]rightmost
            while(1){
                if(cross(gathered_up[i][right], final_up[left - 1], final_up[left]) > 0)
                    left--;
                if(cross(final_up[left], gathered_up[i][right + 1], gathered_up[i][right]) < 0)
                    right++;
                if((cross(final_up[left - 1], gathered_up[i][right], final_up[left]) >= 0 && cross(final_up[left], gathered_up[i][right + 1], gathered_up[i][right]) >= 0) || left == 0 || right == ups[i]){
                    break;
                }
            }
            //Combine the results to final_ch
            for(int j = 0; j < ups[i] - right; j++){
                final_up[left + j + 1] = gathered_up[i][j + right];
            }
            left += ups[i] - right;
            right = 0;
        }
        u = left;
        //MPI_Send(final_up, u * sizeof(struct Point), MPI_BYTE, 0, 0, MPI_COMM_WORLD); 
        //MPI_Send(&u, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    if(myid == 0){
        //MPI_Recv(final_up, u * sizeof(struct Point), MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //MPI_Recv(&u, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //output
        for(int i = 0;i < u; i++){
            printf("%d ", final_up[i].id);
        }
        for(int i = d;i > 0; i--){
            printf("%d ", final_down[i].id);
        }
    }
    //Free memory
    for (int i = 0; i < numprocs; i++) {
        free(gathered_up[i]);
        free(gathered_down[i]);
    }       
    free(final_up);
    free(final_down);
    free(gathered_up);
    free(gathered_down);
    free(ups);
    free(downs);
    free(local_P);
    free(local_upper_ch);
    free(local_lower_ch);
    MPI_Type_free(&PointType);
    MPI_Finalize();
    return 0;
}