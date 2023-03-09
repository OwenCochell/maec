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

#include <gtest/gtest.h>
#include "chrono.hpp"

TEST(ChainTimerTest, Construct) {

    ChainTimer time;
}

TEST(ChainTimerTest, Samplerate) {

    // Create a timer:

    ChainTimer time;

    // Set the samplerate:

    time.set_samplerate(400);

    // Ensure samplerate is accurate:

    ASSERT_DOUBLE_EQ(time.get_samplerate(), 400);

}

TEST(ChainTimerTest, NPF) {

    // Create a timer:

    ChainTimer time;

    // Ensure default NPF is correct:

    ASSERT_EQ(time.get_npf(), 22675);

    // Set a new NPF:

    time.set_npf(50);

    // Ensure new NPF is accurate:

    ASSERT_EQ(time.get_npf(), 50);

}

TEST(ChainTimerTest, Channels) {

    // Create a timer:

    ChainTimer time;

    // Ensure default channels is correct:

    ASSERT_EQ(time.get_channels(), 1);

    // Set new channel number:

    time.set_channels(5);

    // Ensure new channel is correct:

    ASSERT_EQ(time.get_channels(), 5);

}

TEST(ChainTimerTest, Samples) {

    // Create a timer:

    ChainTimer time;

    // Ensure initial sample value is correct:

    ASSERT_EQ(time.get_sample(), 0);

    // Set sample to new value:

    time.set_sample(9);

    // Ensure new sample is correct:

    ASSERT_EQ(time.get_sample(), 9);

    // Increment sample:

    time.inc_sample();

    // Ensure new sample is right:

    ASSERT_EQ(time.get_sample(), 10);

    // Add a large number to the number of samples:

    time.add_sample(10);

    // Finally, ensure new sample is right:

    ASSERT_EQ(time.get_sample(), 20);

}

TEST(ChainTimerTest, Reset) {

    // Create a timer:

    ChainTimer time;

    // Set some crazy values:

    time.set_channels(7);
    time.set_npf(554);
    time.set_sample(500);

    // Now, reset:

    time.reset();

    // Ensure values are default:

    ASSERT_EQ(time.get_channels(), 1);
    ASSERT_EQ(time.get_sample(), 0);
    ASSERT_EQ(time.get_npf(), 0);

}

TEST(ChainTimerTest, Time) {

    // Create timer:

    ChainTimer time;

    // Ensure default time is right:

    ASSERT_EQ(time.get_time(), 0);

    int npf = 5;
    int samples = 10;

    // Set a new NPF:

    time.set_npf(npf);

    // Set sample to something else:

    time.set_sample(samples);

    // Ensure time is correct:

    ASSERT_EQ(time.get_time(), samples * npf);

    // Increment the time by one:

    time.inc_sample();

    // Ensure time is still correct:

    ASSERT_EQ(time.get_time(), (samples+1) * npf);
}

TEST(ChainTimerTest, TimeChannels) {

    // Create timer:

    ChainTimer time;

    int npf = 5;
    int samples = 30;

    // Set a new NPF:

    time.set_npf(npf);

    // Set the channels to 3:

    time.set_channels(3);

    // Set the number of samples:

    time.set_sample(samples);

    // Ensure time is accurate:

    ASSERT_EQ(time.get_time(), 50);

    // Increment by one:

    time.inc_sample();

    // Ensure time is still 50:

    ASSERT_EQ(time.get_time(), 50);

}
