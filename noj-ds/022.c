/********************************
 *  @author: ZYmelaii 项乔栋
 *  @brief: 数据结构（双语）NOJ T022 - Dijkstra算法（严7.42）
 *  @date: 2022/4/7
 *******************************/

/********************************
 * Description
 * 编写程序，实现以邻接表作存储结构，求从源点到其余各顶点的最短路径的Dijkstra算法。
 * 
 * Input
 * 第一行输入顶点数n和边数m；第二行输入顶点信息；分m行输入m对顶点vi，vj（表示由vi到顶点vj（i不等于j）的边）以及该弧的权值。（0<m,n<100）
 * 
 * Output
 * 输出从源点到其余各顶点的最短路径(不可达用-1表示)。
 * 
 * Sample Input
 * 6 11
 * 1 2 50
 * 1 3 10
 * 1 5 45
 * 2 3 15
 * 2 5 10
 * 3 1 20
 * 3 4 15
 * 4 2 20
 * 4 5 35
 * 5 4 30
 * 6 4 3
 * 
 * Sample Output
 * 1 3 10
 * 1 4 25
 * 1 2 45
 * 1 5 45
 * 1 6 -1
 *******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <limits.h>
#include <stdbool.h>

typedef struct arc_s {
    int vertex;
    int weight;
    struct arc_s *next;
} arc_t;

typedef struct {
    int n;
    arc_t *arcs;
} graph_t;

typedef struct {
    int n, origin;
    int *prevs;
    int *dists;
    int *index;
} solution_t;

void insert(graph_t *G, int from, int to, int weight) {
    arc_t *arcs = G->arcs + from - 1;
    arc_t *arc = (arc_t*)malloc(sizeof(arc_t));
    arc->vertex = to - 1;
    arc->weight = weight;
    arc->next = arcs->next;
    arcs->next = arc;
}

void build(graph_t *G) {
    int n, m;
    int from, to, weight;
    scanf("%d%d", &n, &m);

    arc_t *arcs = (arc_t*)malloc(n * sizeof(arc_t));
    memset(arcs, 0, n * sizeof(arc_t));

    G->n = n;
    G->arcs = arcs;

    while (m--) {
        scanf("%d%d%d", &from, &to, &weight);
        insert(G, from, to, weight);
    }
}

void dijkstra(graph_t *G, int in, solution_t *result) {
    result->n = G->n;
    result->origin = in;
    result->prevs = (int*)malloc(sizeof(int) * G->n);
    result->dists = (int*)malloc(sizeof(int) * G->n);

    bool *flags = (bool*)malloc(sizeof(bool) * G->n);
    int *inproc_set = (int*)malloc(sizeof(int) * G->n);
    int set_begin = 0, set_size = 1;

    in -= 1;
    inproc_set[0] = in;

    for (int i = 0; i < G->n; ++i) {
        result->dists[i] = -1;
        flags[i] = false;
        result->prevs[i] = 0;
    }

    result->dists[in] = 0;

    while (set_size != 0) {
        int set_end = set_begin + set_size, j = 0;
        for (int i = set_begin; i < set_end; ++i) {
            int vertex = inproc_set[i];
            arc_t *arc = G->arcs[vertex].next;
            while (arc != NULL) {
                if (result->dists[arc->vertex] == -1
                    || result->dists[vertex] + arc->weight < result->dists[arc->vertex]) {
                    result->prevs[arc->vertex] = vertex + 1;
                    result->dists[arc->vertex] = result->dists[vertex] + arc->weight;
                }
                if (!flags[arc->vertex]) {
                    inproc_set[set_end + j++] = arc->vertex;
                }
                arc = arc->next;
            }
            flags[vertex] = true;
        }
        set_begin = set_end;
        set_size = j;
    }

    free(flags);
    free(inproc_set);
}

void sort_result(solution_t *result) {
    result->index = (int*)malloc(result->n * sizeof(int));
    for (int i = 0; i < result->n; ++i) {
        result->index[i] = i;
    }
    for (int i = 0; i < result->n - 1; ++i) {
        int k = i, lhs = result->dists[result->index[k]];
        for (int j = i + 1; j < result->n; ++j) {
            int rhs = result->dists[result->index[j]];
            if (lhs > rhs && rhs != -1
                || lhs == rhs && result->index[i] > result->index[j]
                || lhs == -1) {
                k = j;
                lhs = result->dists[result->index[k]];
            }
        }
        lhs = result->index[i];
        result->index[i] = result->index[k];
        result->index[k] = lhs;
    }
}

void output(solution_t *result) {
    for (int i = 0; i < result->n; ++i) {
        if (result->index[i] + 1 != result->origin) {
            printf("%d %d %d\n", result->origin, result->index[i] + 1, result->dists[result->index[i]]);
        }
    }
}

int main(int argc, char const *argv[])
{
    graph_t G;
    solution_t result;
    build(&G);
    dijkstra(&G, 1, &result);
    sort_result(&result);
    output(&result);
	return 0;
}