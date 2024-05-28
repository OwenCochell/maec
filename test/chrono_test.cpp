/**
 * @file chrono_test.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Tests for chrono components
 * @version 0.1
 * @date 2023-02-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "chrono.hpp"

#include <catch2/catch_test_macros.hpp>
#include <cstdint>

TEST_CASE("ChainTimer Test", "[chrono]") {

    // Construct a ChainTimer:

    ChainTimer time;

    SECTION("Samplerate", "Ensures we can properly get and set the samplerate") {

        // Set the samplerate:

        time.set_samplerate(400);

        // Ensure samplerate is accurate:

        REQUIRE(time.get_samplerate() == 400);
    }

    SECTION("NPF", "Ensures the nanoseconds per frame methods are accurate") {

        // Ensure default NPF is correct:

        REQUIRE(time.get_npf() == 22675);

        // Set a new NPF:

        time.set_npf(50);

        // Ensure new NPF is accurate:

        REQUIRE(time.get_npf() == 50);
    }

    SECTION("Channels", "Ensures we can properly get and set the number of channels") {

        // Ensure default channels is correct:

        REQUIRE(time.get_channels() == 1);

        // Set new channel number:

        time.set_channels(5);

        // Ensure new channel is correct:

        REQUIRE(time.get_channels() == 5);
    }

    SECTION("Samples", "Ensures we can alter the current sample count") {

        // Ensure initial sample value is correct:

        REQUIRE(time.get_sample() == 0);

        // Set sample to new value:

        time.set_sample(9);

        // Ensure new sample is correct:

        REQUIRE(time.get_sample() == 9);

        // Increment sample:

        time.inc_sample();

        // Ensure new sample is right:

        REQUIRE(time.get_sample() == 10);

        // Add a large number to the number of samples:

        time.add_sample(10);

        // Finally, ensure new sample is right:

        REQUIRE(time.get_sample() == 20);
    }

    SECTION("Reset", "Ensures the ChainTimer is properly reset") {

        // Set some crazy values:

        time.set_channels(7);
        time.set_npf(554);
        time.set_sample(500);

        // Now, reset:

        time.reset();

        // Ensure values are default:

        REQUIRE(time.get_channels() == 1);
        REQUIRE(time.get_sample() == 0);
        REQUIRE(time.get_npf() == 0);
    }

    // Set some values for testing:

    int const npf = 5;
    int const samples = 10;

    // Set a new NPF:

    time.set_npf(npf);

    // Set sample to something else:

    time.set_sample(samples);

    SECTION("Time", "Ensures the ChainTimer is properly determining the time") {

        // Ensure time is correct:

        REQUIRE(time.get_time() == static_cast<int64_t>(samples * npf));

        // Increment the time by one:

        time.inc_sample();

        // Ensure time is still correct:

        REQUIRE(time.get_time() == static_cast<int64_t>((samples + 1) * npf));
    }

    SECTION("Time Channels", "Ensures we can calculate time based using channel count") {

        // Set the channels to 3:

        time.set_channels(3);

        // Ensure time is accurate:

        REQUIRE(time.get_time() == 15);

        // Increment by one:

        time.inc_sample();

        // Ensure time is still 15:

        REQUIRE(time.get_time() == 15);
    }
}
