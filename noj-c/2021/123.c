#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define PI 3.1415926

int main() {
    double r, h, l, s, sq, vq, vz;
    scanf("%lf%lf", &r, &h);
    l  = 2.0 * PI * r;
    s  = PI * r * r;
    sq = 4.0 * PI * r * r;
    vq = (4.0 * PI * r * r * r) / 3.0;
    vz = PI * r * r * h;
    printf("%.2lf\n%.2lf\n%.2lf\n%.2lf\n%.2lf\n", l, s, sq, vq, vz);
    return 0;
}
