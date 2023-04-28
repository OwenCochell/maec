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
#include <ctime>
#include <vector>
#include <gtest/gtest.h>

#include "dsp/ft.hpp"

/// Lots of data to compute
std::vector<long double> ft_data = {-81.06114,-32.43124,-38.83822,-111.74227,52.93935,61.08610,-16.21751,-10.95117,-61.76994,-69.22861,0.42157,37.65178,80.44017,-2.04557,-85.95699,2.72509,-66.35543,-2.34489,-40.39149,-3.64988,-17.75498,71.05104,-48.02644,110.04518,-105.14626,-56.32757,95.84381,64.27214,-43.49700,-16.92379,-109.86381,84.79728,77.47628,57.24378,-110.99503,-55.18763,-95.60758,75.47220,-12.70082,-36.99770,-49.17589,-71.22563,108.82306,-110.42035,90.90029,-105.06521,45.91535,-1.05935,-92.97990,-44.71613};

/// Real and non-real parts of above data
std::vector<long double> reals = {-689.551,-360.762,-9.096,-327.623,-237.608,-56.482,-420.058,-405.181,410.474,384.976,-493.179,396.298,58.616,317.64,-125.924,-26.377,-186.769,192.097,-396.526,145.432,-187.555,-345.553,-606.114,-44.831,821.155,-357.606,821.155,-44.831,-606.114,-345.553,-187.555,145.432,-396.526,192.097,-186.769,-26.377,-125.924,317.64,58.616,396.298,-493.179,384.976,410.474,-405.181,-420.058,-56.482,-237.608,-327.623,-9.096,-360.762};
std::vector<long double> nonreals = {0,-56.799,-88.46,114.599,59.815,-318.733,25.484,298.857,-52.783,639.085,-725.061,-217.793,-50.879,-81.235,-312.271,13.61,-225.7,267.968,-138.318,724.441,358.257,367.87,-379.416,-378.675,-204.907,0,204.907,378.675,379.416,-367.87,-358.257,-724.441,138.318,-267.968,225.7,-13.61,312.271,81.235,50.879,217.793,725.061,-639.085,52.783,-298.857,-25.484,318.733,-59.815,-114.599,88.46,56.799};

/**
 * @brief Ensures the length functions are accurate
 * 
 */
TEST(DFT, Length) {

    // Ensure length functions are accurate:

    ASSERT_EQ(length_ft(10), (10 / 2) + 1);

    ASSERT_EQ(length_ift(10), (10 - 1) * 2);
}

/**
 * @brief Tests that random data can transformed and reversed correctly
 * 
 */
TEST(DFT, Random) {

    // Configure the signal size, should be even!

    int size = 20;

    // Create a collection of input data:

    std::vector<long double> nums;

    // Preallocate (just for fun)

    nums.reserve(size);

    // Create a random number generator:

    std::default_random_engine rand;

    // Seed random number generator:

    rand.seed(time(nullptr));

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

        nums.push_back(static_cast<long double>((static_cast<long double>(rand()) - (std::default_random_engine::max() / 4.0)) / std::default_random_engine::max()));

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

/**
 * @brief Tests that the DFT function can work correctly with known data
 * 
 */
TEST(DFT, Known) {

    // Create vectors containing responses:

    std::vector<long double> real;
    std::vector<long double> nonreal;

    // Get length of output data:

    int output_length = length_ft(ft_data.size());

    // Reserve the vectors:

    real.reserve(output_length);
    nonreal.reserve(output_length);

    // Fill the vectors with zeros:

    for (int i = 0; i < output_length; ++i) {

        real.push_back(0);
        nonreal.push_back(0);
    }

    // Now, send the data through the DFT function:

    dft(ft_data.begin(), ft_data.size(), real.begin(), nonreal.begin());

    // Finally, ensure returned data is correct:

    for (int i = 0; i < output_length; ++i) {

        // Check real part:

        ASSERT_NEAR(reals.at(i), real.at(i), 0.001);

        // Check non-real part:

        ASSERT_NEAR(nonreals.at(i), nonreal.at(i), 0.001);
    }
}

/**
 * @brief Tests that the inverse DFT works with known data
 * 
 */
TEST(DFT, InvKnown) {

    // Create vector for output data:

    std::vector<long double> output;

    // Determine size of output data:

    int output_size = length_ift(nonreals.size());

    // Reserve the output data:

    output.reserve(output_size);

    // Fill output buffer with 0:

    for (int i = 0; i < output_size; ++i) {

        output.push_back(0);
    }

    // Send data through function:

    inv_dft(reals.begin(), nonreals.begin(), reals.size(), output.begin());

    // Ensure outgoing data is correct:

    for (int i = 0; i < output_size; ++i) {

        // Ensure returned data is accurate:

        ASSERT_NEAR(ft_data.at(i), output.at(i), 0.00001);
    }
}
