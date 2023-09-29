#include "mpi.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
/*
bool f(unsigned int*, unsigned int, unsigned int, int);
bool f(unsigned int* tests, unsigned int t, unsigned int pow2n, int m){
    unsigned int set = 0;
    for(int i = 0; i < m; i++){
        if ((t & (1u << i)) != 0) 
            set = set | tests[i];
    }
printf("set = %u, %u, %u\n", set, pow2n, t);
    if(set == pow2n)
        return true;
    else
        return false;
}
*/
int main( int argc, char *argv[])
{
    int n, m, myid, numprocs;
    int count = 0;
    int cost;
    int sum;
    unsigned int pow2n = 1;             //pow2n : process represent in binary
    unsigned int pow2m = 1;             //pow2m : num of premutations
    unsigned int set = 0;
    unsigned int t;
    unsigned int tests[32] = {0};
    double startwtime = 0.0, endwtime;
    int  namelen;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    char input[50];
    char output[50];

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    //MPI_Get_processor_name(processor_name,&namelen);
    if (myid == 0) {
        //startwtime = MPI_Wtime(); 
        scanf("%s", input);
        FILE *input_file = fopen(input, "r");
//printf("open%s!\n", input);
        if(input_file == NULL){
            printf("could not open file %s\n", input);
            fclose(input_file);
            return 1;
        }
        fscanf(input_file, "%d %d", &n, &m);
        for (int j = 0; j < n; j++) {
            pow2n *= 2;
        }
        pow2n--;
        for (int j = 0; j < m; j++) {
            pow2m *= 2;
        }
        for (int j = 0; j < m; j++) {
            int num;
            fscanf(input_file, "%d %d", &num, &cost);
            tests[j] = 0;
            for (int k = 0; k < num; k++) {
                int part;
                fscanf(input_file, "%d", &part);
                tests[j] |= (1 << (part - 1));
            }
            MPI_Bcast(&tests[j], 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
        }
        fclose(input_file);
    }
    MPI_Barrier(MPI_COMM_WORLD); 
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);   
    MPI_Bcast(&pow2n, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
    MPI_Bcast(&pow2m, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
printf("test1, id : %d ,total:%d\n, paremeter: %d %d %u %u", myid, numprocs, n,m,pow2n,pow2m);   
    for (unsigned int i = myid; i < pow2m; i += numprocs){
        t = i;
        set = 0;
        for(int k = 0; k < m; k++){
            if ((t & (1u << k)) != 0) 
                set = set | tests[k];
        }
        if(set == pow2n)
            count++;
printf("id:%d i=%d c=%d\n", myid, i, count);
printf("set=%u, ans=%u \n", set, pow2n);              
        //if(i <= pow2m && f(tests, i, pow2n, m)){count++};
    }
printf("test2 , id : %d\n", myid);   
    MPI_Barrier(MPI_COMM_WORLD);  
printf("test3\n, sum:%d", sum);   
    MPI_Reduce(&count, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);   
printf("test4\n, sum:%d", sum);    
    if (myid == 0){
        strcpy(output, input);
        char *dot = strrchr(output, '.');
        if (dot != NULL) {
            *dot = '\0';
        }
        strcat(output, ".out");
printf("out:%s\n", output);
        FILE *output_file = fopen(output, "w");
printf("%d\n", sum);
        fprintf(output_file, "%d\n", sum); 
        fclose(output_file);
    } 
    MPI_Finalize();
    return 0;
}