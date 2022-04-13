#include "header.h"

typedef struct node {
    int data;
    struct node *left, *right;
} Node;

typedef struct bstree {
    Node* root;
} BSTree;

Node* lcaHelper(Node* p, int na, int nb);
Node* lca(BSTree* tree, int na, int nb);

Node* lca(BSTree* tree, int na, int nb) {
    return lcaHelper(tree->root, na, nb);
}

Node* lcaHelper(Node* p, int na, int nb) {
    if (p == NULL) {
        return NULL; // Again, corner case.
    }
    if (p->data > na && p->data > nb) {
        return lcaHelper(p->left, na, nb);
    }
    if (p->data < na && p->data < nb) {
        return lcaHelper(p->right, na, nb);
    }
    return p;
}