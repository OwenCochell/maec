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

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "dsp/buffer.hpp"

#include <algorithm>
#include <utility>

// Dummy data to work with

std::vector<long double> chan1 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
std::vector<long double> chan2 = {10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
std::vector<long double> chan3 = {20, 21, 22, 23, 24, 25, 26, 27, 28, 29};

std::vector<std::vector<long double>> data = {chan1, chan2, chan3};

std::vector<long double> idata = {0,  10, 20, 1,  11, 21, 2,  12, 22, 3,
                                  13, 23, 4,  14, 24, 5,  15, 25, 6,  16,
                                  26, 7,  17, 27, 8,  18, 28, 9,  19, 29};
std::vector<long double> sdata = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                                  10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                                  20, 21, 22, 23, 24, 25, 26, 27, 28, 29};

/**
 * @brief A generic iterator for testing the functionality of the MAEC iterators
 *
 * We simply iterate over a vector.
 * Not very impressive, but it serves the purpose of testing.
 *
 */
class GenericIterator
    : public BaseMAECIterator<GenericIterator, long double, false> {

   public:
    explicit GenericIterator(std::vector<long double> nbuf)
        : buff(std::move(nbuf)) {}

    long double& resolve_pointer(int index) {
        // Just set the pointer to the current index:
        return this->buff[index];
    }

    std::vector<long double>* get_buffer() { return &(this->buff); }

   private:
    /// Buffer we are iterating over
    std::vector<long double> buff;
};

/**
 * @brief A generic iterator for testing the functionality of MAEC iterators
 *
 * We simply iterator over a vector.
 * Again, not very impressive, but it serves the purpose of testing.
 * This iterator is configured to be constant and read only.
 *
 */
class ConstGenericIterator
    : public BaseMAECIterator<ConstGenericIterator, long double, true> {
   public:
    explicit ConstGenericIterator(std::vector<long double> nbuf)
        : buff(std::move(nbuf)) {}

    const long double& resolve_pointer(int index) const {

        // Just set the pointer to current index:

        return *(this->buff.data() + index);
    }

    const std::vector<long double>* get_buffer() const { return &(this->buff); }

   private:
    /// Buffer we are iterating over
    const std::vector<long double> buff;
};

