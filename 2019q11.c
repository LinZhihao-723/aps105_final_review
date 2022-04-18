#include "header.h"

typedef struct node {
    int data;
    struct node* next;
} Node;

typedef struct node {
    Node* head;
} LinkedList;

void bubbleSortLinkedList(LinkedList* list) {
    int listLen = 0;
    Node* current;

    current = list->head;
    while (current) {
        ++listLen;
        current = current->next;
    }
    if (listLen <= 1) {
        return;
    }

    Node* next;
    bool sorted = false;
    for (int top = listLen - 1; top > 0 && !sorted; --top) {
        sorted = true;
        current = list->head;
        for (int i = 0; i < top; ++i) {
            next = current->next; 
            if (current->data > next->data) {
                int temp = current->data;
                current->data = next->data;
                next->data = temp;
                sorted = false;
            }
            current = next;
        }
    }
}