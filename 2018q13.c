#include "header.h"

typedef struct node {
    int data;
    struct node* link;
} Node;

void findNumber(Node* start, int target) {
    for (Node* current = start; current != NULL; current = current->link) {
        if (current->data == target) {
            printf("%d\n", target);
            return;
        }
    }
}

void printDuplicates(Node* head) {
    for (Node* current = head; current != NULL; current = current->link) {
        int target = current->data;
        findNumber(current->link, target);
    }
}