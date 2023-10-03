#include "mpi.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

struct Point {
    int id, x, y;
} P[12000];

long long int cross(struct Point o, struct Point a, struct Point b) {
    return (long long int)(a.x - o.x) * (b.y - o.y) - (long long int)(a.y - o.y) * (b.x - o.x);
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
    int left_up = 0;
    int left_down = 0;
    int left_count;

    char input[50];
    int rest = 0;

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
        for (int i = 0; i < n; i++) {
            fscanf(input_file, "%d %d", &P[i].x, &P[i].y);
            P[i].id = i + 1;
        }
        fclose(input_file);
        qsort(P, n, sizeof(struct Point), compare);
    }
    //Data Boardcast
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int block_lengths[3] = {1, 1, 1};
    int local_count = n / numprocs; 
    struct Point* local_P = (struct Point*)malloc(local_count * sizeof(struct Point));  
    struct Point* local_upper_ch = (struct Point*)malloc(local_count * sizeof(struct Point));  
    struct Point* local_lower_ch = (struct Point*)malloc(local_count * sizeof(struct Point));   
    MPI_Datatype PointType;
    MPI_Aint offsets[3];
    MPI_Datatype types[3] = {MPI_INT, MPI_INT, MPI_INT};
    offsets[0] = offsetof(struct Point, id);
    offsets[1] = offsetof(struct Point, x);
    offsets[2] = offsetof(struct Point, y);
    MPI_Type_create_struct(3, block_lengths, offsets, types, &PointType);
    MPI_Type_commit(&PointType);
    MPI_Bcast(P, n, PointType, 0, MPI_COMM_WORLD);
    MPI_Scatter(P, local_count, PointType, local_P, local_count, PointType, 0, MPI_COMM_WORLD);
    //Local Calculation
    //Andrew's Monotone Chain
    int up = 0;
    int down = 0;

	for (int i = 0; i < local_count; i++){
		while (down >= 2 && cross(local_lower_ch[down-2], local_lower_ch[down-1], local_P[i]) <= 0) down--;
		local_lower_ch[down++] = local_P[i];
	}
	for (int i = 0; i < local_count; i++){
		while (up >= 2 && cross(local_upper_ch[up-2], local_upper_ch[up-1], local_P[i]) >= 0) up--;
		local_upper_ch[up++] = local_P[i];
	}
    if(myid == 0 && local_count * numprocs != n){
        rest = 1; 
        left_count = n - local_count * numprocs;     
    }
    int* ups = NULL;       
    int* downs = NULL;
    struct Point *final_up= NULL;
    struct Point *final_down= NULL; 
    struct Point **gathered_up = NULL;
    struct Point **gathered_down = NULL;
    if (myid == 0){
        ups = (int*)malloc((numprocs + rest) * sizeof(int));
        downs = (int*)malloc((numprocs + rest) * sizeof(int));
    }  
    MPI_Gather(&up, 1, MPI_INT, ups, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(&down, 1, MPI_INT, downs, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (myid == 0){
        gathered_up = (struct Point**)malloc((numprocs + rest) * sizeof(struct Point*));
        gathered_down = (struct Point**)malloc((numprocs + rest) * sizeof(struct Point*));  
        final_up = (struct Point*)malloc(n * sizeof(struct Point));
        final_down = (struct Point*)malloc(n * sizeof(struct Point));        
        for(int i = 0; i < numprocs;i++){
            gathered_up[i] = (struct Point*)malloc(ups[i] * sizeof(struct Point));
            gathered_down[i] = (struct Point*)malloc(downs[i] * sizeof(struct Point));
        }
    }   

    if (myid != 0) {
        MPI_Send(local_upper_ch, up * sizeof(struct Point), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
        MPI_Send(local_lower_ch, down * sizeof(struct Point), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
    } else {
        for (int i = 1; i < numprocs; i++) {
            MPI_Recv(gathered_up[i], ups[i] * sizeof(struct Point), MPI_BYTE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(gathered_down[i], downs[i] * sizeof(struct Point), MPI_BYTE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    if(myid == 0 && rest != 0){
        gathered_up[numprocs] = (struct Point*)malloc(local_count * sizeof(struct Point));
        gathered_down[numprocs] = (struct Point*)malloc(local_count * sizeof(struct Point));
        for (int i = local_count * numprocs; i < n; i++){
		    while (left_down >= 2 && cross(gathered_down[numprocs][left_down-2], gathered_down[numprocs][left_down-1], P[i]) <= 0) left_down--;
		    gathered_down[numprocs][left_down++] = P[i];
	    }
	    for (int i = local_count * numprocs; i < n; i++){
		    while (left_up >= 2 && cross(gathered_up[numprocs][left_up-2], gathered_up[numprocs][left_up-1], P[i]) >= 0) left_up--;
		    gathered_up[numprocs][left_up++] = P[i];
	    }       
        ups[numprocs] = left_up;
        downs[numprocs] = left_down;   
    }

    //Combine small convex hulls  
    if (myid == 0){
        //Step 1: copy id = 0 to final
        for(int i = 0; i < downs[0]; i++){
            final_down[i] = local_lower_ch[i];
        }
        for(int i = 0; i < ups[0]; i++){
            final_up[i] = local_upper_ch[i];
        }
        //Step 2: iteratvely add id = i to final
        //Lower
        left = downs[0] - 1;
        right = 0;
        for(int i = 1; i < numprocs + rest; i++){
            //Gathered_[i] leftmost and final_[i]rightmost
            while(1){
                if(cross(gathered_down[i][right], final_down[left - 1], final_down[left]) < 0)
                    left--;
                if(cross(final_down[left], gathered_down[i][right + 1], gathered_down[i][right]) > 0)
                    right++;                  
                if((cross(final_down[left - 1], gathered_down[i][right], final_down[left]) <= 0 && cross(final_down[left], gathered_down[i][right + 1], gathered_down[i][right]) <= 0) || left == 0 || right == downs[i]){
                    break;
                }
            }
            //Combine the results to final_ch
            for(int j = 0; j < downs[i] - right; j++){
                final_down[left + j + 1] = gathered_down[i][j + right];
            }
            left += downs[i] - right; 
printf("%d %d %d\n",i , left, right);
            right = 0;
        }
        d = left;
        //Upper
        left = ups[0] - 1;
        right = 0;
        for(int i = 1; i < numprocs + rest; i++){
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
        //output
        for(int i = 0;i < u; i++){
            printf("%d ", final_up[i].id);
        }
        //if(final_up[u].id == final_down[d].id){
        //    printf("%d ", final_down[d--].id);
        //}          
        for(int i = d;i > 0; i--){
            printf("%d ", final_down[i].id);
        }   
printf("\n%d\n", d);
        //Free memory
        for (int i = 0; i < numprocs + rest; i++) {
            free(gathered_up[i]);
            free(gathered_down[i]);
        }
        free(final_up);
        free(final_down);
        free(gathered_up);
        free(gathered_down);
        free(ups);
        free(downs);        
    }
    MPI_Barrier(MPI_COMM_WORLD);  
    free(local_P);
    free(local_upper_ch);
    free(local_lower_ch);
    MPI_Type_free(&PointType);
    MPI_Finalize();
    return 0;
}