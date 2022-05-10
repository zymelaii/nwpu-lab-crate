/**
 * @brief Merge ordered arrays
 * @author zymelaii
 * @date 2022-03-20
 */

#include <stdio.h>

#define s(t) scanf("%d", (t))
#define p(t) printf("%d\n", (t))

int main(int argc, char *argv[])
{
    int *a, *b, *c; //@ arrays
    int n, i = 0;   //@ params for array a
    int m, j = 0;   //@ params for array b
    int t = 0;      //@ current size of array c

    for (s(&n), a = new int[n]; i < n; s(a + i++)) {
        //!input array a
    }
    for (s(&m), b = new int[m]; j < m; s(b + j++)) {
        //!input array b
    }

    c = new int[m + n];

    //!handle public range via comparison
    i = j = 0;
    while (i < n && j < m) {
        if (a[i] < b[j]) {
            c[t++] = a[i++];
        } else {
            c[t++] = b[j++];
        }
    }

    //!append array a
    while (i < n) {
        c[t++] = a[i++];
    }

    //!append array b
    while (j < m) {
        c[t++] = b[j++];
    }

    for (i = 0; i < t; p(c[i++])) {
        //!output merged array
    }

    delete[] a;
    delete[] b;
    delete[] c;

    return 0;
}