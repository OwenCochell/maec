/**
 * @file buffer.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Tests for DSP buffers
 * @version 0.1
 * @date 2023-09-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */

// TODO:
// Test reverse iterators
// Test constant iterators
// Move generic iterators to new tests

#include "dsp/buffer.hpp"

#include <gtest/gtest.h>
#include <algorithm>

// Dummy data to work with

std::vector<long double> chan1 = {0,1,2,3,4,5,6,7,8,9};
std::vector<long double> chan2 = {10,11,12,13,14,15,16,17,18,19};
std::vector<long double> chan3 = {20,21,22,23,24,25,26,27,28,29};

std::vector<std::vector<long double>> data = {chan1, chan2, chan3};

std::vector<long double> idata = {0,10,20,1,11,21,2,12,22,3,13,23,4,14,24,5,15,25,6,16,26,7,17,27,8,18,28,9,19,29};
std::vector<long double> sdata = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29};

/**
 * @brief Ensures the default Buffer constructor works
 * 
 */
TEST(BufferTest, Construct) {

    // Create the Buffer:

    const Buffer<long double> buff(1, 1);

    // Ensure channel count is valid:

    ASSERT_EQ(buff.channels(), 1);
}

/**
 * @brief Ensures the Buffer constructor with a single channel works
 * 
 */
TEST(BufferTest, ConstructSingle) {

    // Create an Buffer with a single channel:

    Buffer<long double> buff(chan1);

    // Ensure channel number is valid:

    ASSERT_EQ(buff.channels(), 1);

    // Ensure single channel is valid:

    ASSERT_TRUE(std::equal(chan1.begin(), chan1.end(), buff.sbegin()));
}

/**
 * @brief Ensures the Buffer constructor with multiple channels works
 * 
 */
TEST(BufferTest, ConstructMulti) {

    // Create an Buffer with multiple channels:

    Buffer<long double> buff(data);

    // Ensure channel number if valid:

    ASSERT_EQ(buff.channels(), data.size());

    // Check split format is valid:

    auto iter1 = buff.sbegin();
    iter1.set_channel(0);
    auto iter2 = buff.sbegin();
    iter2.set_channel(1);
    auto iter3 = buff.sbegin();
    iter3.set_channel(2);

    ASSERT_TRUE(std::equal(chan1.begin(), chan1.end(), iter1));
    ASSERT_TRUE(std::equal(chan2.begin(), chan2.end(), iter2));
    ASSERT_TRUE(std::equal(chan3.begin(), chan3.end(), iter3));
}

/**
 * @brief Ensures Buffer sample rate methods work correctly
 * 
 */
TEST(BufferTest, SampleRate) {

    // Create the Buffer:

    Buffer<long double> buff(1, 1);

    // Ensure default sample rate is valid:

    ASSERT_EQ(buff.get_samplerate(), 44100);

    // Set the sample rate:

    buff.set_samplerate(123456);

    // Check the sample rate:

    ASSERT_EQ(buff.get_samplerate(), 123456);
}

/**
 * @brief Ensures the Buffer size method works correctly
 * 
 */
TEST(BufferTest, Size) {

    // Create the Buffer:

    Buffer<long double> buff(1, 1);

    // Ensure the size is valid:

    ASSERT_EQ(buff.size(), 1);

    // Do some other constructor stuff:

    Buffer<long double> buff2(data);

    ASSERT_EQ(buff2.size(), 10);
}

/**
 * @brief Ensures the MAEC basic iterator operations work correctly
 * 
 */
