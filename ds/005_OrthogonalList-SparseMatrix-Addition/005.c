/**
 * @brief Sparse matrix addition based on orthogonal list
 * @author zymelaii
 * @date 2022-03-19
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>

struct T {
	int i, j, x;
	struct T *down, *right;
};

struct M {
	int row, col;
	struct T *chead, *rhead;
};

void erase(struct M *mat, int i, int j) {
	struct T *p = &mat->chead[j], *e;

	//@ unlink e from column j
	while (p->down->i != i) {
		p = p->down;
	}
	e = p->down;
	p->down = p->down->down;
	
	//@ unlink e from row i
	p = &mat->rhead[i];
	while (p->right->j != j) {
		p = p->right;
	}
	p->right = e->right;

	//@ release e
	free(e);
}

void insert(struct M *mat, int i, int j, int x) {
	if (x == 0) return;

	//@ create node
	struct T *e = (struct T*)malloc(sizeof(struct T));
	*e = (struct T){ i, j, x, 0L, 0L };

	//@ find suitable position for e insertion
	//@ complete row linkage
	struct T *p = &mat->rhead[i];
	if (!p->right) {
		p->right = e;
	} else {
		while (!!p->right && p->right->j < j) {
			p = p->right;
		}
		if (!p->right) {
			p->right = e;
		} else if (p->right->j == j) {
			//@ find element in the same position
			p->right->x += e->x;
			if (p->right->x == 0) {
				//@ erase M(i,j) if addition equals 0
				erase(mat, i, j);
			}
			free(e);
			e = 0L;
		} else {
			e->right = p->right;
			p->right = e;
		}
	}

	//@ complete column linkage
	p = &mat->chead[j];
	if (!p->down) {
		p->down = e;
	} else {
		while (!!p->down && p->down->i < i) {
			p = p->down;
		}
		if (!p->down) {
			p->down = e;
		} else if (p->down->i > i && !!e) {
			e->down = p->down;
			p->down = e;
		}
	}
}

void destroy(struct M *mat) {
	for (int i = 0; i < mat->row; ++i) {
		struct T *p = mat->rhead[i].right;
		while (!!p) {
			struct T *q = p->right;
			free(p);
			p = q;
		}
	}
	free(mat->chead);
	free(mat->rhead);
}

int main(int argc, char const *argv[])
{
	struct M mat = { 0 };
	int t1, t2, t;

	scanf("%d%d%d%d", &mat.row, &mat.col, &t1, &t2);

	t = t1 + t2;
	mat.chead = (struct T*)malloc(sizeof(struct T) * mat.col);
	mat.rhead = (struct T*)malloc(sizeof(struct T) * mat.row);
	memset(mat.chead, 0, sizeof(struct T) * mat.col);
	memset(mat.rhead, 0, sizeof(struct T) * mat.row);

	while (t-- > 0) {
		int i, j, x;
		scanf("%d%d%d", &i, &j, &x);
		--i, --j;
		insert(&mat, i, j, x);
	} //@ read and perform addition in one shot

	//@ output orthogonal list sparse matrix
	for (int i = 0; i < mat.row; ++i) {
		struct T *p = mat.rhead[i].right;
		while (!!p) {
			printf("%d %d %d\n", p->i + 1, p->j + 1, p->x);
			p = p->right;
		}
	}

	//@ release memory
	destroy(&mat);
	return 0;
}