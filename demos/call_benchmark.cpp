/**
 * @file call_benchmark.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Testing multiple function calls vs one vectorized call
 * @version 0.1
 * @date 2024-05-05
 *
 * @copyright Copyright (c) 2024
 *
 * This benchmark measures the speed of certain approaches to calling.
 * We test the following:
 *
 * - Normal function multiple calls with one value returned
 * - Normal function call with vector returned
 * - Function pointer multiple calls with one value returned
 * - Fuction pointer call with vector returned
 * - Application using maec components!
 */

#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

#include "meta_audio.hpp"
#include "sink_module.hpp"

/// Define vector return type
using VectorPointer = std::unique_ptr<std::vector<double>>;

/// Define single function pointer type
using SingleFuncPoint = std::function<double()>;

/// Define vector function pointer type
using VectorFuncPoint = std::function<VectorPointer(int)>;

double single_return() {

    // Just return a single value:

    return 1.0;
}

VectorPointer vector_return(int size) {

    // Create a vector:

    VectorPointer vect = std::make_unique<std::vector<double>>(size);

    // Fill the vector:

    std::fill_n(vect->begin(), size, 1.0);

    // Return the vector:

    return vect;
}

int main() {

    // Define parameters:

    const int iter = 2000;  // Number of iterations for each section
    const int num = 10000;  // Number of values to generate

    std::cout << "+====================================+" << '\n';
    std::cout << " !Benchmarking function performance!" << '\n';
    std::cout << "+====================================+" << '\n';

    // Determine if our clock is steady:

    if (!std::chrono::high_resolution_clock::is_steady) {

        std::cout << "Warning: high_resolution_clock is not steady!" << '\n';
        std::cout << "This may cause inaccurate results." << std::endl;
        std::cout << "+====================================+" << std::endl;
    }

    //
    // Normal Single Function
    //

    std::cout << " --== [ Testing Normal Single Call ] ==--" << "\n";

    double single_norm = 0;

    for (int i = 0; i < iter; ++i) {

        double val = 0;

        // Start the clock:

        auto start = std::chrono::high_resolution_clock::now();

        // Call the function many times:

        for (int j = 0; j < num; ++j) {

            // Grab this value:

            val = single_return();
        }

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        val++;

        // Calculate the time:

        auto diff = end - start;

        auto time_ms = std::chrono::duration<double, std::milli>(diff).count();

        std::cout << "Normal Single Time [" << i << "]: " << time_ms << " ms"
                  << "\n";

        single_norm += time_ms;
    }

    //
    // Normal Vector Function
    //

    std::cout << "--== [ Testing Normal Vector Call ] ==--" << "\n";

    double vector_norm = 0;

    for (int i = 0; i < iter; ++i) {

        VectorPointer vpoint = nullptr;

        // Start the clock:

        auto start = std::chrono::high_resolution_clock::now();

        // Call the function:

        vpoint = vector_return(num);

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        // Calculate the time:

        auto diff = end - start;

        auto time_ms = std::chrono::duration<double, std::milli>(diff).count();

        std::cout << "Normal Vector Time [" << i << "]: " << time_ms << " ms"
                  << "\n";

        vector_norm += time_ms;
    }

    //
    // Pointer Single Function
    //

    std::cout << "--== [ Testing Pointer Single Call ] ==--" << "\n";

    double single_point = 0;

    SingleFuncPoint spointf = &single_return;

    for (int i = 0; i < iter; ++i) {

        double val = 0;

        // Start the clock:

        auto start = std::chrono::high_resolution_clock::now();

        // Call the function many times:

        for (int j = 0; j < num; ++j) {

            // Grab this value:

            val = spointf();
        }

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        val++;

        // Calculate the time:

        auto diff = end - start;

        auto time_ms = std::chrono::duration<double, std::milli>(diff).count();

        std::cout << "Point Single Time [" << i << "]: " << time_ms << " ms"
                  << "\n";

        single_point += time_ms;
    }

    //
    // Pointer Vector Function
    //

    std::cout << "--== [ Testing Pointer Vector Call ] ==--" << "\n";

    double vector_point = 0;

    VectorFuncPoint vpointf = &vector_return;

    for (int i = 0; i < iter; ++i) {

        VectorPointer vpoint = nullptr;

        // Start the clock:

        auto start = std::chrono::high_resolution_clock::now();

        // Call the function:

        vpoint = vpointf(num);

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        // Calculate the time:

        auto diff = end - start;

        auto time_ms = std::chrono::duration<double, std::milli>(diff).count();

        std::cout << "Point Vector Time [" << i << "]: " << time_ms << " ms"
                  << "\n";

        vector_point += time_ms;
    }

    //
    // maec components
    //

    std::cout << "--== [ Testing MAEC Calls ] ==--" << "\n";

    double maec_call = 0;

    // Create the modules:

    SinkModule sink;
    ConstModule osc(1.0);

    // Link them:

    sink.link(&osc);

    for (int i = 0; i < iter; ++i) {

        // Start the clock:

        auto start = std::chrono::high_resolution_clock::now();

        // Process the chain:

        sink.meta_process();

        auto buff = sink.get_buffer();

        // Stop the clock:

        auto end = std::chrono::high_resolution_clock::now();

        buff.at(0) = 0;

        // Calculate the time:

        auto diff = end - start;

        auto time_ms = std::chrono::duration<double, std::milli>(diff).count();

        std::cout << "maec Call Time [" << i << "]: " << time_ms << " ms"
                  << "\n";

        maec_call += time_ms;
    }

    //
    // Results
    //

    std::cout << "+=======================================+" << "\n";
    std::cout << "--== [ Results ] ==--" << "\n";

    std::cout << "Average Normal Single Time: " << single_norm / iter << "\n";
    std::cout << "Average Normal Vector Time: " << vector_norm / iter << "\n";
    std::cout << "Average Pointer Single Time: " << single_point / iter << "\n";
    std::cout << "Average Pointer Vector Time: " << vector_point / iter << "\n";
    std::cout << "Average maec call: " << maec_call / iter << "\n";
}
