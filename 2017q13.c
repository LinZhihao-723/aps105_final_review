#include "header.h"

void swap(double list[], int a, int b) {
    double temp = list[a];
    list[a] = list[b];
    list[b] = temp;
}

void printList(double list[], int size) {
    for (int i = 0; i < size; ++i) {
        printf("%lf ", list[i]);
    }
    printf("\n");
}

int selectPivotAndPartition(double list[], int from, int to) {
    double pivot = list[from];
    int left = from + 1;
    int right = to;
    while (true) {
        while (left <= right && list[left] < pivot) {
            ++left;
        }
        while (right >= left && list[right] > pivot) {
            --right;
        }
        if (left < right) {
            swap(list, left, right);
        } else {
            swap(list, right, from);
            return right;
        }
    }
}

void insertionSort(double list[], int from, int to) {
    printf("Insertion sort from %d to %d\n", from, to);
    for (int i = from + 1; i <= to; ++i) {
        for (int j = i; j > 0; --j) {
            if (list[j] < list[j - 1]) {
                swap(list, j, j - 1);
            } else {
                break;
            }
        }
    }
}

void quickSort(double list[], int from, int to) {
    printf("Quick sort from %d to %d\n", from, to);
    if (from >= to) {
        return;
    }
    if (to - from <= 10) {
        insertionSort(list, from, to);
        return;
    }
    int pivot = selectPivotAndPartition(list, from, to);
    quickSort(list, from, pivot - 1);
    quickSort(list, pivot + 1, to);
}

int main() {
    double list[] = {
        6.37, 9.58, 1.6, -88.37, 96.53, -99.0, 83.0, 82.0, 73.9, 66.69,
        -72.5, 98.64, 28.57, -59.38, 22.4, 22.4
    };
    int size = sizeof(list) / sizeof(double);
    quickSort(list, 0, size - 1);
    printList(list, size);
    return 0;
}