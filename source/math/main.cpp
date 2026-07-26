#include <iostream>
#include "math_utils.h"
#include "log.hpp"

int main() {
    int a = 5, b = 10;
    std::cout << "Math Sample: " << a << " + " << b << " = " << sample_math::add(a, b) << std::endl;
    std::cout << "Math Sample: " << a << " * " << b << " = " << sample_math::multiply(a, b) << std::endl;

    // Example of using a thirdparty library (symbolically)
    M_LOG_INFO("Math application started");

    return 0;
}
