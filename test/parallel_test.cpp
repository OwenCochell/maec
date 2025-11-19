/**
 * @file parallel.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Tests for parallel modules
 * @version 0.1
 * @date 2025-11-13
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "parallel.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <chrono>
#include <cstddef>
#include <thread>

#include "meta_audio.hpp"

/**
 * @brief Special test module for parallel operations
 *
 */
class TestParallel : public Counter, public ConstModule {
private:
    /// Wait time in seconds
    double wait_time = 0.;

public:
    TestParallel() = default;

    TestParallel(double vwt) : wait_time(vwt) {}

    /**
     * @brief Generates audio data for testing
     *
     * We just wait for a period,
     * increment the counter, and generate a buffer.
     *
     */
    void meta_process() override {

        // Wait for the time specified in the wait time

        std::this_thread::sleep_for(std::chrono::duration<double>(wait_time));

        // The counter process method to update our count

        Counter::process();

        // Set the fill value to be our count

        ConstModule::set_value(Counter::processed() - 1);

        // Fill the buffer with our values

        ConstModule::process();
    }
};

TEST_CASE("Parallel module tests", "[mod][parallel]") {

    ParallelModule par;

    SECTION("Construct", "Ensures default constructor has correct values") {

        REQUIRE(par.max_size() == 1);
        REQUIRE(par.size() == 0);
    }

    SECTION("Max Size", "Ensures we can correctly set the queue size") {

        par.max_size(100);

        REQUIRE(par.max_size() == 100);
    }

    SECTION("Process", "Ensures we can process values in parallel") {

        // Maximum queue size

        const std::size_t qsize = 5;

        // Create a source, just make it constant

        TestParallel source;

        // Link the source to the parallel module

        par.link(static_cast<ConstModule*>(&source));

        // Set the maximum size to be bigger than 1

        par.max_size(qsize);

        // Start the module,
        // this will spin up the processing thread

        par.start();

        // We need to iterate and keep track of the time since start,
        // we expect at most a buffer will be generated each second.
        // To do this we spin while continuously checking the state,
        // and ensuring a new value is generated at most every 1 second

        // The last size we collected
        std::size_t lsize = 0;

        // The last time we found a sample

        auto ltime{std::chrono::steady_clock::now()};

        // Iterate until we reach our queue limit

        while (par.size() < qsize) {

            // Determine if a new sample is in the cache

            if (lsize != par.size()) {

                // We have a new value, update

                lsize = par.size();
                ltime = {std::chrono::steady_clock::now()};

                continue;
            }

            // Grab the current time

            auto curr{std::chrono::steady_clock::now()};

            // Determine the duration since we last added something

            std::chrono::duration<double> elapsed_seconds{curr - ltime};

            // Determine if we have gone over a second and need to fail

            REQUIRE(elapsed_seconds.count() < 1.);
        }

        // Now, we need to wait a long time here to make sure more values don't
        // get added, we just wait 4 seconds

        std::this_thread::sleep_for(std::chrono::duration<double>(4));

        // Make sure the queue size does not exceed the maximum

        REQUIRE(par.size() == par.max_size());

        // Remove a value from the queue

        par.meta_process();

        // Grab the buffer for testing
        auto buf = par.get_buffer();

        for (auto val : buf) {

            // Ensure the first buffer retrieved is all 0

            REQUIRE(val == 0.);
        }

        // Wait another 4 seconds to allow the queue to be filled

        std::this_thread::sleep_for(std::chrono::duration<double>(4));

        // Ensure the queue is back to maximum again

        REQUIRE(par.size() == par.max_size());

        SECTION("Multi-process",
                "Ensures the parallel module can be processed twice") {

            // Process the module once again

            par.meta_process();

            // Grab the buffer

            buf = par.get_buffer();

            // Ensure the values are all 1

            for (auto val : buf) {

                REQUIRE(val == 1.);
            }
        }

        // Now, stop the parallel module

        par.stop();
    }

    SECTION("Process Wait", "Ensures we can work correctly for slow modules") {

        // Maximum queue size

        const std::size_t qsize = 5;

        // Create a source, add a wait time

        TestParallel source(3.);

        // Link the source to the parallel module

        par.link(static_cast<ConstModule*>(&source));

        // Set the maximum size to be bigger than 1

        par.max_size(qsize);

        // Start the module,
        // this will spin up the processing thread

        par.start();

        // Iterate a number of times to clean our queue

        for (int i = 0; i < 5; ++i) {

            // Meta process this module,
            // we expect to wait some time here

            par.meta_process();

            // Grab the buffer

            auto buff = par.get_buffer();

            // Ensures all values match our count

            for (const auto& val : buff) {

                REQUIRE_THAT(val, Catch::Matchers::WithinAbs(i, 0.0001));
            }
        }

        // We are done, lets stop now

        par.stop();
    }

    SECTION("Restart",
            "Ensures the parallel module works correctly after being "
            "stopped/started") {

        // Maximum queue size

        const std::size_t qsize = 5;

        // Create a source, just make it constant

        TestParallel source;

        // Link the source to the parallel module

        par.link(static_cast<ConstModule*>(&source));

        // Set the maximum size to be bigger than 1

        par.max_size(qsize);

        // Start the module,
        // this will spin up the processing thread

        par.start();

        // Process as many times that are in the queue

        for (int i = 0; i < qsize; ++i) {

            // Process and grab the buffer

            par.meta_process();

            auto buf = par.get_buffer();

            // Ensure the values match the current index

            for (auto val : buf) {
                REQUIRE(val == static_cast<double>(i));
            }
        }

        // Now, stop the module

        par.stop();

        // Determine the new offset

        const std::size_t offset = qsize + par.size();

        // Start the module again

        par.start();

        // Process as many times that are in the queue

        for (int i = 0; i < qsize; ++i) {

            // Process and grab the buffer

            par.meta_process();

            auto buf = par.get_buffer();

            // Ensure the values match the current index

            for (auto val : buf) {
                REQUIRE(val == static_cast<double>(i + offset));
            }
        }

        // Finally, stop the module

        par.stop();
    }
}