TEST(BufferTest, BasicIterOperations) {

    // Create an audio buffer:

    Buffer buff(data);

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

/**
 * @brief Ensures the MAEC basic iterator comparison operations work correctly
 * 
 */
TEST(BufferTest, BasicIterComparison) {

    // Create an audio buffer:

    Buffer buff(data);

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

/**
 * @brief Ensures the Buffer interleaved iterator read operations work correctly
 * 
 */
TEST(BufferTest, InterleavedIterRead) {

    // Create the Buffer:

    Buffer buff(data);

    // Iterate over said buffer sequentially:

    for(auto iter = buff.ibegin(); iter != buff.iend(); ++iter) {

        // Get the current value:

        const long double val = *iter;

        // Ensure this value is correct:

        ASSERT_EQ(val, data.at(iter.get_channel()).at(iter.get_sample()));

        // Finally, ensure data is valid against known good set:

        ASSERT_EQ(val, idata.at(iter.get_index()));
    }
}

/**
 * @brief Ensures the Buffer sequential iterator read operations work correctly
 * 
 */
TEST(BufferTest, SequentialIterRead) {

    // Create the Buffer:

    Buffer buff(data);

    // Iterate over said buffer:

    for(auto iter = buff.sbegin(); iter != buff.send(); ++iter) {

        // Get the current value:

        const long double val = *iter;

        // Ensure this value is correct:

        ASSERT_EQ(val, data.at(iter.get_channel()).at(iter.get_sample()));  // TODO: This is wrong

        // Finally, ensure data is valid against known good set:

        ASSERT_EQ(val, sdata.at(iter.get_index()));
    }
}

/**
 * @brief Ensures the Buffer interleaved iterator write operations work correctly
 * 
 */
TEST(BufferTest, InterleavedIterWrite) {

    // Create a buffer with dummy data:

    Buffer buff(data);

    // Iterate over interleaved data:

    for(auto iter = buff.ibegin(); iter != buff.iend(); ++iter) {

        // Change the current value:

        *iter = 99;

        // Ensure that the value is 99:

        ASSERT_TRUE(buff.at(iter.get_channel())->at(iter.get_sample()) == 99);  // TODO: This is wrong
    }

    // Now, use an algorithm method:

    std::fill(buff.ibegin(), buff.iend(), 454);

    // Finally, ensure that all values are 454

    ASSERT_TRUE(std::all_of(buff.ibegin(), buff.iend(), [](long double num){ return num == 454.0; }));
}

/**
 * @brief Ensures the Buffer sequential iterator write operations work correctly
 * 
 */
TEST(BufferTest, SequentialIterWrite) {

    // Create a buffer with dummy data:

    Buffer buff(data);

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

    ASSERT_TRUE(std::all_of(buff.sbegin(), buff.send(), [](long double num){ return num == 454.0; }));
}

/**
 * @brief Ensures the Buffer interleaved iterator seek operations work correctly
 * 
 */
TEST(BufferTest, InterleavedIterSeek) {

    // Create an audio buffer:

    Buffer buff(data);

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

/**
 * @brief Ensures the Buffer sequential iterator seek operations work correctly
 * 
 */
TEST(BufferTest, SequentialIterSeek) {

    // Create an audio buffer:

    Buffer buff(data);

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

/**
 * @brief Ensures default construction of a RingBuffer works as expected.
 * 
 */
TEST(RingBufferTest, Construct) {

    // Create a RinBuffer:

    const RingBuffer<long double> ring;

    // Ensure size is accurate:

    ASSERT_EQ(ring.size(), 0);
}

/**
 * @brief Ensures construction of a RingBuffer with a size works as expected.
 * 
 */
TEST(RingBufferTest, ConstructSize) {

    // Create a RingBuffer:

    const RingBuffer<long double> ring(10);

    // Ensure size is accurate:

    ASSERT_EQ(ring.size(), 10);
}

/**
 * @brief Ensures construction of a RingBuffer with data works as expected.
 * 
 */
TEST(RingBufferTest, ConstructData) {

    // Create a RingBuffer:

    const RingBuffer<long double> ring(chan1);

    // Ensure size is accurate:

    ASSERT_EQ(ring.size(), 10);
}

/**
 * @brief Ensures RingBuffer reserve works correctly
 * 
 */
TEST(RingBufferTest, Reserve) {

    // Create a RingBuffer:

    RingBuffer<long double> ring;

    // Reserve some space:

    ring.reserve(10);

    // Ensure size is accurate:

    ASSERT_EQ(ring.size(), 10);
}

/**
 * @brief Ensures RingBuffer data can be accessed correctly
 * 
 */
TEST(RingBufferTest, Access) {

    // Create a RingBuffer:

    RingBuffer<long double> ring(chan1);

    // Access some values:

    for (int i = 0; i < ring.size() * 2; ++i) {

        ASSERT_DOUBLE_EQ(ring[i], chan1[i % ring.size()]);
        ASSERT_DOUBLE_EQ(ring[i % ring.size()], chan1[i % ring.size()]);
    }
}

/**
 * @brief Ensures RingBuffer iterators can read correctly
 * 
 */
TEST(RingBufferTest, IterRead) {

    // Create a RingBuffer:

    RingBuffer<long double> ring(chan1);

    // Iterate over the values:

    auto ring_iter = ring.begin();

    for (int i = 0; i < ring.size() * 2; ++i) {

        ASSERT_DOUBLE_EQ(ring_iter[i], ring[i]);
        ASSERT_DOUBLE_EQ(ring_iter[i % ring.size()], ring[i % ring.size()]);
    }
}

/**
 * @brief Ensures the RingBuffer iterators can write correctly
 * 
 */
TEST(RingBufferTest, IterWrite) {

    // Create a RingBuffer:

    RingBuffer<long double> ring(chan1);

    // Iterate over the values:

    auto ring_iter = ring.begin();

    for (int i = 0; i < ring.size() * 2; ++i) {

        // Write a new value:

        ring_iter[i] = 99.;

        // Ensure the value is correct:

        ASSERT_DOUBLE_EQ(ring_iter[i], 99.);
    }

    // Now, use an algorithm method:

    std::fill(ring.begin(), ring.end(), 454);

    // Finally, ensure that all values are 454

    ASSERT_TRUE(std::all_of(ring.begin(), ring.end(), [](long double num){ return num == 454.0; }));
}
