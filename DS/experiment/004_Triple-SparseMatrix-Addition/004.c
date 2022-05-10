/**
 * @brief Sparse matrix addition based on triple
 * @author zymelaii
 * @date 2022-03-19
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct T { int i, j, x; };

int main(int argc, char const *argv[])
{
	struct T A[100] = { 0 }, B[100] = { 0 }, C[100] = { 0 };
	int u = 0, v = 0, t1, t2, t = 0;

	scanf("%*d%*d%d%d", &t1, &t2); //@ input sparse matrix
	for (int i = 0; i < t1; scanf("%d%d%d", &A[i].i, &A[i].j, &A[i].x), ++i);
	for (int i = 0; i < t2; scanf("%d%d%d", &B[i].i, &B[i].j, &B[i].x), ++i);

	while (u < t1 && v < t2) {
		//@ insert elements orderly
		if (A[u].i < B[v].i) {
			C[t++] = A[u++];
		} else if (A[u].i > B[v].i) {
			C[t++] = B[v++];
		} else {
			if (A[u].j < B[v].j) {
				C[t++] = A[u++];
			} else if (A[u].j > B[v].j) {
				C[t++] = B[v++];
			} else {
				C[t] = (struct T){ A[u].i, A[u].j, A[u].x + B[v].x };
				if (C[t].x != 0) {
					++t;
				} //@ erase zero
				++u, ++v;
			}
		}
	}

	//@ insert the rest of elements
	while (u < t1) {
		C[t++] = A[u++];
	}

	while (v < t2) {
		C[t++] = B[v++];
	}

	//@ output sparse matrix
	for (int i = 0; i < t; ++i) {
		printf("%d %d %d\n", C[i].i, C[i].j, C[i].x);
	}

	return 0;
}