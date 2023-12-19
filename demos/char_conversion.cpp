/**
 * @file char_conversion.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Testing different char conversion types
 * @version 0.1
 * @date 2023-11-28
 * 
 * @copyright Copyright (c) 2023
 * 
 * This file will test many different methods for converting
 * an array of chars into something more complicated
 * (in this case, char->short->double).
 * This will be utilized to help determine the ideal framework for such operations.
 */

#include <array>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <iostream>

using random_bytes_engine =
    std::independent_bits_engine<std::default_random_engine, 8, unsigned char>;

int main() {

    // Number of iterations:

    const int iter = 100;

    // Create initial array.
    // The size is the number of output values there will be

    const int size = 400;
    std::array<char, size*2> data = {};

    // Fill with random chars:

    random_bytes_engine rbe;
    std::generate_n(data.begin(), size*2, rbe);
    std::generate_n(data.begin(), size*2, rbe);

    // From here, we need to test our methods.
    // We assume the vector we are copying to is optimal

    std::vector<long double> odata(size);

    std::cout << "+=========================================+" << std::endl;
    std::cout << " !Benchmarking char conversion performance!" << std::endl;
    std::cout << "+=========================================+" << std::endl;

    // Determine if our clock is steady:

    if (!std::chrono::high_resolution_clock::is_steady) {

        std::cout << "Warning: high_resolution_clock is not steady!"
                  << std::endl;
        std::cout << "This may cause inaccurate results." << std::endl;
        std::cout << "+=======================================+" << std::endl;
    }

    long double acopy = 0;
    long double scopy = 0;
    long double sscopy = 0;

    //
    // Iterate over all methods
    //

    for (int j = 0; j < iter; ++j) {

        //
        // Read data into vector via reinterpret_cast
        //

        std::cout << "Testing all in one copy..." << std::endl;

        auto start = std::chrono::high_resolution_clock::now();

        // Create a vector for intermediate use:

        std::vector<std::int16_t> idata(size);

        // First, convert into shorts:

        std::copy_n(data.begin(), size*2, reinterpret_cast<char*>(idata.data()));

        // Next, convert into long doubles:

        for (int i = 0; i < idata.size(); ++i) {

            // Convert into double and save:

            odata[i] = idata[i] / 32768.0;
        }

        auto end = std::chrono::high_resolution_clock::now();

        // Calculate the time:

        auto diff = end - start;

        // Print the time:

        std::cout << "All in one copy time [" << j << "]: "
                  << std::chrono::duration<double, std::milli>(diff).count()
                  << " ms" << std::endl;

        // Add to the total:

        acopy += std::chrono::duration<double, std::milli>(diff).count();

        //
        // Copy values one at a time via reinterpret_cast
        //

        start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < size; ++i) {

            // Define our type:

            int16_t val = 0;

            // Copy values over:

            std::copy_n(data.begin() + i*2, 2, reinterpret_cast<char*>(&val));

            // Add value to final vector:

            //odata.at(i) = static_cast<long double>(val) / 32768.0;
        }

        end = std::chrono::high_resolution_clock::now();

        // Calculate the time:

        diff = end - start;

        // Print the time:

        std::cout << "Single copy time [" << j << "]: "
                  << std::chrono::duration<double, std::milli>(diff).count()
                  << " ms" << std::endl;

        // Add to the total:

        scopy += std::chrono::duration<double, std::milli>(diff).count();

        //
        // Copy values one at a time safely
        //

        start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < size; ++i) {

            // Define our type:

            int16_t val = 0;

            // Copy values over:

            val = static_cast<int16_t>((static_cast<int16_t>(data[i * 2+1]) << 8) | static_cast<unsigned char>(data[i*2]));

            // Add value to final vector:

            odata.at(i) = static_cast<long double>(val) / 32768.0;
        }

        end = std::chrono::high_resolution_clock::now();

        // Print the time:

        std::cout << "Single safe copy time [" << j << "]: "
                  << std::chrono::duration<double, std::milli>(diff).count()
                  << " ms" << std::endl;

        // Add to the total:

        sscopy += std::chrono::duration<double, std::milli>(diff).count();
    }

    //
    // Output Results
    //

    std::cout << "+=======================================+" << std::endl;
    std::cout << " -== [ Results: ] ==--" << std::endl;

    std::cout << "Average all in one copy time: " << acopy / iter << " ms" << std::endl;
    std::cout << "Average single copy time: " << scopy / iter << " ms" << std::endl;
    std::cout << "Average single safe copy time: " << sscopy / iter << " ms" << std::endl;
}
