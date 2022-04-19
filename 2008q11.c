#include "header.h"

typedef struct node {
    int data;
    struct node* next;
} Node;

Node* join(Node* head1, Node* head2) {
    if (head1 == NULL && head2 == NULL) {
        return NULL;
    }
    if (head1 == NULL) {
        return head2;
    }
    if (head2 == NULL) {
        return head1;
    }

    Node* newHead = NULL;
    Node* tail = NULL;

    Node* current1;
    Node* current2;

    current1 = head1;
    current2 = head2;

    while (current1 && current2) {
        Node* small;
        if (current1->data < current2->data) {
            small = current1;
            current1 = current1->next;
        } else {
            small = current2;
            current2 = current2->next;
        }
        if (tail == NULL) {
            newHead = small;
            tail = newHead;
        } else {
            if (tail->data == small->data) {
                free(small);
            } else {
                tail->next = small;
                tail = small;
            }
        }
    }
    if (current1 == NULL) {
        tail->next = current2;
    }
    if (current2 == NULL) {
        tail->next = current1;
    }
    return newHead;
}