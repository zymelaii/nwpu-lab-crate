#include <stdio.h>
#include <stdlib.h>

int BinarySearch(int A[], int n, int find) {
    int low, upper, mid;
    low = 0, upper = n - 1;
    while (low <= upper) {
        mid = low + (upper - low) / 2;
        if (A[mid] < find)
            low = mid + 1;
        else if (A[mid] > find)
            upper = mid - 1;
        else
            return mid;
    }
    return -1;
}

int main() {
    int A[100000], n, m, i, j;
    scanf("%d", &n);
    for (i = 0; i < n; i++) { scanf("%d", &A[i]); }
    scanf("%d", &j);
    i = BinarySearch(A, n, j);
    if (i >= 0)
        printf("%d", i);
    else
        printf("null");
    return 0;
}
