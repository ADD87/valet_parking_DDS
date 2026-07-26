#include "sort_utils.h"
#include <algorithm>

namespace sample_sort {
    void bubble_sort(std::vector<int>& arr) {
        if (arr.empty()) return;
        for (size_t i = 0; i < arr.size() - 1; ++i) {
            for (size_t j = 0; j < arr.size() - i - 1; ++j) {
                if (arr[j] > arr[j + 1]) {
                    std::swap(arr[j], arr[j + 1]);
                }
            }
        }
    }
}
