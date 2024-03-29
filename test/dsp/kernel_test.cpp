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

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <vector>

#include "dsp/kernel.hpp"
#include "const.hpp"

// Define some constants for testing:

const double freq = 7938.;
const int size = 115;

// Known test data:

std::vector<long double> kern_data = {-7.73459746e-20, 7.49472265e-07,  -3.73641912e-06, -1.43871143e-05,
    -6.64706090e-06, 3.31082595e-05,  5.77308258e-05,  -1.97809012e-19,
    -1.09794530e-04, -1.22727965e-04, 5.08024250e-05,  2.52598660e-04,
    1.87383024e-04,  -1.87971945e-04, -4.71447844e-04, -2.08556390e-04,
    4.61313054e-04,  7.56835777e-04,  1.17014840e-04,  -9.19435071e-04,
    -1.06671118e-03, 1.82817703e-04,  1.59454920e-03,  1.31379989e-03,
    -8.05683120e-04, -2.48213879e-03, -1.35708001e-03, 1.86974025e-03,
    3.51857415e-03,  1.00052779e-03,  -3.47346191e-03, -4.56056169e-03,
    6.19906368e-18,  5.66892154e-03,  5.36926694e-03,  -1.92500325e-03,
    -8.43715255e-03, -5.59785467e-03, 5.08466261e-03,  1.16719547e-02,
    4.77145110e-03,  -9.83980304e-03, -1.51775947e-02, -2.22440809e-03,
    1.67051997e-02,  1.86833601e-02,  -3.11520405e-03, -2.67033425e-02,
    -2.18743747e-02, 1.35199106e-02,  4.26868863e-02,  2.44343023e-02,
    -3.62703272e-02, -7.66238712e-02, -2.60925006e-02, 1.22021407e-01,
    2.87656897e-01,  3.60000024e-01,  2.87656897e-01,  1.22021407e-01,
    -2.60925006e-02, -7.66238712e-02, -3.62703272e-02, 2.44343023e-02,
    4.26868863e-02,  1.35199106e-02,  -2.18743747e-02, -2.67033425e-02,
    -3.11520405e-03, 1.86833601e-02,  1.67051997e-02,  -2.22440809e-03,
    -1.51775947e-02, -9.83980304e-03, 4.77145110e-03,  1.16719547e-02,
    5.08466261e-03,  -5.59785467e-03, -8.43715255e-03, -1.92500325e-03,
    5.36926694e-03,  5.66892154e-03,  6.19906368e-18,  -4.56056169e-03,
    -3.47346191e-03, 1.00052779e-03,  3.51857415e-03,  1.86974025e-03,
    -1.35708001e-03, -2.48213879e-03, -8.05683120e-04, 1.31379989e-03,
    1.59454920e-03,  1.82817703e-04,  -1.06671118e-03, -9.19435071e-04,
    1.17014840e-04,  7.56835777e-04,  4.61313054e-04,  -2.08556390e-04,
    -4.71447844e-04, -1.87971945e-04, 1.87383024e-04,  2.52598660e-04,
    5.08024250e-05,  -1.22727965e-04, -1.09794530e-04, -1.97809012e-19,
    5.77308258e-05,  3.31082595e-05,  -6.64706090e-06, -1.43871143e-05,
    -3.73641912e-06, 7.49472265e-07,  -7.73459746e-20};

TEST_CASE("Kernel Test", "[kern]") {

    SECTION("Sinc", "Ensures the Sinc kernel is correct") {

        // Create vector to hold results:

        std::vector<long double> out(size);

        // Call sinc function:

        sinc_kernel(freq / SAMPLE_RATE, size, out.begin());

        // Ensure output values are accurate:

        for (int i = 0; i < size; ++i) {

            // Compare values:

            REQUIRE_THAT(out.at(i), Catch::Matchers::WithinAbs(kern_data.at(i), 0.0001));
        }
    }
}
