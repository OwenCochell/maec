/**
 * @file container_benchmark.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief This file contains benchmarks for container types
 * @version 0.1
 * @date 2022-09-14
 *
 * @copyright Copyright (c) 2022
 *
 * These benchmarks are designed to test the performance of various containers,
 * in a way that may be similar to how they are used in the project.
 * This benchmark is poorly designed and the code is a mess, but it works.
 *
 * Key takeaways:
 *
 * std::array is the fastest container type (in scenarios similar to this benchmark) 
 * by a large amount, around 100% (!) faster for writing and 5-10% faster for reading
 *
 * In a perfect world, we would use std::array, but we can't
 * as our buffer sizes are variable and we can't guarantee the size to the
 * compiler. The next best thing (while utilizing the standard library) is
 * std::vector preallocated.
 *
 * Believe it or not, dynamic arrays have similar performance to the
 * preallocated vector. Because dynamic arrays do not have the features of
 * vector (iterators that allow the use of algorithms), we will be using the
 * preallocated vector.
 */

#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>

#include "dsp/buffer.hpp"

long double percent_diff(long double first, long double second) {

    // Get the diff:

    const long double diff = std::abs(second - first);

    // Determine the percent difference:

    return diff / ((first + second) / 2) *
           100;  // NOLINT(*-magic-numbers): Considering removing this check anyways...
}

