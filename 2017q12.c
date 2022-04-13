#include "header.h"

struct treeNode {
    int value;
    struct treeNode *left, *right;
};

int treeLeafCount(struct treeNode* root) {
    if (root == NULL) {
        return 0; // Corner case: the actual root is NULL.
    }
    if (!root->left && !root->right) {
        return 1; // A leaf.
    }
    int leftSubtreeCount = treeLeafCount(root->left);
    int rightSubtreeCount = treeLeafCount(root->right);
    return leftSubtreeCount + rightSubtreeCount;
}