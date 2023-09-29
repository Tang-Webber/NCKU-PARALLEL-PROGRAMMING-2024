#include "mpi.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

bool f(unsigned int*, unsigned int, unsigned int, int);
bool f(unsigned int *tests, unsigned int t, unsigned int ltime, int m){
    unsigned int set = 0;
    for(int i = 0; i < m; i++){
        if ((t & (1u << i)) != 0) 
            set = set | tests[i];
    }
    if(set == ltime)
        return true;
    else
        return false;
}

int main( int argc, char *argv[])
{
    int n, m, myid, numprocs;
    int i = 0;
    int count = 0;
    int sum;
    unsigned int ltime = 1;
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
printf("open%s!\n", input);
        if(input_file == NULL){
            printf("could not open file %s\n", input);
            fclose(input_file);
            return 1;
        }
        fscanf(input_file, "%d %d", &n, &m);
printf("%d %d\n", n, m);
        for (int j = 0; j < n; j++) {
            ltime *= 2;
        }
        ltime--;
        for (int j = 0; j < m; j++) {
            int num;
            fscanf(input_file, "%d", &num);
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
    MPI_Bcast(&ltime, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&m, 1, MPI_INT, 0, MPI_COMM_WORLD);

    for (i = myid + 1; i <= n; i += numprocs){
        if(f(tests, i, ltime, m) == true) count++;
    }
    MPI_Barrier(MPI_COMM_WORLD);
printf("test1\n");    
    MPI_Reduce(&count, &sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);   
printf("test2\n");    
    if (myid == 0){
        strcpy(output, input);
        char *dot = strrchr(output, '.');
        if (dot != NULL) {
            *dot = '\0';
        }
printf("out:%s\n", output);
        strcat(output, ".out");
        FILE *output_file = fopen(output, "w");
printf("test3\n");
        fprintf(output_file, "%d\n", sum); 
        fclose(output_file);
    } 
    MPI_Finalize();
    return 0;
}