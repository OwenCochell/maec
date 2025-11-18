/**
 * @file static_bench.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Benchmark for static chains
 * @version 0.1
 * @date 2025-10-04
 *
 * @copyright Copyright (c) 2025
 *
 * We primarily benchmark static chains,
 * which can be compared to dynamic chains.
 *
 * As of now, I do NOT have a method to generate static chains,
 * I will likely be creating a 3rd party program to automate this.
 */

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <iostream>

#include "amp_module.hpp"
#include "audio_module.hpp"
#include "base_module.hpp"
#include "filter_module.hpp"
#include "meta_audio.hpp"
#include "sink_module.hpp"

int main() {

    // Number of iterations to preform
    const std::size_t iter = 100;

    /// Number of processing iterations to preform
    const std::size_t piter = 50;

    // Buffer size
    const std::size_t buff = 100;

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

        // Create a static chain to utilize

        SinkModule<SincFilter<AmplitudeScale<SincFilter<
            SincFilter<AmplitudeAdd<SincFilter<AmplitudeAdd<AmplitudeAdd<
                AmplitudeScale<AmplitudeScale<ConstModule>>>>>>>>>>>
            sink;
        sink.get_info()->out_buffer = buff;

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
        } else {

            create = std::min(stop - start, create);
        }

        // Start the clock

        start = std::chrono::high_resolution_clock::now();

        // Process the chain

        for (int j = 0; j < piter; ++j) {
            sink.meta_process();
        }

        // Stop the clock

        stop = std::chrono::high_resolution_clock::now();

        if (!pset) {
            process = stop - start;
            pset = true;
        } else {

            process = std::min(stop - start, process);
        }

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
