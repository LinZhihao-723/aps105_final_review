#include "header.h"

double hornerHelper(const double a[], const int n, const int i, const double x) {
    if (n == 0) {
        return 0;
    }
    if (i == n) {
        return a[i];
    }
    return a[i] + x * hornerHelper(a, n, i + 1, x);
}

double horner(double a[], int n, double x) {
    return hornerHelper(a, n, 0, x);
}