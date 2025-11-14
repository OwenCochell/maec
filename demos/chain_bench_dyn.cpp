/**
 * @file chain_bench.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Generic dynamic chain benchmark
 * @version 0.1
 * @date 2025-10-04
 *
 * @copyright Copyright (c) 2025
 *
 * We implement a basic chain benchmark
 * for profiling chain implementations.
 * The primary purpose is to compare performance between
 * differing chain implementations.
 *
 * This file will read in a dynamic chain definition,
 * which will create a chain at runtime.
 */

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <random>
#include <ratio>
#include <string>
#include <utility>

#include "amp_module.hpp"
#include "audio_module.hpp"
#include "base_module.hpp"
#include "filter_module.hpp"
#include "meta_audio.hpp"
#include "sink_module.hpp"

/// Module to create the chain
using TestModule = AmplitudeScale<>;

constexpr std::string CHAIN = "fsffafaass";

int main() {

    srand(time(0));

    // Number of iterations to preform
    const std::size_t iter = 50000;

    // Buffer size
    const std::size_t buff = 10;

    // Number of modules in each chain
    const std::size_t nums = CHAIN.size();

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
    std::chrono::duration<long, std::ratio<1, 1000000000>> create{};
    bool cset = false;

    // Process time of each chain
    std::chrono::duration<long, std::ratio<1, 1000000000>> process{};
    bool pset = false;

    for (std::size_t i = 0; i < iter; ++i) {

        // Start the clock

        auto start = std::chrono::high_resolution_clock::now();

        // Create a sink for the chain

        SinkModule sink;

        // The sink should have the desired size

        sink.get_info()->out_buffer = buff;

        // Just for fun, add a LatencyModule

        // LatencyModule lat;

        // Add the latency module to the chain

        // sink.link(&lat);

        // Create a constant source

        ConstModule source;

        // Module to add to collection

        BaseModule* lmod = &sink;

        // Iterate over the dynamic definition

        for (std::size_t modi = 0; modi < nums; ++modi) {

            // Determine what module to add

            BaseModule* mod = nullptr;

            if (CHAIN[modi] == 'a') {

                // This is an add module

                mod =
                    new AmplitudeAdd<>(static_cast<double>(rand()) / RAND_MAX);

            }

            else if (CHAIN[modi] == 's') {

                // This is a scale module

                mod = new AmplitudeScale<>((static_cast<double>(rand())) /
                                           RAND_MAX);
            }

            if (CHAIN[modi] == 'f') {

                // This is a sinc filter

                mod = new SincFilter();
            }

            // Add the module to the chain

            lmod = lmod->link(mod);
        }

        // Add the source to the chain

        lmod->link(&source);

        // Preform the info sync

        sink.meta_info_sync();

        // Preform the meta start

        sink.meta_start();

        // Determine the stop time

        auto stop = std::chrono::high_resolution_clock::now();

        // Add the time to the total

        if (!cset) {
            create = stop - start;
            cset = true;
        }

        else {
            create = std::min(stop - start, create);
        }

        // Start the clock

        start = std::chrono::high_resolution_clock::now();

        // Process the chain

        sink.meta_process();

        // Stop the clock

        stop = std::chrono::high_resolution_clock::now();

        // Calculate the diff

        if (!pset) {
            process = stop - start;
            pset = true;
        }

        else {
            process = std::min(stop - start, process);
        }

        // Get the latency of the chain

        // auto latv = lat.latency();

        // std::cout << "Module Latency: [" << latv << "] ns\n";

        // Preform the meta stop
        // TODO: Should we keep track of meta operations?

        sink.meta_stop();
    }

    // Output the results

    std::cout << "+=======================================+" << "\n";
    std::cout << "--== [ Results ] ==--" << "\n";

    std::cout << "Minimum Creation Time: "
              << std::chrono::duration<double, std::milli>(create).count()
              << "\n";
    std::cout << "Minimum Process Time: "
              << std::chrono::duration<double, std::milli>(process).count()
              << "\n";

    return 0;
}
