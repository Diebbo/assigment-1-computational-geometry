#ifndef MERGE_SORT_H
#define MERGE_SORT_H

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <random>
#include <ctime>

#include <vector>

void printArray(const std::vector<int>& arr);
void sequential_merge(std::vector<int>& arr, int left, int mid, int right);
void parallel_merge_sort(std::vector<int>& arr, int left, int right, int depth = 0);
std::vector<int> read_input(int n, char* inputs[]);
std::pair<int, int> selection(const std::vector<int> &A,
                              const std::vector<int> &B, int k);
void parallel_merge(std::vector<int> &A, std::vector<int> &B,
                    std::vector<int> &C, int start = 0);
void new_parallel_merge(const std::vector<int> &A, int a_start, int a_end,
                        const std::vector<int> &B, int b_start, int b_end,
                        std::vector<int> &C, int c_start);
void fully_parallel_merge_sort(std::vector<int> &arr, int left, int right,
                               int depth = 0);
void new_parallel_merge(const std::vector<int> &A, int a_start, int a_end,
                        const std::vector<int> &B, int b_start, int b_end,
                        std::vector<int> &C, int c_start);
#endif
