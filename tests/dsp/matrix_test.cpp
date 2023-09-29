/**
 * @file matrix_test.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Tests for matrix operations
 * @version 0.1
 * @date 2023-09-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "dsp/matrix.hpp"

#include <gtest/gtest.h>

//
// Matrix Definitions
//

// Simple matrix, same rows/cols:

std::vector<std::vector<int>> a_same = {
    {1, 2, 3},
    {4, 5, 6},
    {7, 8, 9},
};

std::vector<std::vector<int>> b_same = {
    {10, 20, 30},
    {40, 50, 60},
    {70, 80, 90}
};

TEST(Matrix, DotProduct) {

    a_same.cbegin();

    auto thing = b_same.cbegin();

    // Create a buffer for output:

    Buffer<int> out;

    // Define matrixes:

    Buffer<int> a(a_same);
    Buffer<int> b(b_same);

    // Multiply:

    matrix_mult(a, b, out);
}