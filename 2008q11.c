#include "header.h"

typedef struct node {
    int info;
    struct node* link;
} Node, *NodePointer;

NodePointer join(NodePointer head1, NodePointer head2) {
    if (head1 == NULL && head2 == NULL) {
        return NULL;
    }
    if (head1 == NULL) {
        return head2;
    } 
    if (head2 == NULL) {
        return head1;
    }

    NodePointer newHead = NULL;
    NodePointer newCurrent = NULL;
    NodePointer current1 = head1;
    NodePointer current2 = head2;
    while (current1 && current2) {
        NodePointer small;
        if (current1->info <= current2->info) {
            small = current1;
            current1 = current1->link;
        } else {
            small = current2;
            current2 = current2->link;
        }
        if (!newHead) {
            newHead = small;
            newCurrent = newHead;
        } else {
            if (newCurrent->info == small->info) {
                free(small);
            } else {
                newCurrent->link = small;
                newCurrent = newCurrent->link;
            }
        }
    }
    if (current1) {
        newCurrent->link = current1;
    }
    if (current2) {
        newCurrent->link = current2;
    }
    return newHead;
}