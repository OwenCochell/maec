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
#include <algorithm>
#include "audio_buffer.hpp"

// Dummy data to work with
AudioChannel chan1 = {0,1,2,3,4,5,6,7,8,9};
AudioChannel chan2 = {10,11,12,13,14,15,16,17,18,19};
AudioChannel chan3 = {20,21,22,23,24,25,26,27,28,29};

std::vector<AudioChannel> data = {chan1, chan2, chan3};

AudioChannel idata = {0,10,20,1,11,21,2,12,22,3,13,23,4,14,24,5,15,25,6,16,26,7,17,27,8,18,28,9,19,29};
AudioChannel sdata = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29};

TEST(AudioBufferTest, Construct) {

    // Create the AudioBuffer:

    AudioBuffer buff(1,1);

    // Ensure channel count is valid:

    ASSERT_EQ(buff.get_channel_count(), 1);

}

TEST(AudioBufferTest, ConstructSingle) {

    // Create an AudioBuffer with a single channel:

    AudioBuffer buff(chan1);

    // Ensure channel number is valid:

    ASSERT_EQ(buff.get_channel_count(), 1);

    // Ensure single channel is valid:

    ASSERT_TRUE(std::equal(chan1.begin(), chan1.end(), buff.chbegin()->begin()));
}

TEST(AudioBufferTest, ConstructMulti) {

    // Create an AudioBuffer with multiple channels:

    AudioBuffer buff(data);

    // Ensure channel number if valid:

    ASSERT_EQ(buff.get_channel_count(), data.size());

    // Check split format is valid:

    auto iter = buff.chbegin();

    ASSERT_TRUE(std::equal(chan1.begin(), chan1.end(), (iter++)->begin()));
    ASSERT_TRUE(std::equal(chan2.begin(), chan2.end(), (iter++)->begin()));
    ASSERT_TRUE(std::equal(chan3.begin(), chan3.end(), (iter++)->begin()));

}

TEST(AudioBufferTest, SampleRate) {

    // Create the AudioBuffer:

    AudioBuffer buff(1,1);

    // Ensure default sample rate is valid:

    ASSERT_EQ(buff.get_samplerate(), 44100);

    // Set the sample rate:

    buff.set_samplerate(123456);

    // Check the sample rate:

    ASSERT_EQ(buff.get_samplerate(), 123456);

}

TEST(AudioBufferTest, Size) {

    // Create the AudioBuffer:

    AudioBuffer buff(1,1);

    // Ensure the size is valid:

    ASSERT_EQ(buff.size(), 1);

    // Do some other constructor stuff:

    AudioBuffer buff2(data);

    ASSERT_EQ(buff2.size(), 10);

}

TEST(AudioBufferTest, BasicIterOperations) {

    // Create an audio buffer:

    AudioBuffer buff(data);

    // Create an iterator to work with:

    auto iter1 = buff.ibegin();

    // First, ensure default index methods work:

    iter1.set_index(2);

    ASSERT_EQ(iter1.get_index(), 2);

    // Next, check subscripting operators, forward:

    ASSERT_EQ(iter1[7], idata.at(7));
    ASSERT_EQ(iter1.get_index(), 7);

    // And backward:

    ASSERT_EQ(iter1[2], idata.at(2));
    ASSERT_EQ(iter1.get_index(), 2);

    // Next, ensure increments work:

    iter1++;
    ASSERT_EQ(iter1.get_index(), 3);

    ++iter1;
    ASSERT_EQ(iter1.get_index(), 4);

    // Ensure decrements work:
    // (Also test iterator to int conversion going forward)

    iter1--;
    ASSERT_EQ(iter1, 3);

    --iter1;
    ASSERT_EQ(iter1, 2);

    // Now, alter index via operators:

    iter1 = iter1 + 5;
    ASSERT_EQ(iter1, 7);

    iter1 = iter1 - 5;
    ASSERT_EQ(iter1, 2);

    iter1 += 6;
    ASSERT_EQ(iter1, 8);

    iter1 -= 6;
    ASSERT_EQ(iter1, 2);

    // Finally, do the same as above but with iterators:

    auto iter2 = buff.ibegin() + 3;

    iter1 = iter1 + iter2;
    ASSERT_EQ(iter1, 5);

    iter1 = iter1 - iter2;
    ASSERT_EQ(iter1, 2);

    iter1 += iter2;
    ASSERT_EQ(iter1, 5);

    iter1 -= iter2;
    ASSERT_EQ(iter1, 2);

}

