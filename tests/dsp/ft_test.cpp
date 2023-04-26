/**
 * @file ft_test.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Tests for FT operations
 * @version 0.1
 * @date 2023-04-17
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <random>
#include <chrono>
#include <vector>
#include <gtest/gtest.h>

#include "dsp/ft.hpp"

/**
 * @brief Ensures the length functions are accurate
 * 
 * TODO: SEED RANDOM NUMBER GENERATOR!
 * Use time or something...
 */
TEST(DFT, Length) {

    // Ensure length functions are accurate:

    ASSERT_EQ(length_ft(10), (10 / 2) + 1);

    ASSERT_EQ(length_ift(10), (10 - 1) * 2);
}

TEST(DFT, Random) {

    // Configure the signal size, should be even!

    int size = 4;

    // Create a collection of input data:

    std::vector<long double> nums;

    // Preallocate (just for fun)

    nums.reserve(size);

    // Create a random number generator:

    std::default_random_engine rand;

    // Create containers for real and non-real parts

    std::vector<long double> nonreal;
    std::vector<long double> real;

    // Reserve parts:

    nonreal.reserve(length_ft(size));
    real.reserve(length_ft(size));

    // Create container for output signal:

    std::vector<long double> output;

    output.reserve(size);

    // Iterate a number of times

    for (int i = 0; i < size; ++i) {

        // Add a random value to the collection:

        nums.push_back(static_cast<long double>(static_cast<long double>(rand()) / std::default_random_engine::max()));

        // Add a zero to the output signal:

        output.push_back(0);
    }

    for (int i = 0; i < length_ft(size); ++i) {

        // Set each value to zero:

        nonreal.push_back(0);
        real.push_back(0);
    }

    // Now, send the data through the FT algorithm:

    dft(nums.begin(), size, real.begin(), nonreal.begin());

    // Send parts through inverse algorithm:

    inv_dft(real.begin(), nonreal.begin(), length_ft(size), output.begin());

    // Ensure data matches:

    for(int i = 0; i < size; ++i) {

        // Check this value:

        ASSERT_NEAR(nums.at(i), output.at(i), 0.0000001);
    }
}
