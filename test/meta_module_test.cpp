/**
 * @file meta_module_test.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Tests for various meta modules
 * @version 0.1
 * @date 2023-01-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "meta_audio.hpp"
#include "base_oscillator.hpp"
#include "fund_oscillator.hpp"

TEST_CASE("MetaCount Test", "[meta][count]") {

    // Construct a counter:

    Counter count;

    // Ensure default values work correctly:

    REQUIRE(0 == count.processed());
    REQUIRE(0 == count.samples());

    // Create constant oscillator:

    ConstantOscillator con(5.0);

    // Bind the constant oscillator:

    count.bind(&con);

    SECTION("Processing", "Ensures processing works correctly") {

        // Process a few times:

        count.meta_process();
        count.meta_process();
        count.meta_process();

        // Ensure numbers are right:

        REQUIRE(3 == count.processed());
        REQUIRE(3 * con.get_info()->out_buffer == count.samples());

        SECTION("Reset", "Ensures counter resets work correctly") {

            // Reset:

            count.reset();

            // Ensure values are zero:

            REQUIRE(0 == count.processed());
            REQUIRE(0 == count.samples());
        }
    }
}

TEST_CASE("MetaLatency Test", "[meta]") {

    // Create latency module:

    LatencyModule late;

    // Create module to use for benchmarking:

    SineOscillator sine;

    // Bind the sine oscillator:

    late.bind(&sine);

    SECTION("Default", "Ensure default values are accurate") {

        // Ensure initial values are zero:

        REQUIRE(0 == late.time());
        REQUIRE(0 == late.total_time());
        REQUIRE(0 == late.latency());
        REQUIRE(0 == late.sum_latency());
        REQUIRE(0 == late.total_latency());
        REQUIRE(0 == late.expected());
    }

    // Meta process:

    late.start();
    late.meta_process();

    SECTION("Total Time", "Ensures our understanding of total time is accurate") {

        // Ensure start time is not zero:

        REQUIRE(0 < late.get_start_time());

        // Get total time elapsed:

        REQUIRE(std::abs(late.elapsed() - (get_time() - late.get_start_time())) < 1000);

        // Get expected time:

        REQUIRE(440 * (NANO / 44100) == late.expected());
    }

    SECTION("Operation Time", "Ensures our understanding of the operation time is accurate") {

        // Get the elapsed time:

        int64_t total = late.time();

        REQUIRE(late.time() < late.elapsed());
        REQUIRE(late.time() == late.total_time());
        REQUIRE(late.time() == late.average_time());

        // Meta process once more:

        late.meta_process();

        total += late.time();

        REQUIRE(late.time() < late.elapsed());
        REQUIRE(late.total_time() == total);
        REQUIRE(total / 2 == late.average_time());
    }

    SECTION("Latency Time", "Ensures our understanding of the latency time is accurate") {

        // Get the elapsed time:

        int64_t total = late.latency();

        REQUIRE(late.latency() < late.time());
        REQUIRE(late.latency() == late.sum_latency());
        REQUIRE(late.latency() == late.average_latency());
        REQUIRE(late.total_latency() == late.total_time() - late.expected());

        // Meta process once more:

        late.meta_process();

        total += late.latency();

        REQUIRE(late.latency() < late.time());
        REQUIRE(late.sum_latency() == total);
        REQUIRE(total / 2 == late.average_latency());
        REQUIRE(late.total_latency() == late.total_time() - late.expected());
    }

    SECTION("Reset", "Ensures we can be reset") {

        // Reset the module:

        late.reset();

        // Ensure values are zero:

        REQUIRE(0 == late.time());
        REQUIRE(0 == late.total_time());
        REQUIRE(0 == late.average_time());

        REQUIRE(0 == late.latency());
        REQUIRE(0 == late.total_latency());
        REQUIRE(0 == late.average_latency());
        REQUIRE(0 == late.sum_latency());
    }
}

TEST_CASE("MetaBuffer Test", "[meta]") {

    // Construct a buffer module:

    BufferModule buff;

    // Create a dummy buffer:

    std::vector<long double> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    AudioBuffer rep(data);

    SECTION("GetSet", "Ensures the getters and setters work properly") {

        // Attach the dummy data:

        buff.set_rbuffer(&rep);

        // Get dummy data:

        REQUIRE(&rep == buff.get_rbuffer());
    }

    SECTION("Process", "Ensures the process function works properly") {

        // Attach the dummy data:

        buff.set_rbuffer(&rep);

        // Next, ensure data is accurate:

        std::shared_ptr<AudioBuffer> tdata = nullptr;

        for (int i = 0; i < 10 * 4; ++i) {

            if (i % 10 == 0) {
                // Process:

                buff.meta_process();

                tdata = buff.get_buffer();
            }

            REQUIRE_THAT(data.at(i % 10),
                         Catch::Matchers::WithinAbs(tdata->at(i % 10), 0.0001));
        }
    }
}

TEST_CASE("UniformBuffer Test", "[meta]") {

    // Construct a UniformBuffer:

    UniformBuffer uni;

    // Create a BufferModule:

    BufferModule mbuf;

    SECTION("Same Size", "Ensures the UniformBuffer can return data of the same size") {

        // Configure modules:

        int size = 247;

        mbuf.get_info()->out_buffer = size;
        uni.get_info()->out_buffer = size;

        // Bind the modules:

        uni.bind(&mbuf);

        // Create buffer:

        AudioBuffer buff(size);
        auto iter = buff.ibegin();

        for (int i = 0; i < size; ++i) {

            *iter++ = i;
        }

        // Set buffer:

        mbuf.set_rbuffer(&buff);

        // Finally, meta process:

        for (int i = 0; i < 4; ++i) {

            // Meta process:

            uni.meta_process();

            // Grab the buffer:

            auto buff = uni.get_buffer();

            // Ensure buffer is valid:

            iter = buff->ibegin();

            for (int j = 0; j < size; ++j) {

                REQUIRE_THAT(*iter++, Catch::Matchers::WithinAbs(j, 0.0001));
            }
        }
    }

    SECTION("Smaller Size", "Ensures the UniformBuffer can return data of smaller size") {

        // Configure modules:

        int osize = 100;
        int isize = 20;

        mbuf.get_info()->out_buffer = isize;
        uni.get_info()->out_buffer = osize;

        // Bind the modules:

        uni.bind(&mbuf);

        // Loop a certain number of times:

        long double last = 0;

        // Create buffer:

        AudioBuffer buff(isize);
        auto iter = buff.ibegin();

        for (int i = 0; i < isize; ++i) {

            *iter++ = i;
        }

        // Set buffer:

        mbuf.set_rbuffer(&buff);

        // Meta process:

        uni.meta_process();

        // Grab buffer:

        auto obuff = uni.get_buffer();

        // Ensure buffer is accurate:

        for (auto iter = obuff->ibegin(); iter != obuff->iend(); ++iter) {

            REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(iter.get_index() % isize, 0.0001));
        }
    }

    SECTION("Bigger Size", "Ensures the UniformBuffer can return data of larger size") {

        // Configure the module:

        int isize = 100;
        int osize = 20;

        uni.get_info()->out_buffer = osize;
        mbuf.get_info()->out_buffer = isize;

        // Bind the modules:

        uni.bind(&mbuf);

        // Create a buffer:

        AudioBuffer buff(isize);
        auto iter = buff.ibegin();

        // Add values to buffer:

        for (int j = 0; j < isize; ++j) {

            *iter++ = j;
        }

        // Add buffer to module:

        mbuf.set_rbuffer(&buff);

        // Iterate a certain number of time:

        for (int i = 0; i < (isize / 20); ++i) {

            // Meta process:

            uni.meta_process();

            // Grab the buffer:

            auto obuff = uni.get_buffer();

            // Ensure buffer is accurate:

            for (auto tier = obuff->ibegin(); tier != obuff->iend(); ++tier) {

                // Check value:

                REQUIRE_THAT(*tier, Catch::Matchers::WithinAbs(tier.get_index() + (i * osize), 0.0001));
            }
        }
    }
}
