/**
 * @file vector_pass.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Tests for various vector passes
 * @version 0.1
 * @date 2024-07-11
 * 
 * @copyright Copyright (c) 2024
 * 
 * This benchmark preforms tests for common vector 'sharing' operations:
 * - Copy - Copys vector content between the two
 * - Pointer - Passes unique pointer between components
 * - Move - Moves vector between values
 * 
 * This test includes creation, read/write, and destruction.
 * Each approach will be tested and timed.
 */

#include <vector>
#include <array>
#include <memory>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <random>

#include "dsp/buffer.hpp"

/// Vector type
using Vect = Buffer<long double>;

/// Vector pointer type
using VecPoint = std::unique_ptr<Vect>;

/// Random device
std::random_device rd;

/// Random engine
std::mt19937 e2(rd());

/// Random distribution
std::uniform_real_distribution<> dist(0, 10);

long double rand_val(long double in) {

    // Generate random value and return:

    return dist(e2);
}

int main() {

    // Define parameters:

    const int size = 10000;  // Size of each vector
    const int num = 50;  // Number of vectors to test
    const int iters = 100;  // Number of repetitions

    std::cout << "+======================================+" << '\n';
    std::cout << " !Benchmarking vector pass performance!" << '\n';
    std::cout << "+======================================+" << '\n';

    // Determine if our clock is steady:

    if (!std::chrono::high_resolution_clock::is_steady) {

        std::cout << "Warning: high_resolution_clock is not steady!" << '\n';
        std::cout << "This may cause inaccurate results." << std::endl;
        std::cout << "+====================================+" << std::endl;
    }

    //
    // Vector Copy
    //

    long double v_con = 0;
    long double v_create = 0;
    long double v_update = 0;
    long double v_destroy = 0;

    std::cout << "+======================================+" << '\n';
    std::cout << " --== [ Vector Copy: ] ==--" << "\n";

    for (int j = 0; j < iters; ++j) {

        // Initialize normal vector:

        auto start = std::chrono::high_resolution_clock::now();

        std::array<Vect, num> v_arr;

        auto stop = std::chrono::high_resolution_clock::now();

        v_con +=
            std::chrono::duration<double, std::milli>(stop - start).count();

        std::cout << "Iter: [" << j << "]\n";

        for (int i = 0; i < num; ++i) {

            // Determine if we need to allocate a new buffer:

            if (i == 0) {

                // Create new buffer:

                auto start = std::chrono::high_resolution_clock::now();

                // Reserve:

                v_arr[0].reserve(size);

                // Add values to vector:

                std::transform(v_arr[0].begin(), v_arr[0].end(), v_arr[0].begin(), rand_val);

                auto stop = std::chrono::high_resolution_clock::now();

                v_create +=
                    std::chrono::duration<double, std::milli>(stop - start)
                        .count();

                continue;
            }

            // Otherwise, just copy contents:

            auto start = std::chrono::high_resolution_clock::now();

            v_arr[i].reserve(size);

            std::copy_n(v_arr[i-1].begin(), size, v_arr[i].begin());

            auto stop = std::chrono::high_resolution_clock::now();

            v_update +=
                std::chrono::duration<double, std::milli>(stop - start).count();

            if (i == num - 1) {

                // Destroy the buffer:

                auto start = std::chrono::high_resolution_clock::now();

                // Clear final buffer:

                v_arr[i].clear();

                auto stop = std::chrono::high_resolution_clock::now();

                v_destroy +=
                    std::chrono::duration<double, std::milli>(stop - start)
                        .count();
            }
        }
    }

    //
    // Vector Pointer
    //

    long double vpoint_con = 0;
    long double vpoint_create = 0;
    long double vpoint_update = 0;
    long double vpoint_destroy = 0;

    std::cout << "+======================================+" << '\n';
    std::cout << " --== [ Vector Pointer: ] ==--" << "\n";

    for (int j = 0; j < iters; ++j) {

        // Initialize pointer vector:

        auto start = std::chrono::high_resolution_clock::now();

        std::array<VecPoint, num> vpoint_arr;

        auto stop = std::chrono::high_resolution_clock::now();

        vpoint_con +=
            std::chrono::duration<double, std::milli>(stop - start).count();

        std::cout << "Iter: [" << j << "]\n";

        for (int i = 0; i < num; ++i) {

            // Determine if we need to allocate a new buffer:

            if (i == 0) {

                // Create new buffer:

                auto start = std::chrono::high_resolution_clock::now();

                vpoint_arr[0] =
                    std::make_unique<Vect>(size);

                std::transform(vpoint_arr[0]->begin(), vpoint_arr[0]->end(),
                               vpoint_arr[0]->begin(), rand_val);

                auto stop = std::chrono::high_resolution_clock::now();

                vpoint_create +=
                    std::chrono::duration<double, std::milli>(stop - start)
                        .count();

                continue;
            }

            // Otherwise, move the vector pointer around:

            auto start = std::chrono::high_resolution_clock::now();

            vpoint_arr[i] = std::move(vpoint_arr[i - 1]);

            auto stop = std::chrono::high_resolution_clock::now();

            vpoint_update +=
                std::chrono::duration<double, std::milli>(stop - start).count();

            // Determine if we are done and need to destroy:

            if (i == num - 1) {

                // Destroy buffer:

                auto start = std::chrono::high_resolution_clock::now();

                vpoint_arr[i].reset();

                auto stop = std::chrono::high_resolution_clock::now();

                vpoint_destroy +=
                    std::chrono::duration<double, std::milli>(stop - start)
                        .count();
            }
        }
    }

    //
    // Vector move
    //

    long double vm_con = 0;
    long double vm_create = 0;
    long double vm_update = 0;
    long double vm_destroy = 0;

    std::cout << "+======================================+" << '\n';
    std::cout << " --== [ Vector Move: ] ==--" << "\n";

    for (int j = 0; j < iters; ++j) {

        // Initialize move vector:

        auto start = std::chrono::high_resolution_clock::now();

        std::array<Vect, num> vm_arr;

        auto stop = std::chrono::high_resolution_clock::now();

        vm_con +=
            std::chrono::duration<double, std::milli>(stop - start).count();

        std::cout << "Iter: [" << j << "]\n";

        for (int i = 0; i < num; ++i) {

            // Determine if we need to allocate a new buffer:

            if (i == 0) {

                // Create new buffer:

                auto start = std::chrono::high_resolution_clock::now();

                // Reserve:

                vm_arr[0].reserve(size);

                std::transform(vm_arr[0].begin(), vm_arr[0].end(),
                               vm_arr[0].begin(), rand_val);

                // Add values to vector:

                auto stop = std::chrono::high_resolution_clock::now();

                vm_create +=
                    std::chrono::duration<double, std::milli>(stop - start)
                        .count();

                continue;
            }

            // Otherwise, just copy contents:

            auto start = std::chrono::high_resolution_clock::now();

            // Clear current buffer:

            vm_arr[i].clear();

            // Move vector contents:

            vm_arr[i] = std::move(vm_arr[i-1]);

            auto stop = std::chrono::high_resolution_clock::now();

            vm_update +=
                std::chrono::duration<double, std::milli>(stop - start).count();

            if (i == num - 1) {

                // Destroy the buffer:

                auto start = std::chrono::high_resolution_clock::now();

                // Clear final buffer:

                vm_arr[i].clear();

                auto stop = std::chrono::high_resolution_clock::now();

                vm_destroy +=
                    std::chrono::duration<double, std::milli>(stop - start)
                        .count();
            }
        }
    }

    //
    // Results
    //

    std::cout << "+======================================+" << "\n";
    std::cout << " --== [ Results: ] ==--" << "\n";
    std::cout << "Vector Size: " << size << "\n";
    std::cout << "Vector Amounts: " << num << "\n";
    std::cout << "Iterations: " << iters << "\n";
    std::cout << "\n(Lower is better)\n";

    std::cout << "Vector Copy:" << "\n";
    std::cout << "-= Average Construct: " << v_con / iters << "\n";
    std::cout << "-= Average Create: " << v_create / iters << "\n";
    std::cout << "-= Average Update: " << v_update / iters << "\n";
    std::cout << "-= Average Destroy: " << v_destroy / iters << "\n";

    std::cout << "Vector Pointer:" << "\n";
    std::cout << "-= Average Construct: " << vpoint_con / iters << "\n";
    std::cout << "-= Average Create: " << vpoint_create / iters << "\n";
    std::cout << "-= Average Update: " << vpoint_update / iters << "\n";
    std::cout << "-= Average Destroy: " << vpoint_destroy / iters << "\n";

    std::cout << "Vector Move:" << "\n";
    std::cout << "-= Average Construct: " << vm_con / iters << "\n";
    std::cout << "-= Average Create: " << vm_create / iters << "\n";
    std::cout << "-= Average Update: " << vm_update / iters << "\n";
    std::cout << "-= Average Destroy: " << vm_destroy / iters << "\n";

    return 0;
}
