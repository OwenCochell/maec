/**
 * @file parallel_plot.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Plots a comparision between seriel and parallel module chains
 * @version 0.1
 * @date 2025-11-18
 *
 * @copyright Copyright (c) 2025
 *
 * Preforms a comparison between serial and parallel modules in mixing
 * scenarios.
 */

#include <matplot/matplot.h>

#include <chrono>
#include <iostream>
#include <limits>
#include <vector>

#include "filter_module.hpp"
#include "fund_oscillator.hpp"
#include "module_mixer.hpp"
#include "parallel.hpp"
#include "sink_module.hpp"

///
// Benchmark variables
///

/// Lowest number of modules to add
const std::size_t snmods = 1;

/// Maximum number of modules to add
const std::size_t mnmods = 200;

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

/// Shorthand matplot namespace
namespace mp = matplot;

int main() {

    // Create a vector of module sizes (integer steps from snmods to mnmods)
    std::vector<double> x;
    x.reserve(mnmods - snmods + 1);
    for (std::size_t n = snmods; n <= mnmods; ++n) {
        x.push_back(static_cast<double>(n));
    }

    // Define arrays for serial and parallel times for processing and state

    std::vector<double> stime;
    std::vector<double> ptime;
    std::vector<double> sstimer;
    std::vector<double> pstimer;

    // Iterate over X values

    for (auto nmods : x) {

        ///
        // Serial Section
        ///

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

        // Attach the mixer to the sink

        ssink.link(&smix);

        // Attach expensive modules to the mixer

        std::vector<std::shared_ptr<SineOscillator>> oscs;
        std::vector<std::shared_ptr<SincFilter<>>> filts;

        for (int i = 0; i < nmods; ++i) {

            // We use a sine oscillator

            auto tsine = std::make_shared<SineOscillator>(440);
            oscs.push_back(tsine);

            // We then use a SincFilter as it's expensive

            auto tfilt = std::make_shared<SincFilter<>>();
            filts.push_back(tfilt);

            // Set the kernel size on the filter

            tfilt->set_size(ksize);

            // Set a filtering frequency, in hertz

            tfilt->set_start_freq(200);

            // Link them together

            smix.link(tfilt.get())->link(tsine.get());
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
            auto ptime_local = (kstop - kstart) + (sstop - sstart);
            auto pptime_local = (pstop - pstart);

            ssmtime = std::min(ssmtime, ptime_local);
            spmtime = std::min(spmtime, pptime_local);

            sstime += ptime_local;
            sptime += pptime_local;
        }

        // Determine the average computation time and add to results

        stime.push_back(
            std::chrono::duration<double, std::milli>(sptime).count() / iters);

        sstimer.push_back(
            std::chrono::duration<double, std::milli>(sstime).count() / iters);

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

        // Add a mixer to the sink

        ModuleMixDown pmix;

        // Attach the mixer to the sink

        psink.link(&pmix);

        // Attach expensive modules to the mixer

        std::vector<std::shared_ptr<ParallelModule>> pars;

        for (int i = 0; i < nmods; ++i) {

            // The front should be a parallel module

            auto tpar = std::make_shared<ParallelModule>();
            pars.push_back(tpar);

            // Set the cache size of the parallel module

            tpar->max_size(csize);

            // We use a sine oscillator

            auto tsine = std::make_shared<SineOscillator>(440);
            oscs.push_back(tsine);

            // We then use a SincFilter as it's expensive

            auto tfilt = std::make_shared<SincFilter<>>();
            filts.push_back(tfilt);

            // Set the kernel size on the filter

            tfilt->set_size(ksize);

            // Set a filtering frequency, in hertz

            tfilt->set_start_freq(200);

            // Link them together

            pmix.link(tpar.get())->link(tfilt.get())->link(tsine.get());
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

            auto stime_local = (kstop - kstart) + (sstop - sstart);
            auto ptime_local = (pstop - pstart);

            psmtime = std::min(psmtime, stime_local);
            ppmtime = std::min(ppmtime, ptime_local);

            pstime += stime_local;
            pptime += ptime_local;
        }

        ptime.push_back(
            std::chrono::duration<double, std::milli>(pptime).count() / iters);

        pstimer.push_back(
            std::chrono::duration<double, std::milli>(pstime).count() / iters);

        std::cout << nmods << "\n";
    }

    // Create a figure and set size
    auto f = mp::figure(true);
    f->size(1000, 800);
    f->name("Serial vs. Parallel Operations");
    f->color("w");

    // Left subplot: Processing Operations
    mp::subplot(2, 1, 2);
    auto plotps = mp::plot(x, stime);
    plotps->display_name("Serial");
    plotps->line_width(2.5);
    mp::hold(mp::on);
    auto plotpp = mp::plot(x, ptime);
    plotpp->display_name("Parallel");
    plotpp->line_width(2.5);
    mp::hold(mp::off);
    mp::title("Processing Operations");
    mp::xlabel("Number of Modules");
    mp::ylabel("Time in Milliseconds");
    mp::legend()->location(mp::legend::general_alignment::left);

    // Right subplot: State Operations
    mp::subplot(2, 1, 1);
    plotps = mp::plot(x, sstimer);
    plotps->display_name("Serial");
    plotps->line_width(2.5);
    mp::hold(mp::on);
    plotpp = mp::plot(x, pstimer);
    plotpp->display_name("Parallel");
    plotpp->line_width(2.5);
    mp::hold(mp::off);
    mp::title("State Operations");
    mp::xlabel("Number of Modules");
    mp::ylabel("Time in Milliseconds");
    mp::legend()->location(mp::legend::general_alignment::left);

    // Show the plot window
    mp::show();

    return 0;
}
