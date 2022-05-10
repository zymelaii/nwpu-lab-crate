/********************************
 *  @author: ZYmelaii 项乔栋
 *  @brief: 数据结构（双语）NOJ T025 - 二叉排序树的插入与删除（严9.35,、9.36和9.37）
 *  @date: 2022/4/7
 *******************************/

/********************************
 * Description
 * 假设二叉排序树以后继线索链表作存储结构，编写程序，满足以下要求：
 * 输出该二叉排序树中所有大于a小于b的关键字；
 * 在二叉排序树中插入一个关键字；
 * 在二叉排序树中删除一个关键字。
 * 
 * Input
 * 第一行按先序输入二叉排序树各结点（结点值大于0），其期中-1表示取消建立子树结点；第二行输入要求1中a、 b，用空格隔开；
 * 第三行输入要求2中要插入的关键字；第四行输入要求3中要删除的关键字。
 * 
 * Output
 * 按照中序序列，分三行输出要求1、要求2和要求3的结果。
 * 
 * Sample Input
 * 12 8 4 -1 -1 10 -1 -1 16 13 -1 -1 18 -1 -1
 * 10 17
 * 6
 * 12
 * 
 * Sample Output
 * 12 13 16
 * 4 6 8 10 12 13 16 18
 * 4 8 10 13 16 18
 *******************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <limits.h>
#include <stdbool.h>
#include <assert.h>
#include <stddef.h>

typedef struct node_s {
    struct node_s *left, *right;
    int value;
    bool lthread, rthread;
} node_t;

void build(node_t *node) {
    int value;
    scanf("%d", &value);
    if (value == -1) {
        node->value = -1;
        return;
    }
    node->value = value;
    node->left = (node_t*)malloc(sizeof(node_t));
    node->right = (node_t*)malloc(sizeof(node_t));
    build(node->left);
    build(node->right);
    if (node->left->value == -1) {
        free(node->left);
        node->left = NULL;
    }
    if (node->right->value == -1) {
        free(node->right);
        node->right = NULL;
    }
    node->lthread = node->left == NULL;
    node->rthread = node->right == NULL;
}

void lin_threadify(node_t *prev, node_t *node) {
    if (node == NULL) return;
    if (node->lthread) {
        node->left = prev;
        if (node->rthread) return;
    }
    lin_threadify(prev, node->lthread ? NULL : node->left);
    lin_threadify(node, node->rthread ? NULL : node->right);
}

void rin_threadify(node_t *prev, node_t *node) {
    if (node == NULL) return;
    if (node->rthread) {
        node->right = prev;
        if (node->lthread) return;
    }
    rin_threadify(node, node->lthread ? NULL : node->left);
    rin_threadify(prev, node->rthread ? NULL : node->right);
}

node_t* begin(node_t *tree) {
    if (tree == NULL) return NULL;
    while (tree->left) {
        tree = tree->left;
    }
    return tree;
}

node_t* rbegin(node_t *tree) {
    if (tree == NULL) return NULL;
    while (tree->right) {
        tree = tree->right;
    }
    return tree;
}

node_t* next(node_t *node) {
    bool is_rthread = node->rthread;
    node = node->right;
    if (is_rthread) return node;
    while (!node->lthread) {
        node = node->left;
    }
    return node;
}

node_t* prev(node_t *node) {
    bool is_lthread = node->lthread;
    node = node->left;
    if (is_lthread) return node;
    while (!node->rthread) {
        node = node->right;
    }
    return node;
}

void insert(node_t *tree, int value) {
    node_t *it = begin(tree);
    while (it != NULL && it->value <= value) {
        it = next(it);
    }

    node_t *node = (node_t*)malloc(sizeof(node_t));
    node->value = value;

    if (it == NULL) {
        it = rbegin(tree);
        it->right = node;
        node->left = NULL;
        node->right = NULL;
    } else {
        node->left = it->lthread ? NULL : it->left;
        node->right = NULL;
        it->left = node;
    }

    node->lthread = node->left == NULL;
    node->rthread = node->right == NULL;

    lin_threadify(NULL, tree);
    rin_threadify(NULL, tree);
}

node_t* erase(node_t *tree, int value) {
    node_t *it = begin(tree), *node, *parent;
    while (it != NULL && it->value != value) {
        it = next(it);
    }

    if (it == NULL) return tree;

    if (!it->lthread) {
        node = it->left;
        while (!node->rthread) {
            node = node->right;
        }
        it->value = node->value;
        if (node == it->left) {
            it->left = NULL;
            it->lthread = true;
        } else {
            node = prev(node);
            node->right = NULL;
            node->rthread = true;
        }
    } else if (!it->rthread) {
        node = it->right;
        while (!node->lthread) {
            node = node->left;
        }
        it->value = node->value;
        if (node == it->right) {
            it->right = NULL;
            it->rthread = true;
        } else {
            node = next(node);
            node->left = NULL;
            node->lthread = true;
        }
    } else if ((parent = prev(it)) != NULL && parent->right == it) {
        parent->right = NULL;
        parent->rthread = true;
    } else if ((parent = next(it)) != NULL && parent->left == it) {
        parent->left = NULL;
        parent->lthread = true;
    } else {
        return NULL;
    }

    lin_threadify(NULL, tree);
    rin_threadify(NULL, tree);

    return tree;
}

void inorder(node_t *node) {
    if (node == NULL) return;
    inorder(node->lthread ? NULL : node->left);
    printf("%d ", node->value);
    inorder(node->rthread ? NULL : node->right);
}

int main(int argc, char const *argv[])
{
    node_t root, *node, *it;
    build(&root);
    lin_threadify(NULL, &root);
    rin_threadify(NULL, &root);

    int a, b, insertion, deletion;
    scanf("%d%d%d%d", &a, &b, &insertion, &deletion);

    //! requirement 1
    node = it = begin(&root);
    while (it != NULL && it->value < b) {
        if (it->value > a) {
            printf("%d ", it->value);
        }
        it = next(it);
    }
    puts("");

    //! requirement 2
    insert(&root, insertion);
    it = begin(&root);
    while (it != NULL) {
        printf("%d ", it->value);
        it = next(it);
    }
    puts("");
    erase(&root, insertion); //@ restore

    //! requirement 3
    it = begin(erase(&root, deletion));
    while (it != NULL) {
        printf("%d ", it->value);
        it = next(it);
    }

    return 0;
}