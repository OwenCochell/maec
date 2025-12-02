/**
 * @file parallel_plot.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Plots a comparision between seriel and parallel module chains
 * @version 0.1
 * @date 2025-11-18
 *
 * @copyright Copyright (c) 2025
 *
 * Preforms a comparison between serial and parallel modules in mixing scenarios.
 * Also, I know this program leaks memory...
 */

#include <sciplot/sciplot.hpp>
#include <vector>
#include <iostream>

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
const std::size_t mnmods = 20;

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

namespace sp = sciplot;

int main() {

    // Create a vector of module sizes

    const auto x = sp::linspace(snmods, mnmods, mnmods - snmods);

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

        ptime.push_back(
            std::chrono::duration<double, std::milli>(pptime).count() /
            (iters));

        pstimer.push_back(
            std::chrono::duration<double, std::milli>(pstime).count() / iters);

        std::cout << nmods << "\n";
    }

    // Create the plots, one for processing and one for state

    sp::Plot2D pplot;
    pplot.drawCurve(x, stime).label("Serial");
    pplot.drawCurve(x, ptime).label("Parallel");
    pplot.ylabel("Time in Milliseconds");
    pplot.xlabel("Number of Modules");
    pplot.legend().atLeft();

    sp::Plot2D splot;
    splot.drawCurve(x, sstimer).label("Serial");
    splot.drawCurve(x, pstimer).label("Parallel");
    splot.ylabel("Time in Milliseconds");
    splot.xlabel("Number of Modules");
    splot.legend().atLeft();

    // Use previous plots as sub-figures in larger 1x2 figure

    sp::Figure pfig = {{pplot}, {splot}};
    pfig.title("Processing Operations");

    // Create the canvas to hold the figure

    sp::Canvas canv = {{pfig}};
    canv.title("Serial vs. Parallel Operations");
    canv.size(800, 800);

    // Show the plot
    canv.show();

    return 0;
}
