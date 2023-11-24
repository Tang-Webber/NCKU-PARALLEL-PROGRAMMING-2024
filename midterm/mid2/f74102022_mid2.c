#include <stdio.h>
#include <math.h>

int main(int argc, char *argv[]){
    double serial;
    double parallel;
    double answer;
    serial = 1250;
    parallel = 650;
    answer = serial / parallel;
    printf("%.2f", answer);
    return 0;
}