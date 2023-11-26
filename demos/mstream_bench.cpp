/**
 * @file mstream_bench.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Benchmarks certain mstream operations
 * @version 0.1
 * @date 2023-11-24
 *
 * @copyright Copyright (c) 2023
 *
 * This benchmark is to compare the time it takes to read one char
 * at a time and process it as we go, vs.
 * loading large chunks and processing those again.
 *
 * The first method has the benefit of doing everything 'in one step',
 * and does not require any intermediate formats,
 * which 100% saves memory and may save time.
 * 
 * The second method has the benefit of minimizing expensive IO system calls,
 * which is generally preferred when it comes to IO operations.
 * The down side is extra time is spend processing the data in an intermediate format.
 */

#include <chrono>
#include <array>
#include <vector>
#include <iostream>
#include <cmath>

#include "io/mstream.hpp"

int main() {

    // Open up a file (THIS ONE!?!?!?!)

    const std::string path = __FILE__;

    // Open fimstream

    FIStream file1;
    FIStream file2;

    // Set the path:

    file1.set_path(path);
    file2.set_path(path);

    long double total1 = 0;
    long double total2 = 0;

    std::cout << "+====================================+" << std::endl;
    std::cout << " !Benchmarking mstream performance!" << std::endl;
    std::cout << "+====================================+" << std::endl;
    std::cout << "Reading file: " << path << std::endl;
    std::cout << "+====================================+" << std::endl;

    // Determine if our clock is steady:

    if (!std::chrono::high_resolution_clock::is_steady) {

        std::cout << "Warning: high_resolution_clock is not steady!"
                  << std::endl;
        std::cout << "This may cause inaccurate results." << std::endl;
        std::cout << "+====================================+" << std::endl;
    }

    // First, do test of single chunking:

    int count1 = 0;

    std::cout << " --== [ Testing single chunk performance... ] ==--"
              << std::endl;

    file1.start();
    std::vector<double> data1;

    while (true) {

        // Determine if we are EOF:

        if (file1.eof()) {

            std::cout << "Reached end of file!" << std::endl;
            // Just quit
            break;
        }

        // Read a single byte:

        auto start = std::chrono::high_resolution_clock::now();

        std::array<char, 1> byt = {};
        file1.read(byt.begin(), 1);

        // Convert char to double:
        data1.push_back(static_cast<double>(byt[0]) / sizeof(char));

        auto stop = std::chrono::high_resolution_clock::now();

        // Calculate the time:

        auto diff = stop - start;

        // Print the time:

        std::cout << "Singe Byte Time: [" << count1 << "]: "
                  << std::chrono::duration<double, std::milli>(diff).count()
                  << " ms" << std::endl;

        // Add to final time:

        total1 += std::chrono::duration<double, std::milli>(diff).count();

        ++count1;
    }

    file1.stop();

    // Now, do arbitrary size chunk checking:

    const int chunk_size = 100;
    int count2 = 0;

    std::cout << " --== [ Testing multi-chunk performance... ] ==--"
              << std::endl;

    file2.start();
    std::vector<double> data2;

    while (true) {

        // Determine if we are EOF:

        if (file2.eof()) {

            // Just quit

            std::cout << "Reached end of file!" << std::endl;

            break;
        }

        // Read a chunk:

        auto start = std::chrono::high_resolution_clock::now();

        //std::array<char, chunk_size> byt = {};

        std::vector<char> byt;
        byt.reserve(chunk_size);

        file2.read(byt.data(), chunk_size);

        // Iterate over data:

        for (const char val : byt) {

            // Convert data:

            data2.push_back(static_cast<double>(val) / sizeof(char));
        }

        auto stop = std::chrono::high_resolution_clock::now();

        // Calculate the time:

        auto diff = stop - start;

        // Print the time:

        std::cout << "Multi[" << chunk_size << "] Chunk Byte Time: [" << count2 << "]: "
                  << std::chrono::duration<double, std::milli>(diff).count()
                  << " ms" << std::endl;

        // Add to final time:

        total2 += std::chrono::duration<double, std::milli>(diff).count();

        ++count2;
    }

    file2.stop();

    // Just for fun, do some experimentation

    std::cout << "+================================================+"
              << std::endl;
    std::cout << "         --== [ mstream Read Times ] ==--" << std::endl;

    std::cout << "Total Single Read: " << total1 << std::endl;
    std::cout << "Total Chunk Read: " << total2 << std::endl;

    auto diff = std::abs(total2 - total1);
    const long double per = diff / ((total1 + total2) / 2) * 100;

    std::cout << "Percent Difference: " << per << std::endl;
}
