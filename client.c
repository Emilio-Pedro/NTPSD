#include <stdint.h>
#include <stdio.h>
#include <time.h>

struct ntp
{
    double t1, t2, t3;
};

double time_double()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9; // conversão para double
};

int main(void)
{
    double t = time_double();
    printf("tempo atual: %f segundos\n", t);
    return 0;
}