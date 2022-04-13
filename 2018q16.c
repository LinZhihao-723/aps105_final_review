#include "header.h"

typedef struct node {
    int data;
    struct node *left, *right;
} Node;

Node* secondLargestNode(Node* root) {
    if (root == NULL) {
        return NULL;
    }
    Node* current = root;
    Node* parent = NULL;
    while (current->right) {
        parent = current;
        current = current->right;
    }
    if (!current->left) {
        // No left subtree. The parent is the second largest.
        return parent;
    }
    current = current->left;
    while (current->right) {
        current = current->right;
    }
    return current;
}