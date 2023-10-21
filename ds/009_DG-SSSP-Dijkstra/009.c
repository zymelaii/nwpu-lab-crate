/**
 * @brief SSSP problom of UG based on dijkstra
 * @author zymelaii
 * @date 2022-04-09
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <limits.h>
#include <stdbool.h>

typedef struct arc_s {
    int vertex;
    int weight;
    struct arc_s* next;
} arc_t;

typedef struct {
    int n;
    arc_t* arcs;
} graph_t;

typedef struct {
    int n, origin;
    int* prevs;
    int* dists;
} solution_t;

void insert(graph_t* G, int from, int to, int weight) {
    arc_t* arcs = G->arcs + from;
    arc_t* arc = (arc_t*)malloc(sizeof(arc_t));
    arc->vertex = to;
    arc->weight = weight;
    arc->next = arcs->next;
    arcs->next = arc;
}

void build(graph_t* G) {
    int n, x;
    scanf("%d", &n);

    arc_t* arcs = (arc_t*)malloc(n * sizeof(arc_t));
    memset(arcs, 0, n * sizeof(arc_t));

    G->n = n;
    G->arcs = arcs;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            scanf("%d", &x);
            if (x == 0 || x == 10000) continue;
            insert(G, i, j, x);
        }
    }
}

void dijkstra(graph_t* G, int in, solution_t* result) {
    result->n = G->n;
    result->origin = in;
    result->prevs = (int*)malloc(sizeof(int) * G->n);
    result->dists = (int*)malloc(sizeof(int) * G->n);

    int* flags = (int*)malloc(sizeof(int) * G->n);
    int* inproc_set = (int*)malloc(sizeof(int) * G->n);
    int set_begin = 0, set_size = 1;

    inproc_set[0] = in;

    enum { eFree, eInproc, eDone };

    for (int i = 0; i < G->n; ++i) {
        result->dists[i] = INT_MAX / 2;
        flags[i] = eFree;
        result->prevs[i] = -1;
    }

    result->dists[in] = 0;

    while (set_size != 0) {
        int set_end = set_begin + set_size, j = 0;
        for (int i = set_begin; i < set_end; ++i) {
            int vertex = inproc_set[i];
            arc_t* arc = G->arcs[vertex].next;
            while (arc != NULL) {
                if (result->dists[vertex] + arc->weight < result->dists[arc->vertex]) {
                    result->prevs[arc->vertex] = vertex;
                    result->dists[arc->vertex] = result->dists[vertex] + arc->weight;
                }
                if (flags[arc->vertex] == eFree) {
                    inproc_set[set_end + j++] = arc->vertex;
                    flags[arc->vertex] = eInproc;
                }
                arc = arc->next;
            }
            flags[vertex] = eDone;
        }
        set_begin = set_end;
        set_size = j;
    }

    free(inproc_set);
    free(flags);
}

void output(solution_t* result, int out) {
    int* path = (int*)calloc(result->n, sizeof(int)), q = -1;
    int p = out;

    do {
        path[++q] = p;
        p = result->prevs[p];
    } while (p != -1);

    while (q >= 0) {
        printf("%d\n", path[q--]);
    }

    free(path);
}

int main(int argc, char const* argv[])
{
    int in, out;
    graph_t G;
    solution_t result;
    build(&G);
    scanf("%d%d", &in, &out);
    dijkstra(&G, in, &result);
    output(&result, out);
    return 0;
}