#include <iostream>
#include <vector>
#include "sort_utils.h"
#include <json/json.h>

int main() {
    std::vector<int> data = {64, 34, 25, 12, 22, 11, 90};

    std::cout << "Unsorted: ";
    for (int x : data) std::cout << x << " ";
    std::cout << std::endl;

    sample_sort::bubble_sort(data);

    std::cout << "Sorted:   ";
    for (int x : data) std::cout << x << " ";
    std::cout << std::endl;

    // Example of using a thirdparty library (symbolically)
    Json::Value root;
    root["status"] = "success";

    return 0;
}
