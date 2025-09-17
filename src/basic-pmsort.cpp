#include "merge_sort.h"

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
void parallel_merge_sort(std::vector<int>& arr, int left, int right, int depth) {
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

void read_input(std::vector<int>& arr, int n, char* inputs[]) {
    for (int i = 0; i < n; i++) {
        arr[i] = atoi(inputs[i]);
    }
}

void init_default_vector(std::vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n; i++) arr[i] = i + 1;
    std::shuffle(arr.begin(), arr.end(), std::default_random_engine(42));
}

