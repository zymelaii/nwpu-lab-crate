#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int f = 0;

struct tagxue {
    int  no, age;
    char name;
};

typedef struct tagLNode {
    struct tagxue    data;
    struct tagLNode *next;
} LNode, *LinkList;

int input(struct tagxue *l) {
    int  n, m;
    char a[11];
    scanf("%d", &n);
    if (n == 0) {
        f = 1;
        return 0;
    };
    scanf("%s%d", a, &m);
    l->age = m;
    strcpy(&l->name, a);
    l->no = n;
    return 0;
}

void create(LinkList *l) {
    LinkList p, s;
    int      n;
    p = *l = (LinkList)malloc(sizeof(LNode));
    for (n = 0;; n++) {
        s = (LinkList)malloc(sizeof(LNode));
        input(&s->data);
        if (f == 1) break;
        p->next = s, p = s;
    }
    p->next = NULL;
}

void list(LinkList l) {
    LinkList p = l->next;
    while (p != NULL) {
        printf("%d ", p->data.no);
        p = p->next;
    }
}

void shanchu(LinkList l, int n) {
    LinkList p = l->next;
    while (p != NULL) {
        if (p->next->data.no == n) break;
        p = p->next;
    }
    p->next = p->next->next;
}

int main() {
    LinkList q;
    int      m;
    q = (LinkList)malloc(sizeof(LNode));
    create(&q);
    scanf("%d", &m);
    list(q);
    printf("\n");
    shanchu(q, m);
    list(q);
    return 0;
}
