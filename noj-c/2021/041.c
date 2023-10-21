#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main() {
    LinkList q;
    int      n;
    q = (LinkList)malloc(sizeof(LNode));
    scanf("%d", &n);
    create(&q, n);
    list(q);
    return 0;
}
