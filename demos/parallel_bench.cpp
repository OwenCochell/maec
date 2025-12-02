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

#include <chrono>
#include <iostream>
#include <limits>

#include "filter_module.hpp"
#include "fund_oscillator.hpp"
#include "module_mixer.hpp"
#include "parallel.hpp"
#include "sink_module.hpp"

///
// Benchmark variables
///

/// Number of modules in the mixer
const std::size_t nmods = 200;

/// Size of the start buffer
const std::size_t bsize = 100;

/// Size of the kernel
const std::size_t ksize = 50;

/// Number of iterations to preform
const std::size_t iters = 10;

/// Number of times to iterate
const std::size_t piter = 10;

/// Size of the parallel cache
const std::size_t csize = 10;

/// Determines if we should preform serial operations
const bool do_serial = true;

/// Determines if we should preform parallel operations
const bool do_parallel = true;

int main() {

    if (do_serial) {

        ///
        // Serial section
        ///

        // Define the values to hold our times

        // Time spent on state operations
        std::chrono::duration<long, std::ratio<1, 1000000000>> sstime{};

        // Time spent on processing operations
        std::chrono::duration<long, std::ratio<1, 1000000000>> sptime{};

        // Minimum time spent on state operations
        std::chrono::duration<long, std::ratio<1, 1000000000>> ssmtime{
            std::numeric_limits<long>::max()};

        // Minimum time spent on processing operations
        std::chrono::duration<long, std::ratio<1, 1000000000>> spmtime{
            std::numeric_limits<long>::max()};

        // Serial sink

        SinkModule ssink;

        // Set the buffer size

        ssink.get_chain_info()->buffer_size = bsize;

        // Add a mixer to the sink

        ModuleMixDown smix;

        // Add a lattency module to the sink

        LatencyModule lat;

        // Attach the mixer to the sink

        ssink.link(&lat)->link(&smix);

        // Attach expensive modules to the mixer

        for (int i = 0; i < nmods; ++i) {

            // We use a sine oscillator

            auto* tsine = new SineOscillator(440);

            // We then use a SincFilter as it's expensive

            auto* tfilt = new SincFilter();

            // Set the kernel size on the filter

            tfilt->set_size(ksize);

            // Set a filtering frequency, in hertz

            tfilt->set_start_freq(200);

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

            auto ptime = (kstop - kstart) + (sstop - sstart);
            auto pptime = (pstop - pstart);

            ssmtime = std::min(ssmtime, ptime);
            spmtime = std::min(spmtime, pptime);

            sstime += ptime;
            sptime += pptime;
        }

        std::cout << "+=======================================+" << "\n";
        std::cout << "--== [  Serial Results ] ==--" << "\n";
        std::cout << "Total State Time: "
                  << std::chrono::duration<double, std::milli>(sstime) << "\n";
        std::cout << "Total Process Time: "
                  << std::chrono::duration<double, std::milli>(sptime) << "\n";
        std::cout << "Average State Time: "
                  << std::chrono::duration<double, std::milli>(sstime) / iters
                  << "\n";
        std::cout << "Average Process Time: "
                  << std::chrono::duration<double, std::milli>(sptime) /
                         (iters * piter)
                  << "\n";
        std::cout << "Minimum Process Time: "
                  << std::chrono::duration<double, std::milli>(spmtime) << "\n";
        std::cout << "Minimum State Time: "
                  << std::chrono::duration<double, std::milli>(ssmtime) << "\n";
        std::cout << "Average Latency: " << lat.average_latency() * 1e-6
                  << " ms\n";
        std::cout << "Total Latency: " << lat.total_latency() * 1e-6 << " ms\n";
    }

    if (do_parallel) {

        ///
        // Parallel section
        ///

        // Define the values to hold our times

        // Time spent on state operations
        std::chrono::duration<long, std::ratio<1, 1000000000>> pstime{};

        // Time spent on processing operations
        std::chrono::duration<long, std::ratio<1, 1000000000>> pptime{};

        // Minimum time spent on state operations
        std::chrono::duration<long, std::ratio<1, 1000000000>> psmtime{
            std::numeric_limits<long>::max()};

        // Minimum time spent on processing operations
        std::chrono::duration<long, std::ratio<1, 1000000000>> ppmtime{
            std::numeric_limits<long>::max()};

        // Parallel sink

        SinkModule psink;

        // Set the buffer size

        psink.get_chain_info()->buffer_size = bsize;

        // Add a latency module to the chain

        LatencyModule lat;

        // Add a mixer to the sink

        ModuleMixDown pmix;

        // Attach the mixer to the sink

        psink.link(&lat)->link(&pmix);

        // Attach expensive modules to the mixer

        for (int i = 0; i < nmods; ++i) {

            // The front should be a parallel module

            auto* tpar = new ParallelModule();

            // Set the cache size of the parallel module

            tpar->max_size(csize);

            // We use a sine oscillator

            auto* tsine = new SineOscillator(440);

            // We then use a SincFilter as it's expensive

            auto* tfilt = new SincFilter();

            // Set the kernel size on the filter

            tfilt->set_size(ksize);

            // Set a filtering frequency, in hertz

            tfilt->set_start_freq(200);

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

            auto stime = (kstop - kstart) + (sstop - sstart);
            auto ptime = (pstop - pstart);

            psmtime = std::min(psmtime, stime);
            ppmtime = std::min(ppmtime, ptime);

            pstime += stime;
            pptime += ptime;
        }

        std::cout << "--== [  Parallel Results ] ==--" << "\n";
        std::cout << "Total State Time: "
                  << std::chrono::duration<double, std::milli>(pstime) << "\n";
        std::cout << "Total Process Time: "
                  << std::chrono::duration<double, std::milli>(pptime) << "\n";
        std::cout << "Average State Time: "
                  << std::chrono::duration<double, std::milli>(pstime) / iters
                  << "\n";
        std::cout << "Average Process Time: "
                  << std::chrono::duration<double, std::milli>(pptime) /
                         (iters * piter)
                  << "\n";
        std::cout << "Minimum Process Time: "
                  << std::chrono::duration<double, std::milli>(ppmtime) << "\n";
        std::cout << "Minimum State Time: "
                  << std::chrono::duration<double, std::milli>(psmtime) << "\n";
        std::cout << "Average Latency:" << lat.average_latency() * 1e-6
                  << " ms\n";
        std::cout << "Total Latency: " << lat.total_latency() * 1e-6 << "ms\n";

        // std::cout << "\nTotal Difference: "
        //           << std::chrono::duration<double, std::milli>(sptime) -
        //                  std::chrono::duration<double, std::milli>(pptime)
        //           << "\n";
    }

    return 0;
}
