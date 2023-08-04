/**
 * @file kernel_test.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Tests for kernel functions
 * @version 0.1
 * @date 2023-07-31
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <vector>
#include <gtest/gtest.h>

#include "dsp/kernel.hpp"

/**
 * @brief Ensures we can generate a valid sinc filter kernel
 * 
 */
TEST(KernelTest, Sinc) {

    // Create vector to hold results:

    std::vector<long double> out(51);

    // Call sinc function:

    sinc_kernel(0.25, 50, out.begin());
}
