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
void merge(std::vector<int>& arr, int left, int mid, int right);
void parallel_merge_sort(std::vector<int>& arr, int left, int right, int depth = 0);
std::vector<int> read_input(int n, char* inputs[]);

#endif
