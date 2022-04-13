#include "header.h"

typedef struct node {
    int data;
    struct node *left, *right;
} Node;

typedef struct bstree {
    Node* root;
} BSTree;

void initBSTree(BSTree* tree);
void insert(BSTree* tree, int value);

BSTree* treeWithEvens(BSTree* inputTree);
void evensHelper(Node* currrent, BSTree* evenTree);

void evensHelper(Node* currrent, BSTree* evenTree) {
    if (!current) {
        return;
    } 
    if (current->data % 2 == 0) {
        insert(evenTree, current->data);
    }
    evensHelper(current->left, evenTree);
    evensHelper(current->right, evenTree);
}

BSTree* treeWithEvens(BSTree* inputTree) {
    if (!inputTree) {
        return NULL;
    }
    BSTree* evenTree = malloc(sizeof(BSTree));
    initBSTree(evenTree);
    evensHelper(inputTree->root, evenTree);
    return evenTree;
}