#ifndef MERGE_SORT_H
#define MERGE_SORT_H

#include <algorithm>
#include <omp.h>
#include <random>
#include <stdio.h>
#include <stdlib.h>

#include <vector>

void printArray(const std::vector<int> &arr);
void merge(std::vector<int> &arr, int left, int mid, int right);
void parallel_merge_sort(std::vector<int> &arr, int left, int right,
                         int depth = 0);
void read_input(std::vector<int> &arr, int n, char *inputs[]);
void init_default_vector(std::vector<int> &arr);
std::pair<int, int> selection(const std::vector<int> &A,
                              const std::vector<int> &B, int k);
void parallel_merge(std::vector<int>& A, std::vector<int>& B, std::vector<int>& C);

#endif
