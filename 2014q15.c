#include "header.h"

#define SIZE 10

void fillArray(int array[][SIZE]) {
    int temp[SIZE][SIZE] = {
        0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
        0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 2, 2, 0, 0,
        0, 0, 0, 0, 0, 0, 2, 2, 0, 0,
        0, 0, 0, 0, 2, 2, 2, 2, 2, 2,
        0, 0, 0, 0, 2, 2, 2, 2, 2, 0,
        0, 0, 0, 0, 0, 0, 2, 2, 0, 0,
        0, 0, 0, 0, 0, 0, 2, 2, 0, 0
    };
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            array[i][j] = temp[i][j];
        }
    }
}

bool checkDirection(int array[][SIZE], int num, int drow, int dcol, int row, int col, int len) {
    for (int i = 0; i < len; ++i) {
        row += drow;
        col += dcol;
        if (array[row][col] != num) {
            return false;
        }
    }
    return true;
}

void checkCross(int n, int array[][SIZE]) {
    int len = n / 2;
    for (int row = len; row < SIZE - len; ++row) {
        for (int col = len; col < SIZE - len; ++col) {
            if (array[row][col] == 0) {
                continue;
            }
            int num = array[row][col];
            if (checkDirection(array, num,  1,  0, row, col, len) &&
                checkDirection(array, num, -1,  0, row, col, len) &&
                checkDirection(array, num,  0,  1, row, col, len) &&
                checkDirection(array, num,  0, -1, row, col, len)) {
                printf("Found Cross of Size %d of %d at (%d, %d)\n",
                    n, num, row, col);
            }
        }
    }
}

int main(void) {
    int n, array[SIZE][SIZE];
    fillArray(array);
    printf("Enter Cross Size to search (must be odd): ");
    scanf("%d", &n);
    checkCross(n, array);
    return 0;
}