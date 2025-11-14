/**
 * @file parallel_bench.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Benchmark comparing serial and parallel components
 * @version 0.1
 * @date 2025-11-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <cstdint>
#include <chrono>
#include <iostream>

#include "sink_module.hpp"
#include "module_mixer.hpp"
#include "fund_oscillator.hpp"
#include "filter_module.hpp"
#include "parallel.hpp"

///
// Benchmark variables
///

/// Number of modules in the mixer
const std::size_t nmods = 3;

/// Size of the start buffer
const std::size_t bsize = 1000;

/// Size of the kernel
const std::size_t ksize = 100;

/// Number of iterations to preform
const std::size_t iters = 5;

/// Number of times to iterate
const std::size_t piter = 100;

int main() {

    ///
    // Serial section
    ///

    // Define the values to hold our times

    // Time spent on state operations
    std::chrono::duration<long, std::ratio<1, 1000000000>> sstime{};

    // Time spent on processing operations
    std::chrono::duration<long, std::ratio<1, 1000000000>> sptime{};

    // Minimum time spent on state operations
    std::chrono::duration<long, std::ratio<1, 1000000000>> ssmtime{};

    // Minimum time spent on processing operations
    std::chrono::duration<long, std::ratio<1, 1000000000>> spmtime{};

    // Serial sink

    SinkModule ssink;

    // Set the buffer size

    ssink.get_chain_info()->buffer_size = bsize;

    // Add a mixer to the sink

    ModuleMixDown smix;

    // Attach the mixer to the sink

    ssink.link(&smix);

    // Attach expensive modules to the mixer

    for (int i = 0; i < nmods; ++i) {

        // We use a sine oscillator

        SineOscillator* tsine = new SineOscillator(440);

        // We then use a SincFilter as it's expensive

        SincFilter<>* tfilt = new SincFilter();

        // Set the kernel size on the filter

        tfilt->set_size(ksize);

        // Link them together

        smix.link(tfilt)->link(tsine);
    }

    // We now have our chain, iterate a number of times

    for (int i = 0; i < iters; ++i) {

        // Start the chain

        auto sstart = std::chrono::high_resolution_clock::now();

        ssink.meta_start();

        auto sstop = std::chrono::high_resolution_clock::now();

        // Start our iteration loop

        auto pstart = std::chrono::high_resolution_clock::now();

        for (int j = 0; j < piter; ++j) {

            // Process the chain

            ssink.meta_process();
        }

        auto pstop = std::chrono::high_resolution_clock::now();

        // Stop the chain

        auto kstart = std::chrono::high_resolution_clock::now();

        ssink.meta_stop();

        auto kstop = std::chrono::high_resolution_clock::now();

        // Add the temp times to the total

        sstime += (kstop - kstart) + (sstop - sstart);
        sptime += (pstop - pstart);
    }

    ///
    // Parallel section
    ///

    // Define the values to hold our times

    // Time spent on state operations
    std::chrono::duration<long, std::ratio<1, 1000000000>> pstime{};

    // Time spent on processing operations
    std::chrono::duration<long, std::ratio<1, 1000000000>> pptime{};

    // Minimum time spent on state operations
    std::chrono::duration<long, std::ratio<1, 1000000000>> psmtime{};

    // Minimum time spent on processing operations
    std::chrono::duration<long, std::ratio<1, 1000000000>> ppmtime{};

    // Parallel sink

    SinkModule psink;

    // Set the buffer size

    psink.get_chain_info()->buffer_size = bsize;

    // Add a mixer to the sink

    ModuleMixDown pmix;

    // Attach the mixer to the sink

    psink.link(&pmix);

    // Attach expensive modules to the mixer

    for (int i = 0; i < nmods; ++i) {

        // The front should be a parallel module

        ParallelModule* tpar = new ParallelModule();

        // We use a sine oscillator

        SineOscillator* tsine = new SineOscillator(440);

        // We then use a SincFilter as it's expensive

        SincFilter<>* tfilt = new SincFilter();

        // Set the kernel size on the filter

        tfilt->set_size(ksize);

        // Link them together

        pmix.link(tpar)->link(tfilt)->link(tsine);
    }

    // We now have our chain, iterate a number of times

    for (int i = 0; i < iters; ++i) {

        // Start the chain

        auto sstart = std::chrono::high_resolution_clock::now();

        psink.meta_start();

        auto sstop = std::chrono::high_resolution_clock::now();

        // Start our iteration loop

        auto pstart = std::chrono::high_resolution_clock::now();

        for (int j = 0; j < piter; ++j) {

            // Process the chain

            psink.meta_process();
        }

        auto pstop = std::chrono::high_resolution_clock::now();

        // Stop the chain

        auto kstart = std::chrono::high_resolution_clock::now();

        psink.meta_stop();

        auto kstop = std::chrono::high_resolution_clock::now();

        // Add the temp times to the total

        pstime += (kstop - kstart) + (sstop - sstart);
        pptime += (pstop - pstart);

    }

    // Print the results of the benchmark

    std::cout << "+=======================================+" << "\n";
    std::cout << "--== [  Serial Results ] ==--" << "\n";
    std::cout << "Total State Time: " << std::chrono::duration<double, std::milli>(sstime).count() << "\n";
    std::cout << "Total Process Time: " << std::chrono::duration<double, std::milli>(sptime).count() << "\n";
    std::cout << "Average State Time: " << std::chrono::duration<double, std::milli>(sstime).count() / iters << "\n";
    std::cout << "Average Process Time: " << std::chrono::duration<double, std::milli>(sptime).count() / iters << "\n";

    std::cout << "--== [  Parallel Results ] ==--" << "\n";
    std::cout << "Total State Time: " << std::chrono::duration<double, std::milli>(pstime).count() << "\n";
    std::cout << "Total Process Time: " << std::chrono::duration<double, std::milli>(pptime).count() << "\n";
    std::cout << "Average State Time: " << std::chrono::duration<double, std::milli>(pstime).count() / iters << "\n";
    std::cout << "Average Process Time: " << std::chrono::duration<double, std::milli>(pptime).count() / iters << "\n";

    return 0;
}
