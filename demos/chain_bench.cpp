/**
 * @file chain_bench.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Generic chain benchmark
 * @version 0.1
 * @date 2025-10-04
 *
 * @copyright Copyright (c) 2025
 *
 * We implement a basic chain benchmark
 * for profiling chain implementations.
 * The primary purpose is to compare performance between
 * differing chain implementations.
 */

#include <array>
#include <chrono>
#include <cstddef>
#include <iostream>

#include "amp_module.hpp"
#include "base_module.hpp"
#include "meta_audio.hpp"
#include "sink_module.hpp"

/// Module to create the chain
using TestModule = AmplitudeScale;

int main() {

    // Number of iterations to preform
    const std::size_t iter = 500000;

    // Buffer size
    const std::size_t buff = 100;

    // Number of modules in each chain
    const std::size_t nums = 100;

    // Array to hold our modules

    std::array<TestModule, nums> mods;

    std::cout << "+================================+" << '\n';
    std::cout << " !Benchmarking chain performance!" << '\n';
    std::cout << "+================================+" << '\n';

    // Determine if our clock is steady:

    if (!std::chrono::high_resolution_clock::is_steady) {

        std::cout << "Warning: high_resolution_clock is not steady!" << '\n';
        std::cout << "This may cause inaccurate results." << '\n';
        std::cout << "+================================+" << '\n';
    }

    // Time to create each time
    long double create = 0;

    // Process time of each chain
    long double process = 0;

    for (std::size_t i = 0; i < iter; ++i) {

        // Start the clock

        auto start = std::chrono::high_resolution_clock::now();

        // Create a sink for the chain

        SinkModule sink;

        // The sink should have the desired size

        sink.get_info()->out_buffer = buff;

        // Create a constant source

        ConstModule source;

        // Module to add to collection

        BaseModule* lmod = &sink;

        // Iterate over the number of modules to add

        for (std::size_t modi = 0; modi < nums; ++modi) {

            // Create a module to add to the collection,
            // with a random amplitude value

            mods[modi] = std::move(TestModule());

            // Add the module to the chain

            lmod = lmod->link(&mods[modi]);
        }

        // Add the source to the chain

        lmod->link(&source);

        // Preform the info sync

        sink.meta_info_sync();

        // Preform the meta start

        sink.meta_start();

        // Determine the stop time

        auto stop = std::chrono::high_resolution_clock::now();

        // Calculate the diff

        auto ctime =
            std::chrono::duration<double, std::milli>(stop - start).count();

        std::cout << "Creation Time: [" << ctime << "] ms\n";

        // Add the time to the total

        create += ctime;

        // Start the clock

        start = std::chrono::high_resolution_clock::now();

        // Process the chain

        sink.meta_process();

        // Stop the clock

        stop = std::chrono::high_resolution_clock::now();

        // Calculate the diff

        auto ptime =
            std::chrono::duration<double, std::milli>(stop - start).count();

        std::cout << "Process Time: [" << ptime << "] ms\n";

        process += ptime;

        // Preform the meta stop
        // TODO: Should we keep track of meta operations?

        sink.meta_stop();
    }

    // Output the results

    std::cout << "+=======================================+" << "\n";
    std::cout << "--== [ Results ] ==--" << "\n";

    std::cout << "Average Creation Time: " << create / iter << "\n";
    std::cout << "Average Process Time: " << process / iter << "\n";

    return 0;
}
