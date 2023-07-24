/**
 * @file fft_radix2_benchmark.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Benchmarks the Radix2 FFT
 * @version 0.1
 * @date 2023-06-11
 * 
 * @copyright Copyright (c) 2023
 * 
 * This demo does some benchmarks on the maec
 * FFT Radix2 algorithms.
 * We also test some variations on the Radix2 FFT,
 * which are compared against the default maec FFT implementations.
 * 
*/

#include <chrono>
#include <iostream>
#include <ctime>
#include <random>
#include <complex>
#include <algorithm>

#include "dsp/ft.hpp"
#include "dsp/util.hpp"

/// Number of times to repeat the benchmark
const int repeat = 500;

/// Number of data points to generate and compute
const int num = 1000;

/**
 * @brief Generates N complex random number
 * 
 * The real and imaginary parts of the complex number
 * will be -1 < p < 1.
 * 
 * @param size Number of complex numbers to generate
 * @param out Iterator to output array
 */
void rand_complex(int size, std::vector<std::complex<long double>>::iterator out) {

    // Create a random number generator:

    std::default_random_engine rand;

    // Seed random number generator:

    rand.seed(time(nullptr));

    // Iterate over size of data to create:

    for (int i = 0; i < size; ++i) {

        // Create real value:

        auto real = static_cast<long double>((static_cast<long double>(rand()) - (std::default_random_engine::max() / 4.0)) / std::default_random_engine::max());

        // Create imaginary value:

        auto nonreal = static_cast<long double>((static_cast<long double>(rand()) - (std::default_random_engine::max() / 4.0)) / std::default_random_engine::max());
    
        // Set complex value to current iterator:

        *(out + i) = std::complex<long double>(real, nonreal);
    }
}

/**
 * @brief Compares two complex numbers.
 * 
 * We compare the real and imaginary
 * parts of the two given complex numbers.
 * 
 * @param first First complex number
 * @param second Second complex number
 * @return bool true if the same (or near), false if not
 */
bool compare_complex(std::complex<long double> first, std::complex<long double> second) {

    // Define epsilon:

    long double epsilon = 0.0001;

    // Compare real part:

    bool real_near = std::fabs(first.real() - second.real()) < epsilon;

    // Compare imaginary parts:

    bool imaginary_real = std::fabs(first.imag() - second.imag()) < epsilon;

    // Finally, return value:

    return real_near && imaginary_real;
}

/**
 * @brief Ensures the alternative function output matches output from known correct function
 * 
 * We generate a lot of random data and run it through the alternate FFT Radix2 function,
 * and ensures that it matches the output of the FFT Radix2 function.
 * 
 * @return bool true if pass, false if not
 */
bool check_accuracy() {

    std::cout << "+===================================+" << std::endl;
    std::cout << " !Testing Alternate Radix2 Accuracy!" << std::endl;
    std::cout << "+===================================+" << std::endl;

    bool pass = true;

    // Iterate a number of times:

    for(int i = 0; i < repeat; ++i) {

        // First, create a vector of random complex data:

        std::vector<std::complex<long double>> idata(num);
        std::vector<std::complex<long double>> odata(num);
        std::vector<std::complex<long double>> aodata(num);

        // Generate random complex data:

        rand_complex(num, idata.begin());

        // Copy data to new vector:

        std::copy_n(idata.begin(), num, aodata.begin());

        // Run through Radix2:

        fft_c_radix2(idata.begin(), num, odata.begin());

        // Run through alternate Radix2:

        fft_c_radix2(aodata.begin(), num);

        // Do bit reversal:

        bit_reverse(num, aodata.begin());

        // Iterate over results:

        bool iter_pass = true;

        for (int j = 0; j < num; ++j) {

            // Compare current values:

            std::cout << "Iteration: [" << i << "] - Value: [" << j << "]" << std::endl;

            bool val = compare_complex(odata.at(j), aodata.at(j));

            if (!val) {

                // We did not pass, break and alert:

                iter_pass = false;

                std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                std::cout << "Value comparison failed:" << std::endl;
                std::cout << odata.at(j) << std::endl;
                std::cout << aodata.at(j) << std::endl;
                std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;

                break;
            }
        }

        // Determine if any values failed:

        if (!iter_pass) {

            // Did not pass, break and alert:

            pass = false;

            break;
        }
    }

    // Finally, return pass value:

    return pass;
}

