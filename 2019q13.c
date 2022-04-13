#include "header.h"

void swap(char* a, char* b) {
    char temp = *a;
    *a = *b;
    *b = temp;
}

void permutateHelper(char* str, char* current) {
    /*
        The order of final result is slightly different:
            abc
            acb
            bac
            bca
            cba
            cab
        But it doesn't matter. This is the most standard solution for permutation problem.
    */
    if (*current == '\0') {
        printf("%s\n", str);
        return;
    }
    for (char* p = current; *p != '\0'; ++p) {
        swap(current, p);
        permutateHelper(str, current + 1);
        swap(current, p);
    }
}

void permutateOrder(char* str, char* current) {
    /*
        The order is exactly the same.
    */
    if (*current == '\0') {
        printf("%s\n", str);
        return;
    }
    for (int i = 0, size = strlen(current); i < size; ++i) {
        permutateHelper(str, current + 1);
        char newTail = current[0];
        for (int j = 0; j < size - 1; ++j) {
            current[j] = current[j + 1];
        }
        current[size - 1] = newTail;
    }
}

void permutate(char* str) {
    permutateHelper(str, str);
    //permutateOrder(str, str);
}

int main() {
    char str[] = "abc";
    permutate(str);
    return 0;
}