TEST(AudioBufferTest, BasicIterComparison) {

    // Create an audio buffer:

    AudioBuffer buff(data);

    // Get two iterators
    // (Type should be irrelevant, both share the same features)

    auto iter1 = buff.ibegin();
    auto iter2 = buff.ibegin() + 5;

    // Checks when iter1 < iter2

    ASSERT_TRUE(iter1 < iter2);
    ASSERT_FALSE(iter1 > iter2);
    ASSERT_TRUE(iter1 <= iter2);
    ASSERT_FALSE(iter1 >= iter2);
    ASSERT_FALSE(iter1 == iter2);
    ASSERT_TRUE(iter1 != iter2);

    // Checks when iter1 > iter2

    iter1 = iter1 + 10;

    ASSERT_FALSE(iter1 < iter2);
    ASSERT_TRUE(iter1 > iter2);
    ASSERT_FALSE(iter1 <= iter2);
    ASSERT_TRUE(iter1 >= iter2);
    ASSERT_FALSE(iter1 == iter2);
    ASSERT_TRUE(iter1 != iter2);

    // Checks when iter1 == iter2

    iter2 = iter2 + 5;

    ASSERT_FALSE(iter1 < iter2);
    ASSERT_FALSE(iter1 > iter2);
    ASSERT_TRUE(iter1 <= iter2);
    ASSERT_TRUE(iter1 >= iter2);
    ASSERT_TRUE(iter1 == iter2);
    ASSERT_FALSE(iter1 != iter2);

}

TEST(AudioBufferTest, InterleavedIterRead) {

    // Create the AudioBuffer:

    AudioBuffer buff(data);

    // Iterate over said buffer sequentially:

    for(auto iter = buff.ibegin(); iter != buff.iend(); ++iter) {

        // Get the current value:

        long double val = *iter;

        // Ensure this value is correct:

        ASSERT_EQ(val, data.at(iter.get_channel()).at(iter.get_sample()));

        // Finally, ensure data is valid against known good set:

        ASSERT_EQ(val, idata.at(iter.get_index()));
    }
}

TEST(AudioBufferTest, SequentialIterRead) {

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

TEST(AudioBufferTest, InterleavedIterWrite) {

    // Create a buffer with dummy data:

    AudioBuffer buff(data);

    // Iterate over interleaved data:

    for(auto iter = buff.ibegin(); iter != buff.iend(); ++iter) {

        // Change the current value:

        *iter = 99;

        // Ensure that the value is 99:

        ASSERT_TRUE(buff.at(iter.get_channel())->at(iter.get_sample()) == 99);
    }

    // Now, use an algorithm method:

    std::fill(buff.ibegin(), buff.iend(), 454);

    // Finally, ensure that all values are 454

    std::all_of(buff.ibegin(), buff.iend(), [](long double num){ return num == 454.0; });

}

TEST(AudioBufferTest, SequentialIterWrite) {

    // Create a buffer with dummy data:

    AudioBuffer buff(data);

    // Iterate over interleaved data:

    for(auto iter = buff.sbegin(); iter != buff.send(); ++iter) {

        // Change the current value:

        *iter = 99;

        // Ensure that the value is 99:

        ASSERT_TRUE(buff.at(iter.get_channel())->at(iter.get_position()) == 99);
    }

    // Now, use an algorithm method:

    std::fill(buff.sbegin(), buff.send(), 454);

    // Finally, ensure that all values are 454

    std::all_of(buff.sbegin(), buff.send(), [](long double num){ return num == 454.0; });

}

TEST(AudioBufferTest, InterleavedIterSeek) {

    // Create an audio buffer:

    AudioBuffer buff(data);

    // Get an iterator:

    auto iter = buff.ibegin();

    // Seek to a specific sample:

    iter.set_sample(3);

    // Ensure the index is correct:

    ASSERT_EQ(iter.get_index(), 9);

    // Next, seek to a specific position:

    iter.set_position(1, 4);

    // Finally, ensure this position is correct:

    ASSERT_EQ(iter.get_index(), 13);

}

TEST(AudioBufferTest, SequentialIterSeek) {

    // Create an audio buffer:

    AudioBuffer buff(data);

    // Get an iterator:

    auto iter = buff.sbegin();

    // Seek to a specific channel:

    iter.set_channel(1);

    // Ensure the index is correct:

    ASSERT_EQ(iter.get_index(), 10);

    // Next, seek to a specific position:

    iter.set_position(1, 4);

    // Finally, ensure this position is correct:

    ASSERT_EQ(iter.get_index(), 14);

}
