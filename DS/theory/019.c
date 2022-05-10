/********************************
 *  @author: ZYmelaii 项乔栋
 *  @brief: 数据结构（双语）NOJ T019 - 基于图的深度优先搜索策略（耿7.10）
 *  @date: 2022/1/18
 *******************************/

/********************************
 * Description
 * 试基于图的深度优先搜索策略编写程序，判别以邻接表方式存储的有向图中，是否存在由顶点vi到顶点vj的路径（i不等于j）。注意:程序中涉及的图的基本操作必须在此存储结构上实现。
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

void dfs(struct V *vs, struct V *v, bool *flags) {
	flags[v->x - 1] = true;
	struct E *e = v->first;
	while (!!e) {
		if (!flags[e->vi]) {
			dfs(vs, &vs[e->vi], flags);
		}
		e = e->next;
	}
}

int main(int argc, char const *argv[])
{
	int n, m, vi, vj, t = -1;
	struct V *vertices;
	struct E *edges;
	bool *flags;

	scanf("%d%d", &n, &m);
	flags = (bool*)malloc(sizeof(bool) * n);
	vertices = (struct V*)malloc(sizeof(struct V) * n);
	edges = (struct E*)malloc(sizeof(struct E) * m);
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

	dfs(vertices, &vertices[vi - 1], flags);
	puts(flags[vj - 1] ? "yes" : "no");

	// 注：上述假定所有值为vi的节点索引为vi-1（事实也确实如此）

	free(flags);
	free(edges);
	free(vertices);
	return 0;
}