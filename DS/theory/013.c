/********************************
 *  @author: ZYmelaii 项乔栋
 *  @brief: 数据结构（双语）NOJ T013 - 以十字链表为存储结构实现矩阵相加（严5.27）
 *  @date: 2022/1/12
 *******************************/

/********************************
 * Description
 * 以十字链表为存储结构，编写程序，将稀疏桓阵B加到稀疏垣阵A上。
 * 
 * Input
 * 第一行输入四个正整数，分别为稀疏矩阵A和稀疏矩阵B的行数m、列数n、稀疏矩阵A的非零元素个数t1和稀疏矩阵B的非零元素个数t2。接下来的t1+t2行三元组表示，其中第一个元素表示非零元素所在的行值，第二个元素表示非零元素所在的列值，第三个元素表示非零元素的值。
 * 
 * Output
 * 输出相加后的矩阵三元组。
 * 
 * Sample Input
 * 3 4 3 2
 * 1 1 1
 * 1 3 1
 * 2 2 2
 * 1 2 1
 * 2 2 3
 * 
 * Sample Output
 * 1 1 1
 * 1 2 1
 * 1 3 1
 * 2 2 5
 *******************************/

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
	while (p->down->i != i) {
		p = p->down;
	}
	e = p->down;
	p->down = p->down->down;
	p = &mat->rhead[i];
	while (p->right->j != j) {
		p = p->right;
	}
	p->right = e->right;
	free(e);
}

void insert(struct M *mat, int i, int j, int x) {
	if (x == 0) return;

	struct T *e = (struct T*)malloc(sizeof(struct T));
	*e = (struct T){ i, j, x, 0L, 0L };

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
			p->right->x += e->x;
			if (p->right->x == 0) {
				erase(mat, i, j);
			}
			free(e);
			e = 0L;
		} else {
			e->right = p->right;
			p->right = e;
		}
	}

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
	}

	for (int i = 0; i < mat.row; ++i) {
		struct T *p = mat.rhead[i].right;
		while (!!p) {
			printf("%d %d %d\n", p->i + 1, p->j + 1, p->x);
			p = p->right;
		}
	}

	destroy(&mat);
	return 0;
}