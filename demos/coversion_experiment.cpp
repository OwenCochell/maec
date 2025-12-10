/**
 * @file coversion_experiment.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Testing different conversion types
 * @version 0.1
 * @date 2023-01-01
 *
 * @copyright Copyright (c) 2023
 *
 * We test to see which operation yields faster results.
 * We test the dual copy method, as well as the single copy method.
 *
 * This is to answer some questions about these operations.
 * I suspect that the single copy operation will be much faster,
 * but we will see for sure here.
 */

#define _USE_MATH_DEFINES

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>

const double TWO_PI = 2.0 * M_PI;

float conv(double val) { return int16_t(val * 32768.0); }

int main() {

    // Create vector of initial data:

    const int num = 1000;
    const int iter = 1000;

    std::vector<double> vec(num);

    std::vector<int> dest1(num);
    std::vector<double> dest2(num);
    std::vector<int> dest3(num);

    for (int i = 0; i < num; i++) {

        // Add the value:

        vec[i] = sin(TWO_PI * 440 * i / 44100);
    }

    std::cout << "+====================================+" << std::endl;
    std::cout << " !Benchmarking copy performance!" << std::endl;
    std::cout << "+====================================+" << std::endl;

    // Determine if our clock is steady:

    if (!std::chrono::high_resolution_clock::is_steady) {

        std::cout << "Warning: high_resolution_clock is not steady!"
                  << std::endl;
        std::cout << "This may cause inaccurate results." << std::endl;
        std::cout << "+====================================+" << std::endl;
    }

    std::cout << " --== [ Testing single copy performance... ] ==--"
              << std::endl;

    long double stotal = 0;

    for (int i = 0; i < iter; ++i) {

        // Start the clock:

        auto start = std::chrono::high_resolution_clock::now();

        std::transform(vec.begin(), vec.end(), dest1.begin(), conv);

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        // Calculate the time:

        auto diff = end - start;

        // Print the time:

        std::cout << "Single copy time [" << i << "]: "
                  << std::chrono::duration<double, std::milli>(diff).count()
                  << " ms" << std::endl;

        stotal += std::chrono::duration<double, std::milli>(diff).count();
    }

    std::cout << " --== [ Testing dual copy performance... ] ==--" << std::endl;

    long double dtotal = 0;

    for (int i = 0; i < iter; ++i) {

        // Start the clock:

        auto startd = std::chrono::high_resolution_clock::now();

        std::copy(vec.begin(), vec.end(), dest2.begin());
        std::transform(dest2.begin(), dest2.end(), dest3.begin(), conv);

        // Stop the clock:

        auto endd = std::chrono::high_resolution_clock::now();

        // Calculate the time:

        auto diffd = endd - startd;

        // Print the time:

        std::cout << "Dual copy time [" << i << "]: "
                  << std::chrono::duration<double, std::milli>(diffd).count()
                  << " ms" << std::endl;

        dtotal += std::chrono::duration<double, std::milli>(diffd).count();
    }

    std::cout << "Average Single Time: " << stotal / iter << std::endl;
    std::cout << "Average dual time: " << dtotal / iter << std::endl;
}
