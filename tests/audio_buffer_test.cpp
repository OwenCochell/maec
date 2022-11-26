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

TEST(AudioBufferTest, Construct) {

    // Create the AudioBuffer:

    AudioBuffer buff;

}

TEST(AudioBufferTest, ConstructSize) {

    // Create the AudioBuffer:
    // TODO: Fix this test when better constructors are implemented!

    AudioBuffer buff(100);

}

TEST(AudioBufferTest, SampleRate) {

    // Create the AudioBuffer:

    AudioBuffer buff;

    // Set the sample rate:

    buff.set_samplerate(123456);

    // Check the sample rate:

    ASSERT_EQ(buff.get_samplerate(), 123456);

}

TEST(AudioBufferTest, InterleavedIteration) {

    // Create the AudioBuffer:

    AudioBuffer buff;
}