int main() {  // NOLINT(*-complexity): Yeah I know this function is complicated

    // Number of values to test:

    const int num = 5000;

    // Number of times to test:

    const int iterations = 10000;

    // Number of channels:

    const int chans = 5;

    // Create a normal vector:

    std::vector<long double> vec;

    // Create vector pre-allocated:

    std::vector<long double> vec2(num);

    // Create array:

    std::array<long double, num> vec4 = {};

    // Create dynamic array:

    auto* vec5 = new long double[num];  // NOLINT(*-owning-memory)

    // Create reserved vector:

    std::vector<long double> vec6;

    std::cout << "+====================================+" << std::endl;
    std::cout << " !Benchmarking container performance!" << std::endl;
    std::cout << "+====================================+" << std::endl;

    // Determine if our clock is steady:

    if (!std::chrono::high_resolution_clock::is_steady) {

        std::cout << "Warning: high_resolution_clock is not steady!"
                  << std::endl;
        std::cout << "This may cause inaccurate results." << std::endl;
        std::cout << "+====================================+" << std::endl;
    }

    // Alright, test the vector:

    long double vect_read = 0;
    long double vect_write = 0;

    std::cout << " --== [ Testing vector write performance... ] ==--"
              << std::endl;

    for (int i = 0; i < iterations; i++) {

        // First, clear and shrink:

        vec.clear();
        vec.shrink_to_fit();

        // Start the clock:

        auto start = std::chrono::high_resolution_clock::now();

        // Add values to the vector:

        for (int j = 0; j < num; j++) {

            vec.push_back(static_cast<long double>(j));
        }

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        // Calculate the time:

        auto diff = end - start;

        // Print the time:

        std::cout << "Vector write time [" << i << "]: "
                  << std::chrono::duration<double, std::milli>(diff).count()
                  << " ms" << std::endl;

        // Add to the total:

        vect_write += std::chrono::duration<double, std::milli>(diff).count();
    }

    // Test the read of the vector:

    std::cout << "+====================================+" << std::endl;
    std::cout << " --== [ Testing vector read performance... ] ==--"
              << std::endl;

    for (int i = 0; i < iterations; i++) {

        long double val = 0;

        // Start the clock:

        auto start = std::chrono::high_resolution_clock::now();

        // Get values from the vector:

        for (int j = 0; j < num; j++) {

            val = vec[j];
        }

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        // Increment the double, just to use it:

        val++;

        // Calculate the time:

        auto diff = end - start;

        // Print the time:

        std::cout << "Vector read time [" << i << "]: "
                  << std::chrono::duration<double, std::milli>(diff).count()
                  << " ms" << std::endl;

        // Add to the total:

        vect_read += std::chrono::duration<double, std::milli>(diff).count();
    }

    // Alright, test the reserved vector:

    long double vectr_read = 0;
    long double vectr_write = 0;

    std::cout << " --== [ Testing reserved vector write performance... ] ==--"
              << std::endl;

    for (int i = 0; i < iterations; i++) {

        // First, clear and shrink:

        vec6.clear();
        vec6.shrink_to_fit();

        // Reserve the vector:

        vec6.reserve(num);

        // Start the clock:

        auto start = std::chrono::high_resolution_clock::now();

        // Add values to the vector:

        for (int j = 0; j < num; j++) {

            vec6.push_back(static_cast<long double>(j));
        }

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        // Calculate the time:

        auto diff = end - start;

        // Print the time:

        std::cout << "Reserved vector write time [" << i << "]: "
                  << std::chrono::duration<double, std::milli>(diff).count()
                  << " ms" << std::endl;

        // Add to the total:

        vectr_write += std::chrono::duration<double, std::milli>(diff).count();
    }

    // Test the read of the vector:

    std::cout << "+====================================+" << std::endl;
    std::cout << " --== [ Testing reserved vector read performance... ] ==--"
              << std::endl;

    for (int i = 0; i < iterations; i++) {

        // Start the clock:

        long double val = 0;

        auto start = std::chrono::high_resolution_clock::now();

        // Add values to the vector:

        for (int j = 0; j < num; j++) {

            val = vec6[j];
        }

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        // Increment the double, just to use it:

        val++;

        // Calculate the time:

        auto diff = end - start;

        // Print the time:

        std::cout << "Reserved vector read time [" << i << "]: "
                  << std::chrono::duration<double, std::milli>(diff).count()
                  << " ms" << std::endl;

        // Add to the total:

        vectr_read += std::chrono::duration<double, std::milli>(diff).count();
    }

    // Alright, test the pre allocated vector:

    long double vect2_read = 0;
    long double vect2_write = 0;

    std::cout << "+====================================+" << std::endl;
    std::cout
        << " --== [ Testing pre-allocated vector write performance... ] ==--"
        << std::endl;

    for (int i = 0; i < iterations; i++) {

        // Start the clock:

        auto start = std::chrono::high_resolution_clock::now();

        // Add values to the vector:

        for (int j = 0; j < num; j++) {

            vec2[j] = static_cast<long double>(j); // SLOWEST!
        }

        //std::fill_n(tvec.begin(), num, 0);  FASTEST!

        // for (auto iter = tvec.begin(); iter < tvec.end(); ++iter) {  // SECOND-SLOWEST!

        //     *iter = 0;
        // }

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        // Calculate the time:

        auto diff = end - start;

        // Print the time:

        std::cout << "Prealloc-Vector write time [" << i << "]: "
                  << std::chrono::duration<double, std::milli>(diff).count()
                  << " ms" << std::endl;

        // Add to the total:

        vect2_write += std::chrono::duration<double, std::milli>(diff).count();
    }

    // Alright, test the pre allocated vector:

    long double vect2f_write = 0;

    std::cout << "+====================================+" << std::endl;
    std::cout
        << " --== [ Testing pre-allocated vector assignment write performance... ] ==--"
        << std::endl;

    for (int i = 0; i < iterations; i++) {

        // Start the clock:

        auto start = std::chrono::high_resolution_clock::now();

        // Add values to the vector:

        std::fill_n(vec2.begin(), num, 0);

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        // Calculate the time:

        auto diff = end - start;

        // Print the time:

        std::cout << "Prealloc-Vector fill write time [" << i << "]: "
                  << std::chrono::duration<double, std::milli>(diff).count()
                  << " ms" << std::endl;

        // Add to the total:

        vect2f_write += std::chrono::duration<double, std::milli>(diff).count();
    }

    // Alright, test the pre allocated vector via manual iterators:

    long double vect2i_write = 0;

    std::cout << "+====================================+" << std::endl;
    std::cout
        << " --== [ Testing pre-allocated vector write performance... ] ==--"
        << std::endl;

    for (int i = 0; i < iterations; i++) {

        // Start the clock:

        auto start = std::chrono::high_resolution_clock::now();

        // Add values to the vector:

        for (auto iter = vec2.begin(); iter < vec2.end(); ++iter) {  //

            *iter = 0;
        }

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        // Calculate the time:

        auto diff = end - start;

        // Print the time:

        std::cout << "Prealloc-Vector iterator write time [" << i << "]: "
                  << std::chrono::duration<double, std::milli>(diff).count()
                  << " ms" << std::endl;

        // Add to the total:

        vect2i_write += std::chrono::duration<double, std::milli>(diff).count();
    }

    // Test the read of the prealloc-vector:

    std::cout << "+====================================+" << std::endl;
    std::cout
        << " --== [ Testing pre-allocated vector read performance... ] ==--"
        << std::endl;

    for (int i = 0; i < iterations; i++) {

        long double val = 0;

        // Start the clock:

        auto start = std::chrono::high_resolution_clock::now();

        // Add values to the vector:

        for (int j = 0; j < num; j++) {

            //val = vec2[j];
        }

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        // Increment the double, just to use it:

        val++;

        // Calculate the time:

        auto diff = end - start;

        // Print the time:

        std::cout << "Prealloc-Vector read time [" << i << "]: "
                  << std::chrono::duration<double, std::milli>(diff).count()
                  << " ms" << std::endl;

        // Add to the total:

        vect2_read += std::chrono::duration<double, std::milli>(diff).count();
    }

    // Test the read of the array:

    long double vect4_read = 0;
    long double vect4_write = 0;

    std::cout << "+====================================+" << std::endl;
    std::cout << " --== [ testing array write performance... ] ==--"
              << std::endl;

    for (int i = 0; i < iterations; i++) {

        // Start the clock:

        auto start = std::chrono::high_resolution_clock::now();

        // Add values to the vector:

        for (int j = 0; j < vec4.size(); j++) {

            vec4[j] = static_cast<long double>(j);  // NOLINT: Not super worried about safe memory usage
        }

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        // Calculate the time:

        auto diff = end - start;

        // Print the time:

        std::cout << "Array write time [" << i << "]: "
                  << std::chrono::duration<double, std::milli>(diff).count()
                  << " ms" << std::endl;

        // Add to the total:

        vect4_write += std::chrono::duration<double, std::milli>(diff).count();
    }

    std::cout << "+====================================+" << std::endl;
    std::cout << " --== [ Testing array read performance... ] ==--"
              << std::endl;

    for (int i = 0; i < iterations; i++) {

        long double val = 0;

        // Start the clock:

        auto start = std::chrono::high_resolution_clock::now();

        // Add values to the vector:

        for (int j = 0; j < num; j++) {

            val = vec4[j];  // NOLINT: Not sure worried about safe memory usage
        }

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        // increment the double, just to use it:

        val++;

        // Calculate the time:

        auto diff = end - start;

        // Print the time:

        std::cout << "Array read time [" << i << "]: "
                  << std::chrono::duration<double, std::milli>(diff).count()
                  << " ms" << std::endl;

        // Add to the total:

        vect4_read += std::chrono::duration<double, std::milli>(diff).count();
    }

    // Test the read of the array:

    long double vect5_read = 0;
    long double vect5_write = 0;

    std::cout << "+====================================+" << std::endl;
    std::cout << " --== [ testing dynamic array write performance... ] ==--"
              << std::endl;

    for (int i = 0; i < iterations; i++) {

        // Start the clock:

        auto start = std::chrono::high_resolution_clock::now();

        // Add values to the vector:

        for (int j = 0; j < num; j++) {

            vec5[j] = static_cast<long double>(j);  // NOLINT: Pointer arithmetic is necessary here
        }

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        // Calculate the time:

        auto diff = end - start;

        // Print the time:

        std::cout << "Dynamic Array write time [" << i << "]: "
                  << std::chrono::duration<double, std::milli>(diff).count()
                  << " ms" << std::endl;

        // Add to the total:

        vect5_write += std::chrono::duration<double, std::milli>(diff).count();
    }

    std::cout << "+====================================+" << std::endl;
    std::cout << " --== [ Testing dynamic array read performance... ] ==--"
              << std::endl;

    for (int i = 0; i < iterations; i++) {

        long double val = 0;

        // Start the clock:

        auto start = std::chrono::high_resolution_clock::now();

        // Add values to the vector:

        for (int j = 0; j < num; j++) {

            val = vec5[j];  // NOLINT: Pointer arithmetic is necessary here
        }

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        // Increment the double, just to use it:

        val++;

        // Calculate the time:

        auto diff = end - start;

        // Print the time:

        std::cout << "Dynamic Array read time [" << i << "]: "
                  << std::chrono::duration<double, std::milli>(diff).count()
                  << " ms" << std::endl;

        // Add to the total:

        vect5_read += std::chrono::duration<double, std::milli>(diff).count();
    }

    // Test the read of the array:

    long double maec_readi = 0;
    long double maec_writei = 0;

    long double maec_reads = 0;
    long double maec_writes = 0;

    // Create the buffer to utilize
    // (pre-allocated)

    Buffer<long double> buffer(num / chans, chans);

    std::cout << "+====================================+" << std::endl;
    std::cout << " --== [ testing maec buffer interleaved write performance... ] ==--"
              << std::endl;

    for (int i = 0; i < iterations; i++) {

        // Start the clock:

        auto start = std::chrono::high_resolution_clock::now();

        // Add values to the vector:

        for (auto iter = buffer.begin(); iter < buffer.end(); ++iter) {

            *iter = 1;
        }

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        // Calculate the time:

        auto diff = end - start;

        // Print the time:

        std::cout << "maec Array interleaved write time [" << i << "]: "
                  << std::chrono::duration<double, std::milli>(diff).count()
                  << " ms" << std::endl;

        // Add to the total:

        maec_writei += std::chrono::duration<double, std::milli>(diff).count();
    }

    std::cout << "+====================================+" << std::endl;
    std::cout << " --== [ Testing maec buffer interleaved read performance... ] ==--"
              << std::endl;

    for (int i = 0; i < iterations; i++) {

        long double val = 0;

        // Start the clock:

        auto start = std::chrono::high_resolution_clock::now();

        // Add values to the vector:

        for (auto iter = buffer.begin(); iter < buffer.end(); ++iter) {

            val = *iter;
        }

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        // Increment the double, just to use it:

        val++;

        // Calculate the time:

        auto diff = end - start;

        // Print the time:

        std::cout << "maec buffer interleaved read time [" << i << "]: "
                  << std::chrono::duration<double, std::milli>(diff).count()
                  << " ms" << std::endl;

        // Add to the total:

        maec_readi += std::chrono::duration<double, std::milli>(diff).count();
    }

    std::cout << "+====================================+" << std::endl;
    std::cout
        << " --== [ testing maec buffer sequential write performance... ] ==--"
        << std::endl;

    for (int i = 0; i < iterations; i++) {

        // Start the clock:

        auto start = std::chrono::high_resolution_clock::now();

        // Add values to the vector:

        for (auto iter = buffer.sbegin(); iter < buffer.send(); ++iter) {

            *iter = 1;
        }

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        // Calculate the time:

        auto diff = end - start;

        // Print the time:

        std::cout << "maec buffer sequential write time [" << i << "]: "
                  << std::chrono::duration<double, std::milli>(diff).count()
                  << " ms" << std::endl;

        // Add to the total:

        maec_writes += std::chrono::duration<double, std::milli>(diff).count();
    }

    std::cout << "+====================================+" << std::endl;
    std::cout
        << " --== [ Testing maec buffer sequential read performance... ] ==--"
        << std::endl;

    for (int i = 0; i < iterations; i++) {

        long double val = 0;

        // Start the clock:

        auto start = std::chrono::high_resolution_clock::now();

        // Add values to the vector:

        for (auto iter = buffer.sbegin(); iter < buffer.send(); ++iter) {

            val = *iter;
        }

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        // Increment the double, just to use it:

        val++;

        // Calculate the time:

        auto diff = end - start;

        // Print the time:

        std::cout << "maec buffer interleaved read time [" << i << "]: "
                  << std::chrono::duration<double, std::milli>(diff).count()
                  << " ms" << std::endl;

        // Add to the total:

        maec_reads += std::chrono::duration<double, std::milli>(diff).count();
    }

    // Return the results:

    std::cout << "+================================================+"
              << std::endl;
    std::cout << "         --== [ Vector Write Times ] ==--" << std::endl;

    std::cout << "Vector average write time: " << vect_write / iterations
              << " ms" << std::endl;
    std::cout << "Prealloc-Vector average assignment write time: "
              << vect2_write / iterations << " ms" << std::endl;
    std::cout << "Prealloc-Vector average fill write time: "
              << vect2f_write / iterations << " ms" << std::endl;
    std::cout << "Prealloc-Vector average iterator write time: "
              << vect2i_write / iterations << " ms" << std::endl;
    std::cout << "Reserved Vector average write time: "
              << vectr_write / iterations << " ms" << std::endl;
    std::cout << "Array average write time: " << vect4_write / iterations
              << " ms" << std::endl;
    std::cout << "Dynamic array average write time: "
              << vect5_write / iterations << " ms" << std::endl;

    std::cout << "MAEC buffer interleaved write: " << maec_writei / iterations << " ms"
              << "\n";
    std::cout << "MAEC buffer sequential write: " << maec_writes / iterations << " ms"
              << "\n";

    std::cout << "  --== [ Vector Read Times: ] ==--" << std::endl;

    std::cout << "Vector average read time: " << vect_read / iterations << " ms"
              << std::endl;
    std::cout << "Prealloc-Vector average read time: "
              << vect2_read / iterations << " ms" << std::endl;
    std::cout << "Reserved Vector average read time: "
              << vectr_read / iterations << " ms" << std::endl;
    std::cout << "Array average read time: " << vect4_read / iterations << " ms"
              << std::endl;
    std::cout << "Dynamic array average read time: " << vect5_read / iterations
              << " ms" << std::endl;
    std::cout << "MAEC buffer interleaved read time: " << maec_readi / iterations << " ms"
              << "\n";
    std::cout << "MAEC buffer sequential read time: "
              << maec_reads / iterations << " ms"
              << "\n";

    std::cout << "+================================================+" << std::endl;
    std::cout << " --== [ Comparisons ] ==--" << std::endl;
    std::cout << "Array write time is "
              << percent_diff(vect4_write / iterations,
                              vect2_write / iterations)
              << " percent faster than preallocated vector write time."
              << std::endl;
    std::cout << "Array read time is "
              << percent_diff(vect4_read / iterations, vect2_read / iterations)
              << " percent faster than preallocated vector read time."
              << std::endl;

    std::cout << "Prealloc-Vector write time is "
              << percent_diff(vect2_write / iterations, vect_read / iterations)
              << " percent faster than normal vector write time." << std::endl;
    std::cout << "Prealloc-Vector read time is "
              << percent_diff(vect2_read / iterations, vect_read / iterations)
              << " percent faster than normal vector write time." << std::endl;

    std::cout << "Reserved-Vector write time is "
              << percent_diff(vect2_write,
                              vectr_write)
              << " percent faster than prealloc-vector write time."
              << std::endl;
    std::cout << "Reserved-Vector read time is "
              << percent_diff(vect2_read, vectr_read)
              << " percent faster than prealloc-vector vector read time." << std::endl;

    return 0;
}
