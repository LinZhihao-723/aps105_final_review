#include "header.h"

bool canMakeSumHelper(int inputArray[], int arraysize, int targetSum, int current, int remain) {
    if (remain == 0 && targetSum == 0) {
        return true;
    }
    if (current == arraysize) {
        return false;
    }
    return canMakeSumHelper(inputArray, arraysize, targetSum - inputArray[current], current + 1, remain - 1) ||
        canMakeSumHelper(inputArray, arraysize, targetSum, current + 1, remain);
}

bool canMakeSum(int inputArray[], int arraysize, int targetSum) {
    return canMakeSumHelper(inputArray, arraysize, targetSum, 0, 3);
}

int main() {
    int inputArray[] = {1, 6, 8, 11, 15};
    int sum;
    printf("Enter sum: ");
    scanf("%d", &sum);
    if (canMakeSum(inputArray, sizeof(inputArray) / sizeof(int), sum)) {
        printf("Has sum!\n");
    } else {
        printf("No sum!\n");
    }
    return 0;
}