TEST_CASE("Iterator Test", "[buff][iter]") {

    // Create an iterator:

    GenericIterator iter1(idata);

    SECTION("Iterator Operations", "Ensures basic iterator operations work correctly") {

        // Get a pointer to the internal buffer:

        auto* buff = iter1.get_buffer();

        // First, ensure default index methods work:

        iter1.set_index(2);

        REQUIRE(iter1.get_index() == 2);

        // Next, check subscripting operators, forward:

        REQUIRE(iter1[7] == buff->at(7));
        REQUIRE(iter1.get_index() == 2);

        // And backward:

        REQUIRE(iter1[2] == buff->at(2));
        REQUIRE(iter1.get_index() == 2);

        // Next, ensure increments work:

        iter1++;
        REQUIRE(iter1.get_index() == 3);

        ++iter1;
        REQUIRE(iter1.get_index() == 4);

        // Ensure decrements work:
        // (Also test iterator to int conversion going forward)

        iter1--;
        REQUIRE(iter1 == 3);

        --iter1;
        REQUIRE(iter1 == 2);

        // Now, alter index via operators:

        iter1 = iter1 + 5;
        REQUIRE(iter1 == 7);

        iter1 = iter1 - 5;
        REQUIRE(iter1 == 2);

        iter1 += 6;
        REQUIRE(iter1 == 8);

        iter1 -= 6;
        REQUIRE(iter1 == 2);

        // Finally, do the same as above but with iterators:

        GenericIterator iter2(idata);
        iter2 += 3;

        iter1 = iter1 + iter2;
        REQUIRE(iter1 == 5);

        iter1 = iter1 - iter2;
        REQUIRE(iter1 == 2);

        iter1 += iter2;
        REQUIRE(iter1 == 5);

        iter1 -= iter2;
        REQUIRE(iter1 == 2);
    }

    SECTION("Iterator Comparison", "Ensures iterators can be compared") {

        // Create two iterators:

        auto iter2 = iter1 + 5;

        // Checks when iter1 < iter2

        REQUIRE(iter1 < iter2);
        REQUIRE_FALSE(iter1 > iter2);
        REQUIRE(iter1 <= iter2);
        REQUIRE_FALSE(iter1 >= iter2);
        REQUIRE_FALSE(iter1 == iter2);
        REQUIRE(iter1 != iter2);

        // Checks when iter1 > iter2

        iter1 = iter1 + 10;

        REQUIRE_FALSE(iter1 < iter2);
        REQUIRE(iter1 > iter2);
        REQUIRE_FALSE(iter1 <= iter2);
        REQUIRE(iter1 >= iter2);
        REQUIRE_FALSE(iter1 == iter2);
        REQUIRE(iter1 != iter2);

        // Checks when iter1 == iter2

        iter2 = iter2 + 5;

        REQUIRE_FALSE(iter1 < iter2);
        REQUIRE_FALSE(iter1 > iter2);
        REQUIRE(iter1 <= iter2);
        REQUIRE(iter1 >= iter2);
        REQUIRE(iter1 == iter2);
        REQUIRE_FALSE(iter1 != iter2);
    }

    SECTION("Iterator Read", "Ensures iterators can be read") {

        // Get internal buffer:

        auto* buff = iter1.get_buffer();

        // Iterate until completion:

        for (int i = 0; i < idata.size(); ++i) {

            // Ensure current value is accurate:

            REQUIRE_THAT(iter1[i], Catch::Matchers::WithinAbs(buff->at(i), 0.0001));

            REQUIRE_THAT(*(iter1 + i), Catch::Matchers::WithinAbs(buff->at(i), 0.0001));
        }
    }

    SECTION("Iterator Write", "Ensures iterators can be written") {

        // Get a pointer to the buffer:

        auto* buff = iter1.get_buffer();

        // Iterate until completion:

        for (int i = 0; i < idata.size(); ++i) {

            // Determine new value to set:

            const long double val = i + 1;

            // Write new value:

            iter1[i] = val;

            // Ensure value is correct:

            REQUIRE_THAT(iter1[i], Catch::Matchers::WithinAbs(val, 0.0001));
            REQUIRE_THAT(iter1[i], Catch::Matchers::WithinAbs(buff->at(i), 0.0001));

            // Now, write using a different method:

            auto iter2 = iter1 + i;
            auto* buff2 = iter2.get_buffer();

            *(iter2) = val + 1;

            // Ensure new value is correct:

            REQUIRE_THAT(*(iter2), Catch::Matchers::WithinAbs(val + 1, 0.0001));
            REQUIRE_THAT(*(iter2), Catch::Matchers::WithinAbs(buff2->at(i), 0.0001));
        }
    }

    SECTION("Constant Iterator Read", "Ensures the constant iterator reads correctly") {

        // Create ConstantGenericIterator:

        ConstGenericIterator iter(idata);

        // Get pointer to internal buffer:

        const auto* buff = iter.get_buffer();

        // Iterate until complete:

        for (int i = 0; i < idata.size(); ++i) {

            // Ensure value is correct:

            REQUIRE_THAT(iter[i], Catch::Matchers::WithinAbs(buff->at(i), 0.0001));

            REQUIRE_THAT(*(iter + i), Catch::Matchers::WithinAbs(buff->at(i), 0.0001));
        }
    }
}

