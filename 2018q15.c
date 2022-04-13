#include "header.h"

void printArray(int input[], int size) {
    for (int i = 0; i < size; ++i) {
        printf("%d ", input[i]);
    }
    printf("\n");
}

void sortOddEven(int input[], int size) {
    int left = 0;
    int right = size - 1;
    while (right > left) {
        while (left < right && input[left] % 2 == 1) {
            ++left;
        }
        while (right > left && input[right] % 2 == 0) {
            --right;
        }
        int temp = input[right];
        input[right] = input[left];
        input[left] = temp;
    }
}

int main() {
    // int input[] = {1, 4, 6, 5, 9, 3, 8, 2};
    int input[] = {2, 4, 1, 3, 5, 6, 8};
    int size = sizeof(input) / sizeof(int);
    sortOddEven(input, size);
    printArray(input, size);
    return 0;
}
