#include <stdio.h>
#include <stdlib.h>

struct tag {
    long int no;
    char     name[21];
    int      age;
    double   yy, sx, yw, zz, wl, hx, xx, s, jun;
};

int main() {
    struct tag a[10], t, q;
    int        i, j;
    for (i = 0; i < 10; i++)
        scanf(
            "%ld%s%d%lf%lf%lf%lf%lf%lf%lf",
            &a[i].no,
            a[i].name,
            &a[i].age,
            &a[i].yy,
            &a[i].sx,
            &a[i].yw,
            &a[i].zz,
            &a[i].wl,
            &a[i].hx,
            &a[i].xx);
    for (i = 0; i < 10; i++) {
        a[i].s =
            a[i].yy + a[i].sx + a[i].yw + a[i].zz + a[i].wl + a[i].hx + a[i].xx;
        a[i].jun = a[i].s / 7;
    }
    t.s = a[0].s;
    for (i = 1; i < 10; i++)
        if (a[i].s > t.s) {
            t.s = a[i].s;
            j   = i;
        }
    q = a[j];
    printf(
        "%ld %s %d %.2lf %.2lf %.2lf %.2lf %.2lf %.2lf %.2lf",
        q.no,
        q.name,
        q.age,
        q.yy,
        q.sx,
        q.yw,
        q.zz,
        q.wl,
        q.hx,
        q.xx);
    return 0;
}
