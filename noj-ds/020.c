/********************************
 *  @author: ZYmelaii 项乔栋
 *  @brief: 数据结构（双语）NOJ T020 - 基于图的广度优先搜索策略（耿7.11）
 *  @date: 2022/1/18
 *******************************/

/********************************
 * Description
 * 试基于图的广度优先搜索策略编写程序，判别以邻接表方式存储的有向图中，是否存在由顶点vi到顶点vj的路径（i不等于j）。注意:程序中涉及的图的基本操作必须在此存储结构上实现。
 * 
 * Input
 * 第一行输入有向图的顶点数n和边数m，用空格隔开；第二行输入顶点信息；分m行输入有向图边的信息，例顶点对1，2表示从顶点1到顶点2的一条弧。最后一行输入待判别的顶点对vi，vj。(0<m,n<100)
 * 
 * Output
 * 若有向图中存在由顶点vi到顶点vj的路径（i不等于j），则输出yes；否则输出no。
 * 
 * Sample Input
 * 4 4
 * 1 2 3 4
 * 1 2
 * 1 3
 * 1 4
 * 2 3
 * 2 3
 * 
 * Sample Output
 * yes
 *******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>

struct E {
	int vi;
	struct E *next;
};

struct V {
	int x;
	struct E *first;
};

#define MAX_QLEN 100

struct Q { int queue[MAX_QLEN]; int rear, length; };

int front(struct Q *queue) {
	return queue->queue[(queue->rear - queue->length + MAX_QLEN) % MAX_QLEN];
}

int rear(struct Q *queue) {
	return queue->queue[(queue->rear - 1 + MAX_QLEN) % MAX_QLEN];
}

bool push(struct Q *queue, int e) {
	if (queue->length == MAX_QLEN) {
		return false;
	}
	++queue->length;
	queue->queue[queue->rear] = e;
	queue->rear = (queue->rear + 1) % MAX_QLEN;
	return true;
}

bool pop(struct Q *queue, int *e) {
	if (queue->length == 0) {
		return false;
	}
	if (!!e) {
		*e = front(queue);
	}
	--queue->length;
	return true;
}

bool bfs(struct V *vs, int vi, int vj, bool *flags) {
	struct Q queue = { 0 };
	struct E *e;
	int v;

	push(&queue, vi);
	flags[vi - 1] = true;

	while (queue.length != 0) {
		pop(&queue, &v);
		if (v == vj) {
			return true;
		}
		e = vs[v - 1].first;
		while (!!e) {
			if (!flags[e->vi]) {
				push(&queue, e->vi + 1);
				flags[e->vi] = true;
			}
			e = e->next;
		}
	}

	return false;
}

int main(int argc, char const *argv[])
{
	int n, m, vi, vj, t = -1;
	struct V vertices[100] = { 0 };
	struct E edges[100] = { 0 };
	bool flags[100] = { 0 };

	scanf("%d%d", &n, &m);
	memset(flags, 0, sizeof(int) * n);

	for (int i = 0; i < n; ++i) {
		scanf("%d", &vertices[i].x);
		vertices[i].first = 0L;
	}

	for (int i = 0; i < m; ++i) {
		scanf("%d%d", &vi, &vj);
		struct E *edge = &edges[++t];
		edge->next = 0L;
		edge->vi = vj - 1;
		if (!vertices[vi - 1].first) {
			vertices[vi - 1].first = edge;
		} else {
			struct E *e = vertices[vi - 1].first;
			while (!!e->next) {
				e = e->next;
			}
			e->next = edge;
		}
	}

	scanf("%d%d", &vi, &vj);

	puts(bfs(vertices, vi, vj, flags) ? "yes" : "no");

	// 注：上述假定所有值为vi的节点索引为vi-1（事实也确实如此）

	return 0;
}