#include <stdio.h>
#include <stdlib.h>

typedef struct tagLNode {
    int              data;
    struct tagLNode *next;
} LNode, *LinkList;

void create(LinkList *l, int n) {
    LinkList p, s;
    p = *l = (LinkList)malloc(sizeof(LNode));
    for (; n > 0; n--) {
        s = (LinkList)malloc(sizeof(LNode));
        scanf("%d", &s->data);
        p->next = s, p = s;
    }
    p->next = NULL;
}

void list(LinkList l) {
    LinkList p = l->next;
    while (p != NULL) {
        printf("%d ", p->data);
        p = p->next;
    }
}

void chazhao(LinkList l, int n) {
    LinkList p = l->next, s, q;
    int      i, j, k;
    for (i = 0; i < 100; i++) {
        j = p->data;
        q = p->next;
        k = q->data;
        if (j < n && k > n) break;
        p = p->next;
    }
    s       = (LinkList)malloc(sizeof(LNode));
    s->data = n;
    s->next = p->next, p->next = s;
}

int main() {
    LinkList q;
    int      n, m;
    q = (LinkList)malloc(sizeof(LNode));
    scanf("%d", &n);
    create(&q, n);
    scanf("%d", &m);
    chazhao(q, m);
    list(q);
    return 0;
}
