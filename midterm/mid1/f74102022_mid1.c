#include <stdio.h>
#include <math.h>

int main(int argc, char *argv[]){
    double serial;
    double answer;
    serial = 0.13;      // 100% - 87%
    answer = 1 / serial;
    printf("%.2f", answer);
    return 0;
}