/**
 * @brief Benchmarks the out-of-place FFT Radix2 algorithm
 * 
 * @return long double Average computation time
 */
long double benchmark_radix2_out() {

    std::cout << "+=================================+" << std::endl;
    std::cout << " !Benchmarking Alternative Radix2!" << std::endl;
    std::cout << "+=================================+" << std::endl;

    // Determine if our clock is steady:

    if (!std::chrono::high_resolution_clock::is_steady) {

        std::cout << "Warning: high_resolution_clock is not steady!" << std::endl;
        std::cout << "This may cause inaccurate results." << std::endl;
        std::cout << "+=================================+" << std::endl;

    }

    // Define some values:

    long double total_time = 0;

    // Ok, iterate a number of times:

    for(int i = 0; i < repeat; ++i) {

        // First, create a vector of random complex data:

        std::vector<std::complex<long double>> idata(num);

        // Generate random complex data:

        rand_complex(num, idata.begin());

        // Start the clock:
    
        auto start = std::chrono::high_resolution_clock::now();

        // Compute the value:

        fft_c_radix2(idata.begin(), num);

        // Do bit reversal:

        bit_reverse(num, idata.begin());

        // Stop the clock:

        auto stop = std::chrono::high_resolution_clock::now();

        // Calculate the time:
    
        auto diff = stop - start;

        // Print the time:
    
        std::cout << "FFT Radix2-Out Time [" << i << "]: " << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;

        // Add to the total:

        total_time += std::chrono::duration <double, std::milli> (diff).count();
    }

    // Output some stats:

    std::cout << "Total Radix2-Alt time: " << total_time << " ms" << std::endl;
    std::cout << "Average Radix2-Alt time: " << total_time / repeat << " ms" << std::endl;

    // Finally, return average time:

    return total_time / repeat;
}

/**
 * @brief Benchmarks the FFT Radix2 algorithm
 * 
 * @return long double Average computation time
 */
long double benchmark_radix2() {

    std::cout << "+=================================+" << std::endl;
    std::cout << "      !Benchmarking Radix2!" << std::endl;
    std::cout << "+=================================+" << std::endl;

    // Determine if our clock is steady:

    if (!std::chrono::high_resolution_clock::is_steady) {

        std::cout << "Warning: high_resolution_clock is not steady!" << std::endl;
        std::cout << "This may cause inaccurate results." << std::endl;
        std::cout << "+=================================+" << std::endl;

    }

    // Define some values:

    long double total_time = 0;

    // Ok, iterate a number of times:

    for(int i = 0; i < repeat; ++i) {

        // First, create a vector of random complex data:

        std::vector<std::complex<long double>> idata;
        std::vector<std::complex<long double>> odata;

        // Reserve data:

        idata.reserve(num);
        odata.reserve(num);

        // Generate random complex data:

        rand_complex(num, idata.begin());

        // Start the clock:
    
        auto start = std::chrono::high_resolution_clock::now();

        // Compute the value:

        fft_c_radix2(idata.begin(), num, odata.begin());

        // Stop the clock:

        auto stop = std::chrono::high_resolution_clock::now();

        // Calculate the time:
    
        auto diff = stop - start;

        // Print the time:
    
        std::cout << "FFT Radix2 Time [" << i << "]: " << std::chrono::duration <double, std::milli> (diff).count() << " ms" << std::endl;

        // Add to the total:

        total_time += std::chrono::duration <double, std::milli> (diff).count();
    }

    // Output some stats:

    std::cout << "Total Radix2-Alt time: " << total_time << " ms" << std::endl;
    std::cout << "Average Radix2-Alt time: " << total_time / repeat << " ms" << std::endl;

    // Finally, return average time:

    return total_time / repeat;
}

int main() {

    // First, check outputs:

    bool acc = check_accuracy();

    // Check if we are accurate:

    if (!acc) {

        std::cout << "Not Accurate! See above" << std::endl;

        //return 1;

    }

    // Run benchmark for Radix2-alt:

    long double alt_avg = benchmark_radix2_out();

    // Run benchmark for Radix2:

    long double avg = benchmark_radix2();

    // Output some data:

    std::cout << "+========================================+" << std::endl;
    std::cout << "    --==[ Average Output Times: ] ==--" << std::endl;

    std::cout << "Radix2 Average is: " << avg << " ms" << std::endl;
    std::cout << "Radix2-Alt Average is: " << alt_avg << " ms" << std::endl;
}
