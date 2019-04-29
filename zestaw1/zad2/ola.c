#include <stdio.h>
#include <time.h>


int main(){
    struct timespec a;
    clock_gettime(CLOCK_REALTIME, &a);
    printf("%ld", a.tv_sec);
}