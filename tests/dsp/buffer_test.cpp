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
 * @brief A generic iterator for testing the functionality of the MAEC iterators
 * 
 * We simply iterate over a vector.
 * Not very impressive, but it serves the purpose of testing.
 * 
 */
class GenericIterator : public BaseMAECIterator<GenericIterator, long double> {

    public:

        explicit GenericIterator(const std::vector<long double>& nbuf) : buff(nbuf) {}

        void resolve_pointer() {

            // Just set the pointer to the current index:

            this->set_pointer(this->buff.data() + this->get_index());
        }

    private:

        /// Buffer we are iterating over
        std::vector<long double> buff;
};

/**
 * @brief Ensures the MAEC basic iterator operations work correctly
 *
 */
TEST(IteratorTest, BasicIterOperations) {

    // Create an iterator:

    GenericIterator iter1(idata);

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

    GenericIterator iter2(idata);
    iter2 += 3;

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
TEST(IteratorTest, BasicIterComparison) {

    // Create two iterators:

    GenericIterator iter1(idata);
    auto iter2 = iter1 + 5;

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

TEST(IteratorTest, BasicIterRead) {

    // Create an iterator:

    GenericIterator iter(idata);

    // Iterate until completion:

    for (int i = 0; i < idata.size(); ++i) {

        // Ensure current value is accurate:

        ASSERT_EQ(iter[i], idata.at(i));

        // Reset the index:

        iter.set_index(0);

        ASSERT_EQ(*(iter + i), idata.at(i));
    }
}

TEST(IteratorTest, BasicIterWrite) {

    // Create an interator:

    GenericIterator iter(idata);

    // Iterate until completion:

    for (int i = 0; i < idata.size(); ++i) {

        // Determine new value to set:

        long double val = i + 1;

        // Write new value:

        iter[i] = val;

        // Ensure value is correct:

        ASSERT_EQ(iter[i], val);
    }
}

/**
 * @brief Ensures the default Buffer constructor works
 * 
 */
TEST(BufferTest, Construct) {

    // Create the Buffer:

    const Buffer<long double> buff(1, 1);

    // Ensure channel count is valid:

    ASSERT_EQ(buff.channels(), 1);
    ASSERT_EQ(buff.size(), 1);
    ASSERT_EQ(buff.total_size(), 1);
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
    ASSERT_EQ(buff.size(), chan1.size());
    ASSERT_EQ(buff.total_size(), 1 * chan1.size());

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
    ASSERT_EQ(buff.size(), chan1.size());
    ASSERT_EQ(buff.total_size(), data.size() * chan1.size());

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
 * @brief Ensures data can be retrieved from the buffer correctly
 * 
*/
TEST(BufferTest, Retrieval) {

    // Construct a multi channel buffer:

    Buffer buff(data);

    // First, ensure channel / sample retrieval works:

    for (int chan = 0; chan < buff.channels(); ++chan) {

        // Iterate over samples:

        for (int samp = 0; samp < buff.size(); ++samp) {

            // Ensure value is accurate:

            ASSERT_DOUBLE_EQ(buff.at(chan, samp), data.at(chan).at(samp));
        }
    }

    // Now, ensure interleaved retrieval works:

    for (int i = 0; i < buff.total_size(); ++i) {

        ASSERT_DOUBLE_EQ(buff.at(i), idata.at(i));
    }
}

/**
 * @brief Ensures the getters and setters for channel/sample size work
 * 
 */
TEST(BufferTest, ChannelSize) {

    // Create a buffer with 10 samples and 5 channels:

    Buffer<long double> buff(10, 5);

    // Ensure constructor works:

    ASSERT_EQ(buff.size(), 10);
    ASSERT_EQ(buff.channels(), 5);
    ASSERT_EQ(buff.total_size(), 10 * 5);

    // Change the values:

    buff.set_channels(9);
    buff.set_size(7);

    // Ensure these values are in place:

    ASSERT_EQ(buff.size(), 7);
    ASSERT_EQ(buff.channels(), 9);
    ASSERT_EQ(buff.total_size(), 9 * 7);
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

        ASSERT_EQ(val, data.at(iter.get_channel()).at(iter.get_sample()));

        // Finally, ensure data is valid against known good set:

        ASSERT_EQ(val, sdata.at(iter.get_index()));
    }
}

/**
 * @brief Ensures we can read data sequentially backwards 
 * 
 */
TEST(BufferTest, SequentialIterReadReverse) {

    // Create a buffer:

    Buffer buff(data);

    // Iterate over it backwards:

    auto biter = sdata.rbegin();
    int traversed = 0;

    for (auto iter = buff.srbegin(); iter != buff.srend(); ++iter) {

        // Ensure current value works:

        ASSERT_DOUBLE_EQ(*iter, *biter);

        ++biter;
        ++traversed;
    }

    // Ensure we iterated over enough values:

    ASSERT_EQ(traversed, sdata.size());
}

/**
 * @brief Ensures we can read data sequentially backwards
 * 
 */
TEST(BufferTest, InterleavedIterReadReverse) {

    // Create a buffer:

    Buffer buff(data);

    // Iterate over it backwards:

    auto biter = idata.rbegin();
    int traversed = 0;

    for (auto iter = buff.irbegin(); iter != buff.irend(); ++iter) {

        // Ensure current value works:

        ASSERT_DOUBLE_EQ(*iter, *biter);

        ++biter;
        ++traversed;
    }

    // Ensure we iterated over enough values:

    ASSERT_EQ(traversed, idata.size());
}

/**
 * @brief Ensures we can read from interleaved constant iterators
 * 
 */
TEST(BufferTest, InterleavedConstant) {

    // Create a buffer:

    Buffer buff(data);

    // Iterate over it:

    for (auto iter = buff.icbegin(); iter != buff.icend(); ++iter) {

        // Read the value:

        ASSERT_DOUBLE_EQ(*iter, data.at(iter.get_channel()).at(iter.get_sample()));
    }
}

/**
 * @brief Ensures we can read from sequential constant iterators
 * 
 */
TEST(BufferTest, SequencialConstant) {

    // Create a buffer:

    Buffer buff(data);

    // Iterate over it:

    for (auto iter = buff.scbegin(); iter != buff.scend(); ++iter) {

        // Read the value:

        ASSERT_DOUBLE_EQ(*iter, data.at(iter.get_channel()).at(iter.get_sample()));
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

        // Determine the new value:

        const long double val = 10. * iter.get_index() + 1;

        // Change the current value:

        *iter = val;

        // Ensure that the value is 99:

        ASSERT_DOUBLE_EQ(*iter, val);
        ASSERT_DOUBLE_EQ(buff.at(iter.get_index()), val);
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

        // Determine the new value:

        const long double val = 10. * iter.get_index() + 1;

        // Change the current value:

        *iter = val;

        // Ensure that the value is the set value:

        ASSERT_DOUBLE_EQ(*iter, val);
        ASSERT_DOUBLE_EQ(buff.at(iter.get_channel(), iter.get_sample()), val);
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