TEST_CASE("Buffer Test", "[buff]") {

    SECTION("Construct", "Ensures a buffer can be constructed") {

        // Create the Buffer:

        const Buffer<long double> buff(1, 1);

        // Ensure channel count is valid:

        REQUIRE(buff.channels() == 1);
        REQUIRE(buff.channel_capacity() == 1);
        REQUIRE(buff.total_capacity() == 1);
    }

    SECTION("Construct Single Channel", "Ensures a buffer can be created with a single channel") {

        // Create an Buffer with a single channel:

        Buffer<long double> buff(chan1);

        // Ensure channel number is valid:

        REQUIRE(buff.channels() == 1);
        REQUIRE(buff.channel_capacity() == chan1.size());
        REQUIRE(buff.total_capacity() == 1 * chan1.size());

        // Ensure single channel is valid:

        REQUIRE(std::equal(chan1.begin(), chan1.end(), buff.sbegin()));
    }

    SECTION("Construct Mutli-Channel", "Ensures a buffer can be created with multiple channels") {

        // Create an Buffer with multiple channels:

        Buffer<long double> buff(data);

        // Ensure channel number if valid:

        REQUIRE(buff.channels() == data.size());
        REQUIRE(buff.channel_capacity() == chan1.size());
        REQUIRE(buff.total_capacity() == data.size() * chan1.size());

        // Check split format is valid:

        auto iter1 = buff.sbegin();
        iter1.set_channel(0);
        auto iter2 = buff.sbegin();
        iter2.set_channel(1);
        auto iter3 = buff.sbegin();
        iter3.set_channel(2);

        REQUIRE(std::equal(chan1.begin(), chan1.end(), iter1));
        REQUIRE(std::equal(chan2.begin(), chan2.end(), iter2));
        REQUIRE(std::equal(chan3.begin(), chan3.end(), iter3));
    }

    SECTION("Samplerate", "Ensures we can get and set the samplerate") {

        // Create the Buffer:

        Buffer<long double> buff(1, 1);

        // Ensure default sample rate is valid:

        REQUIRE(buff.get_samplerate() == 44100);

        // Set the sample rate:

        buff.set_samplerate(123456);

        // Check the sample rate:

        REQUIRE(buff.get_samplerate() == 123456);
    }

    SECTION("Retrieval", "Ensures data can be retrieved from the buffer") {

        // Construct a multi channel buffer:

        Buffer buff(data);

        // First, ensure channel / sample retrieval works:

        for (int chan = 0; chan < buff.channels(); ++chan) {

            // Iterate over samples:

            for (int samp = 0; samp < buff.channel_capacity(); ++samp) {

                // Ensure value is accurate:

                REQUIRE_THAT(buff.at(chan, samp), Catch::Matchers::WithinAbs(data.at(chan).at(samp), 0.0001));
            }
        }

        // Now, ensure interleaved retrieval works:

        for (int i = 0; i < buff.total_capacity(); ++i) {

            REQUIRE_THAT(buff.at(i), Catch::Matchers::WithinAbs(idata.at(i), 0.0001));
        }
    }

    SECTION("Fill", "Ensures a buffer can be filled") {

        // Create an empty buffer:

        Buffer<int> buff(10, 3);

        // Fill with zeros:

        buff.fill();

        // Ensure each value is zero:

        for (int i = 0; i < buff.total_capacity(); ++i) {

            // Ensure current value is zero:

            REQUIRE(0 == buff.at(i));
        }
    }

    SECTION("Fill Partial", "Ensures a buffer can be partially filled") {

        // Create a partial buffer:

        Buffer<long double> buff(chan1);

        // Make the capacity bigger:

        buff.set_channel_capacity(10);
        buff.set_channels(3);

        // Now, fill with zero:

        buff.fill();

        // Iterate over the first section:

        for (int i = 0; i < chan1.size(); ++i) {

            // Ensure value is accurate:

            REQUIRE_THAT(buff.at(i), Catch::Matchers::WithinAbs(chan1.at(i), 0.0001));
        }

        // Now, ensure the rest is filled with zeros:

        for (int i = static_cast<int>(chan1.size()); i < buff.total_capacity();
             ++i) {

            // Ensure value is zero:

            REQUIRE_THAT(buff.at(i), Catch::Matchers::WithinAbs(0, 0.0001));
        }
    }

    SECTION("Channel Size", "Ensures the channels can be get/set") {

        // Create a buffer with 10 samples and 5 channels:

        Buffer<long double> buff(10, 5);

        // Ensure constructor works:

        REQUIRE(buff.channel_capacity() == 10);
        REQUIRE(buff.channels() == 5);
        REQUIRE(buff.total_capacity() == static_cast<int64_t>(10 * 5));

        // Change the values:

        buff.set_channels(9);
        buff.set_channel_capacity(7);

        // Ensure these values are in place:

        REQUIRE(buff.channel_capacity() == 7);
        REQUIRE(buff.channels() == 9);
        REQUIRE(buff.total_capacity() == static_cast<int64_t>(9 * 7));
    }

    SECTION("Iterator Tests", "Tests the iterators included with the buffer") {

        // Create the Buffer:

        Buffer buff(data);

        SECTION("Interleaved Iterator Read", "Ensures the interleaved iterator can read correctly") {

            // Iterate over said buffer sequentially:

            for (auto iter = buff.ibegin(); iter != buff.iend(); ++iter) {

                // Get the current value:

                const long double val = *iter;

                // Ensure this value is correct:

                REQUIRE(val == data.at(iter.get_channel()).at(iter.get_sample()));

                // Finally, ensure data is valid against known good set:

                REQUIRE(val == idata.at(iter.get_index()));
            }
        }

        SECTION("Sequential Iterator Read", "Ensures the sequential iterator can read") {

            // Iterate over said buffer:

            for (auto iter = buff.sbegin(); iter != buff.send(); ++iter) {

                // Get the current value:

                const long double val = *iter;

                // Ensure this value is correct:

                REQUIRE(val == data.at(iter.get_channel()).at(iter.get_sample()));

                // Finally, ensure data is valid against known good set:

                REQUIRE(val == sdata.at(iter.get_index()));
            }
        }

        SECTION("Sequential Iterator Read Reverse",
                "Ensures the sequential iterator can be read in reverse") {

            // Iterate over it backwards:

            auto biter = sdata.rbegin();
            int traversed = 0;

            for (auto iter = buff.srbegin(); iter != buff.srend(); ++iter) {

                // Ensure current value works:

                REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(*biter, 0.0001));

                ++biter;
                ++traversed;
            }

            // Ensure we iterated over enough values:

            REQUIRE(traversed == sdata.size());
        }

        SECTION("Interleaved Iterator Read Reverse", "Ensures the Interleaved iterator can read in reverse") {

            // Iterate over it backwards:

            auto biter = idata.rbegin();
            int traversed = 0;

            for (auto iter = buff.irbegin(); iter != buff.irend(); ++iter) {

                // Ensure current value works:

                REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(*biter, 0.0001));

                ++biter;
                ++traversed;
            }

            // Ensure we iterated over enough values:

            REQUIRE(traversed == idata.size());
        }

        SECTION("Interleaved Iterator Constant", "Ensures the constant interleaved iterator can read data") {

            // Iterate over it:

            for (auto iter = buff.icbegin(); iter != buff.icend(); ++iter) {

                // Read the value:

                REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(data.at(iter.get_channel()).at(iter.get_sample()), 0.0001));
            }
        }

        SECTION("Sequential Iterator Constant", "Ensures the sequential constant iterator can read data") {

            // Iterate over it:

            for (auto iter = buff.scbegin(); iter != buff.scend(); ++iter) {

                // Read the value:

                REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(data.at(iter.get_channel()).at(iter.get_sample()), 0.0001));
            }
        }

        SECTION("Interleaved Iterator Write", "Ensures the interleaved iterator can write") {

            // Iterate over interleaved data:

            for (auto iter = buff.ibegin(); iter != buff.iend(); ++iter) {

                // Determine the new value:

                const long double val = 10. * iter.get_index() + 1;

                // Change the current value:

                *iter = val;

                // Ensure that the value is 99:

                REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(val, 0.0001));
                REQUIRE_THAT(buff.at(iter.get_index()), Catch::Matchers::WithinAbs(val, 0.0001));
            }

            // Now, use an algorithm method:

            std::fill(buff.ibegin(), buff.iend(), 454);

            // Finally, ensure that all values are 454

            bool res = std::all_of(buff.ibegin(), buff.iend(),
                                   [](long double num) { return num == 454.0; });

            REQUIRE(res);
        }

        SECTION("Sequential Iterator Write", "Ensures the sequential iterator can write") {

            // Iterate over interleaved data:

            for (auto iter = buff.sbegin(); iter != buff.send(); ++iter) {

                // Determine the new value:

                const long double val = 10. * iter.get_index() + 1;

                // Change the current value:

                *iter = val;

                // Ensure that the value is the set value:

                REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(val, 0.0001));
                REQUIRE_THAT(buff.at(iter.get_channel(), iter.get_sample()),
                             Catch::Matchers::WithinAbs(val, 0.0001));
            }

            // Now, use an algorithm method:

            std::fill(buff.sbegin(), buff.send(), 454);

            // Finally, ensure that all values are 454

            bool res = std::all_of(buff.sbegin(), buff.send(),
                                   [](long double num) { return num == 454.0; });

            REQUIRE(res);
        }

        SECTION("Interleaved Iterator Seek", "Ensures the Interleaved Iterator can seek to data") {

            // Get an iterator:

            auto iter = buff.ibegin();

            // Seek to a specific sample:

            iter.set_sample(3);

            // Ensure the index is correct:

            REQUIRE(iter.get_index() == 9);

            // Next, seek to a specific position:

            iter.set_position(1, 4);

            // Finally, ensure this position is correct:

            REQUIRE(iter.get_index() == 13);
        }

        SECTION("Sequential Iterator Seek", "Ensures the sequential iterator can seek") {

            // Get an iterator:

            auto iter = buff.sbegin();

            // Seek to a specific channel:

            iter.set_channel(1);

            // Ensure the index is correct:

            REQUIRE(iter.get_index() == 10);

            // Next, seek to a specific position:

            iter.set_position(1, 4);

            // Finally, ensure this position is correct:

            REQUIRE(iter.get_index() == 14);
        }
    }
}

