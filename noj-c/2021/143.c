#include <stdio.h>
#include <stdlib.h>

int main() {
    int a[20], s = 1, n, m, i = 1, j = 1, b, e, f;
    scanf("%d", &n);
    m = n;
    for (n = 0; n < m; n++) scanf("%d", &a[n]);
    if (m == 1) {
        if (a[0] < 0)
            printf("-1");
        else
            printf("%d", a[0]);
        return 0;
    };
    if (m == 2) {
        if ((a[0] < 0) && (a[1] < 0))
            printf("%d", (a[0] * a[1]));
        else if ((a[0] > 0) && a[1] > 0)
            printf("%d", (a[0] * a[1]));
        else if (a[0] > a[1])
            printf("%d", a[0]);
        else
            printf("%d", a[1]);
        return 0;
    };
    b = 0;
    for (n = 0; n < m; n++)
        if (a[n] < 0) b = b + 1;
    if (b % 2 == 0) {
        for (n = 0; n < m; n++) s = s * a[n];
        printf("%d", s);
        return 0;
    } else {
        for (n = 0; n < m; n++)
            if (a[n] < 0) {
                e = n;
                break;
            };
        for (n = m - 1; n >= 0; n--)
            if (a[n] < 0) {
                f = n;
                break;
            };
        for (n = e + 1; n < m; n++) i = i * a[n];
        for (n = f - 1; n >= 0; n--) j = j * a[n];
        if (i > j)
            s = i;
        else
            s = j;
        printf("%d", s);
    };
    return 0;
}
