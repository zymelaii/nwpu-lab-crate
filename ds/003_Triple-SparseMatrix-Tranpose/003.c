/**
 * @brief Sparse matrix transpose based on triple
 * @author zymelaii
 * @date 2022-03-19
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct T { int i, j, x; };

int main(int argc, char const* argv[])
{
	struct T
		A[1024] = { 0 }, //@ origin matrix
		B[1024] = { 0 }; //@ transposed matrix
	int c = 0, n, m;

	scanf("%d%d", &n, &m);
	while (1) {
		scanf("%d%d%d", &A[c].i, &A[c].j, &A[c].x);
		if (A[c].x == 0) break;
		++c;
		//@ input matrix
	}

	int *P = (int*)calloc(m + 1, sizeof(int));
	for (int i = 0; i < c; ++i) {
		++P[A[i].j + 1];
	} //@ compute number of each column
	for (int i = 1; i <= m; ++i) {
		P[i] += P[i - 1];
	} //@ compute startpos of each column in transposed matrix
	for (int i = 0; i < c; ++i) {
		int index = P[A[i].j];
		B[index].i = A[i].j;
		B[index].j = A[i].i;
		B[index].x = A[i].x;
		++P[A[i].j];
	} //@ perform transpose

	for (int i = 0; i < c; ++i) {
		printf("%d %d %d\n", B[i].i, B[i].j, B[i].x);
	} //@ output transposed matrix

	free(P);
	return 0;
}
