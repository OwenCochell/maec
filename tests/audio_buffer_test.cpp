/**
 * @file audio_buffer_test.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Test files for AudioBuffers
 * @version 0.1
 * @date 2022-11-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <gtest/gtest.h>
#include "audio_buffer.hpp"

// Dummy data to work with
AudioChannel chan1 = {0,1,2,3,4,5,6,7,8,9};
AudioChannel chan2 = {10,11,12,13,14,15,16,17,18,19};
AudioChannel chan3 = {20,21,22,23,24,25,26,27,28,29};

std::vector<AudioChannel> data = {chan1, chan2, chan3};

AudioChannel idata = {0.0,10.0,20.0,1.0,11.0,21.0,2,12.0,22.0,3.0,13.0,23.0,4.0,14.0,24.0,5.0,15.0,25.0,6.0,16.0,26.0,7.0,17.0,27.0,8.0,18.0,28.0,9.0,19.0,29.0};
AudioChannel sdata = {0.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0,11.0,12.0,13.0,14.0,15.0,16.0,17.0,18.0,19.0,20.0,21.0,22.0,23.0,24.0,25.0,26.0,27.0,28.0,29.0};

TEST(AudioBufferTest, Construct) {

    // Create the AudioBuffer:

    AudioBuffer buff(1,1);

}

TEST(AudioBufferTest, ConstructSingle)
{
    // Create an AudioBuffer with a single channel:

    AudioBuffer buff(chan1);

    // Ensure channel number is valid:

    ASSERT_EQ(buff.get_channel_count(), 1);
}

TEST(AudioBufferTest, ConstructMulti)
{
    // Create an AudioBuffer with multiple channels:

    AudioBuffer buff(data);

    // Ensure channel number if valid:

    ASSERT_EQ(buff.get_channel_count(), data.size());
}

TEST(AudioBufferTest, SampleRate) {

    // Create the AudioBuffer:

    AudioBuffer buff(1,1);

    // Ensure default sample rate is valid:

    ASSERT_EQ(buff.get_samplerate(), 4400);

    // Set the sample rate:

    buff.set_samplerate(123456);

    // Check the sample rate:

    ASSERT_EQ(buff.get_samplerate(), 123456);

}

TEST(AudioBufferTest, InterleavedIteration) {

    // Create the AudioBuffer:

    AudioBuffer buff(data);

    // Iterate over said buffer:

    for(auto iter = buff.ibegin(); iter != buff.iend(); ++iter) {

        // Get the current value:

        long double val = *iter;

        // Ensure this value is correct:

        ASSERT_EQ(val, data.at(iter.get_channel()).at(iter.get_sample()));

        // Finally, ensure data is valid against known good set:

        ASSERT_EQ(val, idata.at(iter.get_index()));
    }
}

TEST(AudioBufferTest, SequentialIteration) {

    // Create the AudioBuffer:

    AudioBuffer buff(data);

    // Iterate over said buffer:

    for(auto iter = buff.sbegin(); iter != buff.send(); ++iter) {

        // Get the current value:

        long double val = *iter;

        // Ensure this value is correct:

        ASSERT_EQ(val, data.at(iter.get_channel()).at(iter.get_position()));

        // Finally, ensure data is valid against known good set:

        ASSERT_EQ(val, sdata.at(iter.get_index()));
    }
}
