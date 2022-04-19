#include "header.h"

typedef struct node {
    int data;
    struct node* next;
} Node;

typedef struct node {
    Node* head;
} LinkedList;

void bubbleSortLinkedList(LinkedList* list) {
    int listSize = 0;
    Node* current;

    current = list->head;
    // while (current != NULL) {
    while (current) {
        ++listSize;
        current = current->next;
    }
    if (listSize == 0 || listSize == 1) {
        return;
    }

    bool sorted = false;
    for (int top = listSize - 1; top > 0 && !sorted; --top) {
        sorted = true;
        current = list->head;
        for (int i = 0; i < top; ++i) {
            // Traverse #top times.
            Node* next = current->next;
            if (current->data > next->data) { /* if (a[i] > a[i + 1]) */
                int temp = next->data;
                next->data = current->data;
                current->data = temp;
                sorted = false;
            }
            current = current->next;
        }
    }
}