TEST_CASE("Ring Buffer Test", "[buff][ring]") {

    SECTION("Construct", "Ensures the ring buffer can be constructed") {

        // Create a RinBuffer:

        const RingBuffer<long double> ring;

        // Ensure size is accurate:

        REQUIRE(ring.size() == 0);
    }

    SECTION("Construct Size", "Ensures we can construct a RingBuffer with a size") {

        // Create a RingBuffer:

        const RingBuffer<long double> ring(10);

        // Ensure size is accurate:

        REQUIRE(ring.size() == 10);
    }

    SECTION("Construct Data", "Ensures we can construct a RingBuffer with a size") {

        // Create a RingBuffer:

        const RingBuffer<long double> ring(chan1);

        // Ensure size is accurate:

        REQUIRE(ring.size() == 10);
    }

    SECTION("Reserve", "Ensures we can reserve a RingBuffer with a size") {

        // Create a RingBuffer:

        RingBuffer<long double> ring;

        // Reserve some space:

        ring.reserve(10);

        // Ensure size is accurate:

        REQUIRE(ring.size() == 10);
    }

    SECTION("Retrieval", "Ensures we can retrieve data from the RingBuffer") {

        // Create a RingBuffer:

        RingBuffer<long double> ring(chan1);

        // Access some values:

        for (int i = 0; i < ring.size() * 2; ++i) {

            REQUIRE_THAT(ring[i], Catch::Matchers::WithinAbs(chan1[i % ring.size()], 0.0001));
            REQUIRE_THAT(ring[i % ring.size()], Catch::Matchers::WithinAbs(chan1[i % ring.size()], 0.0001));
        }
    }

    SECTION("Iterator Tests", "Tests for iterator operations") {

        // Create a RingBuffer:

        RingBuffer<long double> ring(chan1);

        SECTION("Iterator Read", "Ensures the iterator can read data") {

            // Create a RingBuffer:

            RingBuffer<long double> ring(chan1);

            // Iterate over the values:

            auto ring_iter = ring.begin();

            for (int i = 0; i < ring.size() * 2; ++i) {

                REQUIRE_THAT(ring_iter[i], Catch::Matchers::WithinAbs(ring[i], 0.0001));
                REQUIRE_THAT(ring_iter[i % ring.size()], Catch::Matchers::WithinAbs(ring[i % ring.size()], 0.0001));
            }
        }

        SECTION("Iterator Write", "Ensures the iterator can write") {

            // Iterate over the values:

            auto ring_iter = ring.begin();

            for (int i = 0; i < ring.size() * 2; ++i) {

                // Write a new value:

                ring_iter[i] = 99.;

                // Ensure the value is correct:

                REQUIRE_THAT(ring_iter[i], Catch::Matchers::WithinAbs(99., 0.0001));
            }

            // Now, use an algorithm method:

            std::fill(ring.begin(), ring.end(), 454);

            // Finally, ensure that all values are 454

            bool res = std::all_of(ring.begin(), ring.end(),
                                   [](long double num) { return num == 454.0; });

            REQUIRE(res);
        }
    }
}
