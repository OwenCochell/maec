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
std::vector<long double> reals = {-689.550950000000000495, -360.761730393757302754, -9.096278055139454366, -327.622828943793179379, -237.608057949996392261, -56.4823335254946352346, -420.058431365069619989, -405.181098212725274849, 410.473673843006212542, 384.975560036772850053, -493.178524073104277453, 396.29829142118509408, 58.6164290163560154981, 317.639586912586160045, -125.923587794970665815, -26.3771664745064575632, -186.769206518632219671, 192.096986502756232273, -396.525639886059804523, 145.432315203991060118, -187.554675926895965149, -345.553158653373674175, -606.114272127071156582, -44.8309738736410453262, 821.155170837587098875, -357.60615000000000463};
std::vector<long double> nonreals = {0, -56.7987997464240911907, -88.4598663671046795617, 114.599317038851535089, 59.8147392880918194191, -318.732530313384460674, 25.4838243897183948702, 298.856932852787649163, -52.7828546196374414311, 639.085076012777270449, -725.060920075901748472, -217.793354878621167464, -50.8793220693156938196, -81.234573565751501345, -312.271354760117160837, 13.609817631546604403, -225.700304988404179207, 267.968070726490371763, -138.317628623756120776, 724.441385521062682362, 358.256926195154155268, 367.869822138613221829, -379.416274255357369588, -378.675193748765464757, -204.906665100902654453, 4.48782537655248046241e-13};

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

        ASSERT_DOUBLE_EQ(reals.at(i), real.at(i));

        // Check non-real part:

        ASSERT_DOUBLE_EQ(nonreals.at(i), nonreal.at(i));
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
