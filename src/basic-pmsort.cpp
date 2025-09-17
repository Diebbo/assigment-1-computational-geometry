#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <random>

// Print helper
void printArray(const std::vector<int>& arr) {
    for (int v : arr) printf("%d ", v);
    printf("\n");
}

// Merge two sorted halves
void merge(std::vector<int>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::vector<int> L(n1), R(n2);
    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

// Parallel merge sort
void parallel_merge_sort(std::vector<int>& arr, int left, int right, int depth = 0) {
    if (left >= right) return;

    int mid = left + (right - left) / 2;

    if (depth < 4) { // limit parallel recursion depth
        #pragma omp parallel sections
        {
            #pragma omp section
            parallel_merge_sort(arr, left, mid, depth + 1);
            #pragma omp section
            parallel_merge_sort(arr, mid + 1, right, depth + 1);
        }
    } else {
        // fallback sequential recursion
        parallel_merge_sort(arr, left, mid, depth + 1);
        parallel_merge_sort(arr, mid + 1, right, depth + 1);
    }

    merge(arr, left, mid, right);
}

int main() {
    std::vector<int> a(10);

    // Fill & shuffle array
    for (int i = 0; i < 10; i++) a[i] = i + 1;
    std::shuffle(a.begin(), a.end(), std::default_random_engine(42));

    printf("Unsorted array:\n");
    printArray(a);

    parallel_merge_sort(a, 0, a.size() - 1);

    printf("Sorted array:\n");
    printArray(a);

    return 0;
}
