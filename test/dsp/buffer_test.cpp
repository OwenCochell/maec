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

#include <algorithm>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <initializer_list>
#include <utility>
#include <vector>

#include "dsp/const.hpp"

// Dummy data to work with

namespace {

constexpr std::array<double, 10> achan1 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
constexpr std::array<double, 10> achan2 = {10, 11, 12, 13, 14,
                                           15, 16, 17, 18, 19};
constexpr std::array<double, 10> achan3 = {20, 21, 22, 23, 24,
                                           25, 26, 27, 28, 29};

std::array<double, 30> aidata = {0,  10, 20, 1,  11, 21, 2,  12, 22, 3,
                                 13, 23, 4,  14, 24, 5,  15, 25, 6,  16,
                                 26, 7,  17, 27, 8,  18, 28, 9,  19, 29};
std::array<double, 30> asdata = {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                                 10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
                                 20, 21, 22, 23, 24, 25, 26, 27, 28, 29};

std::vector<double> chan1(achan1.begin(), achan1.end());
std::vector<double> chan2(achan2.begin(), achan2.end());
std::vector<double> chan3(achan3.begin(), achan3.end());

std::vector<double> idata(aidata.begin(), aidata.end());
std::vector<double> sdata(asdata.begin(), asdata.end());

std::vector<std::vector<double>> data = {chan1, chan2, chan3};
}  // namespace

/**
 * @brief A generic iterator for testing the functionality of the MAEC iterators
 *
 * We simply iterate over a vector.
 * Not very impressive, but it serves the purpose of testing.
 *
 */
class GenericIterator
    : public BaseMAECIterator<GenericIterator, double, false> {

public:
    explicit GenericIterator(std::vector<double> nbuf)
        : buff(std::move(nbuf)) {}

    double* resolve_pointer(int index) {
        // Just set the pointer to the current index:
        return this->buff.data() + index;
    }

    std::vector<double>* get_buffer() { return &(this->buff); }

private:
    /// Buffer we are iterating over
    std::vector<double> buff;
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
    : public BaseMAECIterator<ConstGenericIterator, double, true> {
public:
    explicit ConstGenericIterator(std::vector<double> nbuf)
        : buff(std::move(nbuf)) {}

    const double* resolve_pointer(int index) const {

        // Just set the pointer to current index:

        return this->buff.data() + index;
    }

    const std::vector<double>* get_buffer() const { return &(this->buff); }

private:
    /// Buffer we are iterating over
    std::vector<double> buff;
};

TEST_CASE("Iterator Test", "[buff][iter]") {

    // Create an iterator:

    GenericIterator iter1(idata);

    SECTION("Iterator Operations",
            "Ensures basic iterator operations work correctly") {

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

            REQUIRE_THAT(iter1[i],
                         Catch::Matchers::WithinAbs(buff->at(i), 0.0001));

            REQUIRE_THAT(*(iter1 + i),
                         Catch::Matchers::WithinAbs(buff->at(i), 0.0001));
        }
    }

    SECTION("Iterator Write", "Ensures iterators can be written") {

        // Get a pointer to the buffer:

        auto* buff = iter1.get_buffer();

        // Iterate until completion:

        for (int i = 0; i < idata.size(); ++i) {

            // Determine new value to set:

            const double val = i + 1;

            // Write new value:

            iter1[i] = val;

            // Ensure value is correct:

            REQUIRE_THAT(iter1[i], Catch::Matchers::WithinAbs(val, 0.0001));
            REQUIRE_THAT(iter1[i],
                         Catch::Matchers::WithinAbs(buff->at(i), 0.0001));

            // Now, write using a different method:

            auto iter2 = iter1 + i;
            auto* buff2 = iter2.get_buffer();

            *(iter2) = val + 1;

            // Ensure new value is correct:

            REQUIRE_THAT(*(iter2), Catch::Matchers::WithinAbs(val + 1, 0.0001));
            REQUIRE_THAT(*(iter2),
                         Catch::Matchers::WithinAbs(buff2->at(i), 0.0001));
        }
    }

    SECTION("Constant Iterator Read",
            "Ensures the constant iterator reads correctly") {

        // Create ConstantGenericIterator:

        ConstGenericIterator iter(idata);

        // Get pointer to internal buffer:

        const auto* buff = iter.get_buffer();

        // Iterate until complete:

        for (int i = 0; i < idata.size(); ++i) {

            // Ensure value is correct:

            REQUIRE_THAT(iter[i],
                         Catch::Matchers::WithinAbs(buff->at(i), 0.0001));

            REQUIRE_THAT(*(iter + i),
                         Catch::Matchers::WithinAbs(buff->at(i), 0.0001));
        }
    }
}

TEST_CASE("Base Buffer Test", "[buff]") {

    // Define the container type to utilize:
    // (We just go with a vector for simplicity)

    using CONT = std::vector<double>;

    SECTION("Default Construct",
            "Ensures a buffer can be default constructed") {

        // Create the buffer:

        const BaseBuffer<CONT, double> buff;

        // Ensure defaults are correct:

        REQUIRE(buff.channels() == 1);
        REQUIRE(buff.channel_capacity() == 0);
        REQUIRE(buff.size() == 0);
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(SAMPLE_RATE, 0.0001));
    }

    SECTION("Construct", "Ensures a buffer can be constructed") {

        // Create the Buffer:

        const BaseBuffer<CONT, double> buff(1, 1);

        // Ensure channel count is valid:

        REQUIRE(buff.channels() == 1);
        REQUIRE(buff.channel_capacity() == 1);
        REQUIRE(buff.size() == 1);
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(SAMPLE_RATE, 0.0001));
    }

    SECTION("Construct Crazy",
            "Ensures a buffer can be constructed with some crazy values") {

        const int csize = 123;
        const int channels = 456;

        // Create the Buffer:

        const BaseBuffer<CONT, double> buff(csize, channels);

        // Ensure channel count is valid:

        REQUIRE(buff.channels() == channels);
        REQUIRE(buff.channel_capacity() == csize);
        REQUIRE(buff.size() == csize * channels);
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(SAMPLE_RATE, 0.0001));
    }

    SECTION("Construct Sample",
            "Ensures a buffer can be constructed with a provided sample rate") {

        // Create the buffer:

        const BaseBuffer<CONT, double> buff(1, 1, 1.0);

        // Ensure channel count is valid:

        REQUIRE(buff.channels() == 1);
        REQUIRE(buff.channel_capacity() == 1);
        REQUIRE(buff.size() == 1);
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(1., 0.0001));
    }

    SECTION("Construct Sample Crazy",
            "Ensures a buffer can be constructed with a provided sample rate "
            "and crazy values") {

        const int csize = 123;
        const int chans = 456;
        const double samp = 789.1;

        // Create the buffer:

        const BaseBuffer<CONT, double> buff(csize, chans, samp);

        // Ensure channel count is valid:

        REQUIRE(buff.channels() == chans);
        REQUIRE(buff.channel_capacity() == csize);
        REQUIRE(buff.size() == csize * chans);
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(samp, 0.0001));
    }

    SECTION("Construct Copy Single Channel",
            "Ensures a buffer can be created via copy with a single channel") {

        // Create a BaseBuffer via copying:

        BaseBuffer<CONT, double> buff(chan1);

        // Ensure channel number is valid:

        REQUIRE(buff.channels() == 1);
        REQUIRE(buff.channel_capacity() == chan1.size());
        REQUIRE(buff.size() == chan1.size());
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(SAMPLE_RATE, 0.0001));

        // Ensure single channel is valid:

        REQUIRE(std::equal(chan1.begin(), chan1.end(), buff.ibegin()));
    }

    SECTION("Construct Copy Multi Channel",
            "Ensures a buffer can be created via copy with multi channels") {

        const int channels = 2;

        // Create a BaseBuffer via copying:

        BaseBuffer<CONT, double> buff(chan1, channels);

        // Ensure channel number is valid:

        REQUIRE(buff.channels() == channels);
        REQUIRE(buff.channel_capacity() == chan1.size() / channels);
        REQUIRE(buff.size() == chan1.size());
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(SAMPLE_RATE, 0.0001));

        // Ensure single channel is valid:

        REQUIRE(std::equal(chan1.begin(), chan1.end(), buff.ibegin()));
    }

    SECTION("Construct Copy Multi Channel Sample Rate",
            "Ensures a buffer can be created via copy with multi channels and "
            "sample rate") {

        const int channels = 2;
        const double samp = 1234.;

        // Create a BaseBuffer via copying:

        BaseBuffer<CONT, double> buff(chan1, channels, samp);

        // Ensure channel number is valid:

        REQUIRE(buff.channels() == channels);
        REQUIRE(buff.channel_capacity() == chan1.size() / channels);
        REQUIRE(buff.size() == chan1.size());
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(samp, 0.0001));

        // Ensure single channel is valid:

        REQUIRE(std::equal(chan1.begin(), chan1.end(), buff.ibegin()));
    }

    SECTION("Construct Iterator Single Channel",
            "Ensures a buffer can be constructed using iterators with a single "
            "channel") {

        // Create a buffer via iterators:

        BaseBuffer<CONT, double> buff(idata.begin(), idata.end());

        // Ensure the values are correct:

        REQUIRE(buff.channels() == 1);
        REQUIRE(buff.channel_capacity() == idata.size());
        REQUIRE(buff.size() == idata.size());
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(SAMPLE_RATE, 0.0001));

        REQUIRE(std::equal(idata.begin(), idata.end(), buff.ibegin()));
    }

    SECTION("Construct Iterator Multi Channel",
            "Ensures a buffer can be constructed using iterators with multi "
            "channels") {

        const int channels = 2;

        // Create a buffer via iterators:

        BaseBuffer<CONT, double> buff(idata.begin(), idata.end(), channels);

        // Ensure the values are correct:

        REQUIRE(buff.channels() == channels);
        REQUIRE(buff.channel_capacity() == idata.size() / channels);
        REQUIRE(buff.size() == idata.size());
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(SAMPLE_RATE, 0.0001));

        REQUIRE(std::equal(idata.begin(), idata.end(), buff.ibegin()));
    }

    SECTION("Construct Iterator Multi Channel Sample Rate",
            "Ensures a buffer can be constructed using iterators with multi "
            "channels and sample rate") {

        const int channels = 2;
        const int samp = 456.;

        // Create a buffer via iterators:

        BaseBuffer<CONT, double> buff(idata.begin(), idata.end(), channels,
                                      samp);

        // Ensure the values are correct:

        REQUIRE(buff.channels() == channels);
        REQUIRE(buff.channel_capacity() == idata.size() / channels);
        REQUIRE(buff.size() == idata.size());
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(samp, 0.0001));

        REQUIRE(std::equal(idata.begin(), idata.end(), buff.ibegin()));
    }

    SECTION("Construct Initializer List Single Channel",
            "Ensures a buffer can be created via copy with single channel") {

        std::initializer_list<double> vals = {1, 2, 3, 4, 5};

        // Create a buffer via initializer list:

        BaseBuffer<CONT, double> buff = vals;

        // Ensure the values are correct:

        REQUIRE(buff.channels() == 1);
        REQUIRE(buff.channel_capacity() == 5);
        REQUIRE(buff.size() == 5);
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(SAMPLE_RATE, 0.0001));

        REQUIRE(std::equal(vals.begin(), vals.end(), buff.ibegin()));
    }

    SECTION("Construct Initializer List Multi Channel",
            "Ensures a buffer can be created via copy with multiple channels") {

        const int channels = 2;

        std::initializer_list<double> vals = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        // Create a buffer via initializer list:

        BaseBuffer<CONT, double> buff(vals, channels);

        // Ensure the values are correct:

        REQUIRE(buff.channels() == channels);
        REQUIRE(buff.channel_capacity() == 10 / 2);
        REQUIRE(buff.size() == 10);
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(SAMPLE_RATE, 0.0001));

        REQUIRE(std::equal(vals.begin(), vals.end(), buff.ibegin()));
    }

    SECTION("Construct Initializer List Multi Channel",
            "Ensures a buffer can be created via copy with multiple channels "
            "and sample rate") {

        const int channels = 2;
        const double samp = 789.;

        std::initializer_list<double> vals = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        // Create a buffer via initializer list:

        BaseBuffer<CONT, double> buff(vals, channels, samp);

        // Ensure the values are correct:

        REQUIRE(buff.channels() == channels);
        REQUIRE(buff.channel_capacity() == 10 / 2);
        REQUIRE(buff.size() == 10);
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(samp, 0.0001));

        // Ensure the values are what we expect:

        REQUIRE(std::equal(vals.begin(), vals.end(), buff.ibegin()));
    }

    SECTION("Variadic Template",
            "Ensures a buffer can be created via the variadic template "
            "constructor") {

        // Create the buffer:

        BaseBuffer<CONT, double> buff(0., 1., 2., 3., 4., 5., 6., 7., 8., 9.);

        // Ensure the values are correct:

        REQUIRE(buff.channels() == 1);
        REQUIRE(buff.channel_capacity() == 10);
        REQUIRE(buff.size() == 10);
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(SAMPLE_RATE, 0.0001));

        // Ensure the values are valid:

        for (int i = 0; i < buff.size(); ++i) {

            // Ensure the value is the same as the index:

            REQUIRE_THAT(buff.at(i), Catch::Matchers::WithinAbs(i, 0.0001));
        }
    }

    SECTION("Copy Constructor", "Ensure copy constructor works correctly") {

        const int chans = 5;
        const double samp = 1234.;

        // Create an initial buffer:

        BaseBuffer<CONT, double> buff(chan1, chans, samp);

        // Create a new vector via copy assignment:

        BaseBuffer<CONT, double> buff2(buff);

        // Ensure the vales are the same:

        REQUIRE(buff.channels() == buff2.channels());
        REQUIRE(buff.channel_capacity() == buff2.channel_capacity());
        REQUIRE(buff.size() == buff2.size());
        REQUIRE_THAT(
            buff.get_samplerate(),
            Catch::Matchers::WithinAbs(buff2.get_samplerate(), 0.0001));

        REQUIRE(std::equal(buff.begin(), buff.end(), buff2.begin()));
    }

    SECTION("Move Constructor", "Ensure move constructor works correctly") {

        const int chans = 5;
        const double samp = 1234.;

        // Create an initial buffer:

        BaseBuffer<CONT, double> buff(chan1, chans, samp);

        // Create a new vector via copy assignment:

        BaseBuffer<CONT, double> buff2(std::move(buff));

        // Ensure the vales are the same:

        REQUIRE(buff2.channels() == chans);
        REQUIRE(buff2.channel_capacity() == chan1.size() / chans);
        REQUIRE(buff2.size() == chan1.size());
        REQUIRE_THAT(buff2.get_samplerate(),
                     Catch::Matchers::WithinAbs(samp, 0.0001));

        REQUIRE(std::equal(buff2.begin(), buff2.end(), chan1.begin()));
    }

    SECTION("Copy Assignment", "Ensure copy assignment works correctly") {

        const int chans = 5;
        const double samp = 1234.;

        // Create an initial buffer:

        BaseBuffer<CONT, double> buff(chan1, chans, samp);

        // Create destination buffer:

        BaseBuffer<CONT, double> buff2;

        // Preform copy assignment:

        buff2 = buff;

        // Ensure values are the same:

        REQUIRE(buff.channels() == buff2.channels());
        REQUIRE(buff.channel_capacity() == buff2.channel_capacity());
        REQUIRE(buff.size() == buff2.size());
        REQUIRE_THAT(
            buff.get_samplerate(),
            Catch::Matchers::WithinAbs(buff2.get_samplerate(), 0.0001));

        REQUIRE(std::equal(buff.begin(), buff.end(), buff2.begin()));
    }

    SECTION("Move Assignment", "Ensure move assignment works correctly") {

        const int chans = 5;
        const double samp = 1234.;

        // Create an initial buffer:

        BaseBuffer<CONT, double> buff(chan1, chans, samp);

        // Create destination buffer:

        BaseBuffer<CONT, double> buff2;

        // Preform copy assignment:

        buff2 = std::move(buff);

        // Ensure values are the same:

        REQUIRE(buff2.channels() == chans);
        REQUIRE(buff2.channel_capacity() == chan1.size() / chans);
        REQUIRE(buff2.size() == chan1.size());
        REQUIRE_THAT(buff2.get_samplerate(),
                     Catch::Matchers::WithinAbs(samp, 0.0001));

        REQUIRE(std::equal(buff2.begin(), buff2.end(), chan1.begin()));
    }

    SECTION("Container Copy",
            "Ensures container copy assignment works correctly") {

        // Create a buffer:

        BaseBuffer<CONT, double> buff;

        // Copy some data in:

        buff = chan1;

        // Ensure the values are correct:

        REQUIRE(buff.channels() == 1);
        REQUIRE(buff.channel_capacity() == chan1.size());
        REQUIRE(buff.size() == chan1.size());
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(SAMPLE_RATE, 0.0001));

        REQUIRE(std::equal(buff.begin(), buff.end(), chan1.begin()));
    }

    SECTION("Container Move",
            "Ensures container move assignment works correctly") {

        // Create copy of channel data:

        std::vector<double> tdata = chan1;

        // Create a buffer:

        BaseBuffer<CONT, double> buff;

        // Copy some data in:

        buff = std::move(tdata);

        // Ensure the values are correct:

        REQUIRE(buff.channels() == 1);
        REQUIRE(buff.channel_capacity() == chan1.size());
        REQUIRE(buff.size() == chan1.size());
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(SAMPLE_RATE, 0.0001));

        REQUIRE(std::equal(buff.begin(), buff.end(), chan1.begin()));
    }

    SECTION("Samplerate", "Ensures we can get and set the samplerate") {

        // Create the Buffer:

        BaseBuffer<CONT, double> buff(1, 1);

        // Ensure default sample rate is valid:

        REQUIRE(buff.get_samplerate() == SAMPLE_RATE);

        // Set the sample rate:

        buff.set_samplerate(123456);

        // Check the sample rate:

        REQUIRE(buff.get_samplerate() == 123456);
    }

    SECTION("Channel Size", "Ensures the channels can be get/set") {

        // Create a buffer with 10 samples and 5 channels:

        BaseBuffer<CONT, double> buff(10, 5);

        // Ensure constructor works:

        REQUIRE(buff.channel_capacity() == 10);
        REQUIRE(buff.channels() == 5);

        // Change the values:

        buff.set_channels(9);

        // Ensure these values are in place:

        REQUIRE(buff.channel_capacity() == buff.size() / buff.channels());
        REQUIRE(buff.channels() == 9);
    }

    SECTION("Retrieval", "Ensures data can be retrieved from the buffer") {

        // Construct a multi channel buffer:

        BaseBuffer<CONT, double> buff(idata, 3);

        // First, ensure channel / sample retrieval works:

        for (int chan = 0; chan < buff.channels(); ++chan) {

            // Iterate over samples:

            for (int samp = 0; samp < buff.channel_capacity(); ++samp) {

                // Ensure value is accurate:

                REQUIRE_THAT(
                    buff.at(chan, samp),
                    Catch::Matchers::WithinAbs(data.at(chan).at(samp), 0.0001));
            }
        }

        // Now, ensure interleaved retrieval works:

        for (int i = 0; i < buff.size(); ++i) {

            REQUIRE_THAT(buff.at(i),
                         Catch::Matchers::WithinAbs(idata.at(i), 0.0001));
        }
    }

    SECTION("Iterator Tests", "Tests the iterators included with the buffer") {

        // Create the Buffer:

        BaseBuffer<CONT, double> buff(idata, 3);

        SECTION("Interleaved Iterator Read",
                "Ensures the interleaved iterator can read correctly") {

            // Iterate over said buffer sequentially:

            for (auto iter = buff.ibegin(); iter != buff.iend(); ++iter) {

                // Get the current value:

                const double val = *iter;

                // Ensure this value is correct:

                REQUIRE(val ==
                        data.at(iter.get_channel()).at(iter.get_sample()));

                // Finally, ensure data is valid against known good set:

                REQUIRE(val == idata.at(iter.get_index()));
            }
        }

        SECTION("Sequential Iterator Read",
                "Ensures the sequential iterator can read") {

            // Iterate over said buffer:

            for (auto iter = buff.sbegin(); iter != buff.send(); ++iter) {

                // Get the current value:

                const double val = *iter;

                // Ensure this value is correct:

                REQUIRE(val ==
                        data.at(iter.get_channel()).at(iter.get_sample()));

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

        SECTION("Interleaved Iterator Read Reverse",
                "Ensures the Interleaved iterator can read in reverse") {

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

        SECTION("Interleaved Iterator Constant",
                "Ensures the constant interleaved iterator can read data") {

            // Iterate over it:

            for (auto iter = buff.icbegin(); iter != buff.icend(); ++iter) {

                // Read the value:

                REQUIRE_THAT(
                    *iter,
                    Catch::Matchers::WithinAbs(
                        data.at(iter.get_channel()).at(iter.get_sample()),
                        0.0001));
            }
        }

        SECTION("Sequential Iterator Constant",
                "Ensures the sequential constant iterator can read data") {

            // Iterate over it:

            for (auto iter = buff.scbegin(); iter != buff.scend(); ++iter) {

                // Read the value:

                REQUIRE_THAT(
                    *iter,
                    Catch::Matchers::WithinAbs(
                        data.at(iter.get_channel()).at(iter.get_sample()),
                        0.0001));
            }
        }

        SECTION("Interleaved Iterator Write",
                "Ensures the interleaved iterator can write") {

            // Iterate over interleaved data:

            for (auto iter = buff.ibegin(); iter != buff.iend(); ++iter) {

                // Determine the new value:

                const double val = 10. * iter.get_index() + 1;

                // Change the current value:

                *iter = val;

                // Ensure that the value is 99:

                REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(val, 0.0001));
                REQUIRE_THAT(buff.at(iter.get_index()),
                             Catch::Matchers::WithinAbs(val, 0.0001));
            }

            // Now, use an algorithm method:

            std::fill(buff.ibegin(), buff.iend(), 454);

            // Finally, ensure that all values are 454

            bool const res =
                std::all_of(buff.ibegin(), buff.iend(),
                            [](double num) { return num == 454.0; });

            REQUIRE(res);
        }

        SECTION("Sequential Iterator Write",
                "Ensures the sequential iterator can write") {

            // Iterate over interleaved data:

            for (auto iter = buff.sbegin(); iter != buff.send(); ++iter) {

                // Determine the new value:

                const double val = 10. * iter.get_index() + 1;

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

            bool const res =
                std::all_of(buff.sbegin(), buff.send(),
                            [](double num) { return num == 454.0; });

            REQUIRE(res);
        }

        SECTION("Interleaved Iterator Seek",
                "Ensures the Interleaved Iterator can seek to data") {

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

        SECTION("Sequential Iterator Seek",
                "Ensures the sequential iterator can seek") {

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

TEST_CASE("Dynamic Buffer Test", "[buff]") {

    SECTION("Default Construct",
            "Ensures a buffer can be default constructed") {

        // Create the buffer:

        const Buffer<double> buff;

        // Ensure defaults are correct:

        REQUIRE(buff.channels() == 1);
        REQUIRE(buff.channel_capacity() == 0);
        REQUIRE(buff.size() == 0);
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(SAMPLE_RATE, 0.0001));
    }

    SECTION("Construct", "Ensures a buffer can be constructed") {

        // Create the Buffer:

        const Buffer<double> buff(1, 1);

        // Ensure channel count is valid:

        REQUIRE(buff.channels() == 1);
        REQUIRE(buff.channel_capacity() == 1);
        REQUIRE(buff.size() == 1);
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(SAMPLE_RATE, 0.0001));
    }

    SECTION("Construct Crazy",
            "Ensures a buffer can be constructed with some crazy values") {

        const int csize = 123;
        const int channels = 456;

        // Create the Buffer:

        const Buffer<double> buff(csize, channels);

        // Ensure channel count is valid:

        REQUIRE(buff.channels() == channels);
        REQUIRE(buff.channel_capacity() == csize);
        REQUIRE(buff.size() == csize * channels);
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(SAMPLE_RATE, 0.0001));
    }

    SECTION("Construct Sample",
            "Ensures a buffer can be constructed with a provided sample rate") {

        // Create the buffer:

        const Buffer<double> buff(1, 1, 1.0);

        // Ensure channel count is valid:

        REQUIRE(buff.channels() == 1);
        REQUIRE(buff.channel_capacity() == 1);
        REQUIRE(buff.size() == 1);
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(1., 0.0001));
    }

    SECTION("Construct Sample Crazy",
            "Ensures a buffer can be constructed with a provided sample rate "
            "and crazy values") {

        const int csize = 123;
        const int chans = 456;
        const double samp = 789.1;

        // Create the buffer:

        const Buffer<double> buff(csize, chans, samp);

        // Ensure channel count is valid:

        REQUIRE(buff.channels() == chans);
        REQUIRE(buff.channel_capacity() == csize);
        REQUIRE(buff.size() == csize * chans);
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(samp, 0.0001));
    }

    SECTION("Construct Copy Single Channel",
            "Ensures a buffer can be created via copy with a single channel") {

        // Create a BaseBuffer via copying:

        Buffer<double> buff(chan1);

        // Ensure channel number is valid:

        REQUIRE(buff.channels() == 1);
        REQUIRE(buff.channel_capacity() == chan1.size());
        REQUIRE(buff.size() == chan1.size());
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(SAMPLE_RATE, 0.0001));

        // Ensure single channel is valid:

        REQUIRE(std::equal(chan1.begin(), chan1.end(), buff.ibegin()));
    }

    SECTION("Construct Copy Multi Channel",
            "Ensures a buffer can be created via copy with multi channels") {

        const int channels = 2;

        // Create a BaseBuffer via copying:

        Buffer<double> buff(chan1, channels);

        // Ensure channel number is valid:

        REQUIRE(buff.channels() == channels);
        REQUIRE(buff.channel_capacity() == chan1.size() / channels);
        REQUIRE(buff.size() == chan1.size());
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(SAMPLE_RATE, 0.0001));

        // Ensure single channel is valid:

        REQUIRE(std::equal(chan1.begin(), chan1.end(), buff.ibegin()));
    }

    SECTION("Construct Copy Multi Channel Sample Rate",
            "Ensures a buffer can be created via copy with multi channels and "
            "sample rate") {

        const int channels = 2;
        const double samp = 1234.;

        // Create a BaseBuffer via copying:

        Buffer<double> buff(chan1, channels, samp);

        // Ensure channel number is valid:

        REQUIRE(buff.channels() == channels);
        REQUIRE(buff.channel_capacity() == chan1.size() / channels);
        REQUIRE(buff.size() == chan1.size());
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(samp, 0.0001));

        // Ensure single channel is valid:

        REQUIRE(std::equal(chan1.begin(), chan1.end(), buff.ibegin()));
    }

    SECTION("Construct Iterator Single Channel",
            "Ensures a buffer can be constructed using iterators with a single "
            "channel") {

        // Create a buffer via iterators:

        Buffer<double> buff(idata.begin(), idata.end());

        // Ensure the values are correct:

        REQUIRE(buff.channels() == 1);
        REQUIRE(buff.channel_capacity() == idata.size());
        REQUIRE(buff.size() == idata.size());
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(SAMPLE_RATE, 0.0001));

        REQUIRE(std::equal(idata.begin(), idata.end(), buff.ibegin()));
    }

    SECTION("Construct Iterator Multi Channel",
            "Ensures a buffer can be constructed using iterators with multi "
            "channels") {

        const int channels = 2;

        // Create a buffer via iterators:

        Buffer<double> buff(idata.begin(), idata.end(), channels);

        // Ensure the values are correct:

        REQUIRE(buff.channels() == channels);
        REQUIRE(buff.channel_capacity() == idata.size() / channels);
        REQUIRE(buff.size() == idata.size());
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(SAMPLE_RATE, 0.0001));

        REQUIRE(std::equal(idata.begin(), idata.end(), buff.ibegin()));
    }

    SECTION("Construct Iterator Multi Channel Sample Rate",
            "Ensures a buffer can be constructed using iterators with multi "
            "channels and sample rate") {

        const int channels = 2;
        const int samp = 456.;

        // Create a buffer via iterators:

        Buffer<double> buff(idata.begin(), idata.end(), channels, samp);

        // Ensure the values are correct:

        REQUIRE(buff.channels() == channels);
        REQUIRE(buff.channel_capacity() == idata.size() / channels);
        REQUIRE(buff.size() == idata.size());
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(samp, 0.0001));

        REQUIRE(std::equal(idata.begin(), idata.end(), buff.ibegin()));
    }

    SECTION("Construct Initializer List Single Channel",
            "Ensures a buffer can be created via copy with single channel") {

        std::initializer_list<double> vals = {1, 2, 3, 4, 5};

        // Create a buffer via initializer list:

        Buffer<double> buff = vals;

        // Ensure the values are correct:

        REQUIRE(buff.channels() == 1);
        REQUIRE(buff.channel_capacity() == 5);
        REQUIRE(buff.size() == 5);
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(SAMPLE_RATE, 0.0001));

        REQUIRE(std::equal(vals.begin(), vals.end(), buff.ibegin()));
    }

    SECTION("Construct Initializer List Multi Channel",
            "Ensures a buffer can be created via copy with multiple channels") {

        const int channels = 2;

        std::initializer_list<double> vals = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        // Create a buffer via initializer list:

        Buffer<double> buff(vals, channels);

        // Ensure the values are correct:

        REQUIRE(buff.channels() == channels);
        REQUIRE(buff.channel_capacity() == 10 / 2);
        REQUIRE(buff.size() == 10);
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(SAMPLE_RATE, 0.0001));

        REQUIRE(std::equal(vals.begin(), vals.end(), buff.ibegin()));
    }

    SECTION("Construct Initializer List Multi Channel",
            "Ensures a buffer can be created via copy with multiple channels "
            "and sample rate") {

        const int channels = 2;
        const double samp = 789.;

        std::initializer_list<double> vals = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        // Create a buffer via initializer list:

        Buffer<double> buff(vals, channels, samp);

        // Ensure the values are correct:

        REQUIRE(buff.channels() == channels);
        REQUIRE(buff.channel_capacity() == 10 / 2);
        REQUIRE(buff.size() == 10);
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(samp, 0.0001));

        // Ensure the values are what we expect:

        REQUIRE(std::equal(vals.begin(), vals.end(), buff.ibegin()));
    }

    SECTION("Variadic Template",
            "Ensures a buffer can be created via the variadic template "
            "constructor") {

        // Create the buffer:

        Buffer<double> buff(0., 1., 2., 3., 4., 5., 6., 7., 8., 9.);

        // Ensure the values are correct:

        REQUIRE(buff.channels() == 1);
        REQUIRE(buff.channel_capacity() == 10);
        REQUIRE(buff.size() == 10);
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(SAMPLE_RATE, 0.0001));
    }

    SECTION("Copy Constructor", "Ensure copy constructor works correctly") {

        const int chans = 5;
        const double samp = 1234.;

        // Create an initial buffer:

        Buffer<double> buff(chan1, chans, samp);

        // Create a new vector via copy assignment:

        Buffer<double> buff2(buff);

        // Ensure the vales are the same:

        REQUIRE(buff.channels() == buff2.channels());
        REQUIRE(buff.channel_capacity() == buff2.channel_capacity());
        REQUIRE(buff.size() == buff2.size());
        REQUIRE_THAT(
            buff.get_samplerate(),
            Catch::Matchers::WithinAbs(buff2.get_samplerate(), 0.0001));

        REQUIRE(std::equal(buff.begin(), buff.end(), buff2.begin()));
    }

    SECTION("Move Constructor", "Ensure move constructor works correctly") {

        const int chans = 5;
        const double samp = 1234.;

        // Create an initial buffer:

        Buffer<double> buff(chan1, chans, samp);

        // Create a new vector via copy assignment:

        Buffer<double> buff2(std::move(buff));

        // Ensure the vales are the same:

        REQUIRE(buff2.channels() == chans);
        REQUIRE(buff2.channel_capacity() == chan1.size() / chans);
        REQUIRE(buff2.size() == chan1.size());
        REQUIRE_THAT(buff2.get_samplerate(),
                     Catch::Matchers::WithinAbs(samp, 0.0001));

        REQUIRE(std::equal(buff2.begin(), buff2.end(), chan1.begin()));
    }

    SECTION("Copy Assignment", "Ensure copy assignment works correctly") {

        const int chans = 5;
        const double samp = 1234.;

        // Create an initial buffer:

        Buffer<double> buff(chan1, chans, samp);

        // Create destination buffer:

        Buffer<double> buff2;

        // Preform copy assignment:

        buff2 = buff;

        // Ensure values are the same:

        REQUIRE(buff.channels() == buff2.channels());
        REQUIRE(buff.channel_capacity() == buff2.channel_capacity());
        REQUIRE(buff.size() == buff2.size());
        REQUIRE_THAT(
            buff.get_samplerate(),
            Catch::Matchers::WithinAbs(buff2.get_samplerate(), 0.0001));

        REQUIRE(std::equal(buff.begin(), buff.end(), buff2.begin()));
    }

    SECTION("Move Assignment", "Ensure move assignment works correctly") {

        const int chans = 5;
        const double samp = 1234.;

        // Create an initial buffer:

        Buffer<double> buff(chan1, chans, samp);

        // Create destination buffer:

        Buffer<double> buff2;

        // Preform copy assignment:

        buff2 = std::move(buff);

        // Ensure values are the same:

        REQUIRE(buff2.channels() == chans);
        REQUIRE(buff2.channel_capacity() == chan1.size() / chans);
        REQUIRE(buff2.size() == chan1.size());
        REQUIRE_THAT(buff2.get_samplerate(),
                     Catch::Matchers::WithinAbs(samp, 0.0001));

        REQUIRE(std::equal(buff2.begin(), buff2.end(), chan1.begin()));
    }

    SECTION("Container Copy",
            "Ensures container copy assignment works correctly") {

        // Create a buffer:

        Buffer<double> buff;

        // Copy some data in:

        buff = chan1;

        // Ensure the values are correct:

        REQUIRE(buff.channels() == 1);
        REQUIRE(buff.channel_capacity() == chan1.size());
        REQUIRE(buff.size() == chan1.size());
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(SAMPLE_RATE, 0.0001));

        REQUIRE(std::equal(buff.begin(), buff.end(), chan1.begin()));
    }

    SECTION("Container Move",
            "Ensures container move assignment works correctly") {

        // Create copy of channel data:

        std::vector<double> tdata = chan1;

        // Create a buffer:

        Buffer<double> buff;

        // Copy some data in:

        buff = std::move(tdata);

        // Ensure the values are correct:

        REQUIRE(buff.channels() == 1);
        REQUIRE(buff.channel_capacity() == chan1.size());
        REQUIRE(buff.size() == chan1.size());
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(SAMPLE_RATE, 0.0001));

        REQUIRE(std::equal(buff.begin(), buff.end(), chan1.begin()));
    }

    SECTION("Retrieval", "Ensures data can be retrieved from the buffer") {

        // Construct a multi channel buffer:

        Buffer buff(idata, 3);

        // First, ensure channel / sample retrieval works:

        for (int chan = 0; chan < buff.channels(); ++chan) {

            // Iterate over samples:

            for (int samp = 0; samp < buff.channel_capacity(); ++samp) {

                // Ensure value is accurate:

                REQUIRE_THAT(
                    buff.at(chan, samp),
                    Catch::Matchers::WithinAbs(data.at(chan).at(samp), 0.0001));
            }
        }

        // Now, ensure interleaved retrieval works:

        for (int i = 0; i < buff.size(); ++i) {

            REQUIRE_THAT(buff.at(i),
                         Catch::Matchers::WithinAbs(idata.at(i), 0.0001));
        }
    }

    SECTION("Channel Size", "Ensures the channels can be get/set") {

        // Create a buffer with 10 samples and 5 channels:

        Buffer<double> buff(10, 5);

        // Ensure constructor works:

        REQUIRE(buff.channel_capacity() == 10);
        REQUIRE(buff.channels() == 5);

        // Change the values:

        buff.set_channels(9);

        // Ensure these values are in place:

        REQUIRE(buff.channel_capacity() == ceil(buff.size() / buff.channels()));
        REQUIRE(buff.channels() == 9);
    }

    SECTION("Iterator Tests", "Tests the iterators included with the buffer") {

        // Create the Buffer:

        Buffer<double> buff(idata, 3);

        SECTION("Interleaved Iterator Read",
                "Ensures the interleaved iterator can read correctly") {

            // Iterate over said buffer sequentially:

            for (auto iter = buff.ibegin(); iter != buff.iend(); ++iter) {

                // Get the current value:

                const double val = *iter;

                // Ensure this value is correct:

                REQUIRE(val ==
                        data.at(iter.get_channel()).at(iter.get_sample()));

                // Finally, ensure data is valid against known good set:

                REQUIRE(val == idata.at(iter.get_index()));
            }
        }

        SECTION("Sequential Iterator Read",
                "Ensures the sequential iterator can read") {

            // Iterate over said buffer:

            for (auto iter = buff.sbegin(); iter != buff.send(); ++iter) {

                // Get the current value:

                const double val = *iter;

                // Ensure this value is correct:

                REQUIRE(val ==
                        data.at(iter.get_channel()).at(iter.get_sample()));

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

        SECTION("Interleaved Iterator Read Reverse",
                "Ensures the Interleaved iterator can read in reverse") {

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

        SECTION("Interleaved Iterator Constant",
                "Ensures the constant interleaved iterator can read data") {

            // Iterate over it:

            for (auto iter = buff.icbegin(); iter != buff.icend(); ++iter) {

                // Read the value:

                REQUIRE_THAT(
                    *iter,
                    Catch::Matchers::WithinAbs(
                        data.at(iter.get_channel()).at(iter.get_sample()),
                        0.0001));
            }
        }

        SECTION("Sequential Iterator Constant",
                "Ensures the sequential constant iterator can read data") {

            // Iterate over it:

            for (auto iter = buff.scbegin(); iter != buff.scend(); ++iter) {

                // Read the value:

                REQUIRE_THAT(
                    *iter,
                    Catch::Matchers::WithinAbs(
                        data.at(iter.get_channel()).at(iter.get_sample()),
                        0.0001));
            }
        }

        SECTION("Interleaved Iterator Write",
                "Ensures the interleaved iterator can write") {

            // Iterate over interleaved data:

            for (auto iter = buff.ibegin(); iter != buff.iend(); ++iter) {

                // Determine the new value:

                const double val = 10. * iter.get_index() + 1;

                // Change the current value:

                *iter = val;

                // Ensure that the value is 99:

                REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(val, 0.0001));
                REQUIRE_THAT(buff.at(iter.get_index()),
                             Catch::Matchers::WithinAbs(val, 0.0001));
            }

            // Now, use an algorithm method:

            std::fill(buff.ibegin(), buff.iend(), 454);

            // Finally, ensure that all values are 454

            bool const res =
                std::all_of(buff.ibegin(), buff.iend(),
                            [](double num) { return num == 454.0; });

            REQUIRE(res);
        }

        SECTION("Sequential Iterator Write",
                "Ensures the sequential iterator can write") {

            // Iterate over interleaved data:

            for (auto iter = buff.sbegin(); iter != buff.send(); ++iter) {

                // Determine the new value:

                const double val = 10. * iter.get_index() + 1;

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

            bool const res =
                std::all_of(buff.sbegin(), buff.send(),
                            [](double num) { return num == 454.0; });

            REQUIRE(res);
        }

        SECTION("Interleaved Iterator Seek",
                "Ensures the Interleaved Iterator can seek to data") {

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

        SECTION("Sequential Iterator Seek",
                "Ensures the sequential iterator can seek") {

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

TEST_CASE("Static Buffer Test", "[buff]") {

    SECTION("Default Construct",
            "Ensures a buffer can be default constructed") {

        // Create the Buffer:

        const StaticBuffer<double, 1> buff;

        // Ensure channel count is valid:

        REQUIRE(buff.channels() == 1);
        REQUIRE(buff.channel_capacity() == 1);
        REQUIRE(buff.size() == 1);
        REQUIRE(buff.get_samplerate() == SAMPLE_RATE);
    }

    SECTION("Construct Single Channel",
            "Ensures a buffer can be created with a single channel") {

        // Create an Buffer with a single channel:

        StaticBuffer<double, 10> buff(1);

        // Ensure channel number is valid:

        REQUIRE(buff.channels() == 1);
        REQUIRE(buff.channel_capacity() == 10);
        REQUIRE(buff.size() == 10);
        REQUIRE(buff.get_samplerate() == SAMPLE_RATE);
    }

    SECTION("Construct Multi Channel",
            "Ensures a buffer can be created with a multiple channels") {

        const int chan = 5;

        // Create an Buffer with a single channel:

        StaticBuffer<double, 10> buff(chan);

        // Ensure channel number is valid:

        REQUIRE(buff.channels() == 5);
        REQUIRE(buff.channel_capacity() == 2);
        REQUIRE(buff.size() == 10);
        REQUIRE(buff.get_samplerate() == SAMPLE_RATE);
    }

    SECTION("Construct Sample Rate",
            "Ensures a buffer can be created with a special sample rate") {

        const double samp = 1234;

        // Create an Buffer with a single channel:

        StaticBuffer<double, 10> buff(1, samp);

        // Ensure channel number is valid:

        REQUIRE(buff.channels() == 1);
        REQUIRE(buff.channel_capacity() == 10);
        REQUIRE(buff.size() == 10);
        REQUIRE(buff.get_samplerate() == samp);
    }

    SECTION("Copy Constructor", "Ensure copy constructor works correctly") {

        const int chans = 5;
        const double samp = 1234.;

        // Create an initial buffer:

        StaticBuffer<double, 10> buff(chans, samp);

        // Assign some data in:

        buff.assign(achan1);

        // Create a new vector via copy assignment:

        StaticBuffer<double, 10> buff2(buff);

        // Ensure the vales are the same:

        REQUIRE(buff.channels() == buff2.channels());
        REQUIRE(buff.channel_capacity() == buff2.channel_capacity());
        REQUIRE(buff.size() == buff2.size());
        REQUIRE_THAT(
            buff.get_samplerate(),
            Catch::Matchers::WithinAbs(buff2.get_samplerate(), 0.0001));

        REQUIRE(std::equal(buff.begin(), buff.end(), buff2.begin()));
    }

    SECTION("Move Constructor", "Ensure move constructor works correctly") {

        const int chans = 5;
        const double samp = 1234.;

        // Create an initial buffer:

        StaticBuffer<double, 10> buff(chans, samp);

        // Assign some data in:

        buff.assign(achan1);

        // Create a new vector via copy assignment:

        StaticBuffer<double, 10> buff2(std::move(buff));

        // Ensure the vales are the same:

        REQUIRE(buff2.channels() == chans);
        REQUIRE(buff2.channel_capacity() == chan1.size() / chans);
        REQUIRE(buff2.size() == chan1.size());
        REQUIRE_THAT(buff2.get_samplerate(),
                     Catch::Matchers::WithinAbs(samp, 0.0001));

        REQUIRE(std::equal(buff2.begin(), buff2.end(), chan1.begin()));
    }

    SECTION("Copy Assignment", "Ensure copy assignment works correctly") {

        const int chans = 5;
        const double samp = 1234.;

        // Create an initial buffer:

        StaticBuffer<double, 10> buff(chans, samp);

        // Assign some data in:

        buff.assign(achan1);

        // Create destination buffer:

        StaticBuffer<double, 10> buff2;

        // Preform copy assignment:

        buff2 = buff;

        // Ensure values are the same:

        REQUIRE(buff.channels() == buff2.channels());
        REQUIRE(buff.channel_capacity() == buff2.channel_capacity());
        REQUIRE(buff.size() == buff2.size());
        REQUIRE_THAT(
            buff.get_samplerate(),
            Catch::Matchers::WithinAbs(buff2.get_samplerate(), 0.0001));

        REQUIRE(std::equal(buff.begin(), buff.end(), buff2.begin()));
    }

    SECTION("Move Assignment", "Ensure move assignment works correctly") {

        const int chans = 5;
        const double samp = 1234.;

        // Create an initial buffer:

        StaticBuffer<double, 10> buff(chans, samp);

        // Assign some data in:

        buff.assign(achan1);

        // Create destination buffer:

        StaticBuffer<double, 10> buff2;

        // Preform copy assignment:

        buff2 = std::move(buff);

        // Ensure values are the same:

        REQUIRE(buff2.channels() == chans);
        REQUIRE(buff2.channel_capacity() == chan1.size() / chans);
        REQUIRE(buff2.size() == chan1.size());
        REQUIRE_THAT(buff2.get_samplerate(),
                     Catch::Matchers::WithinAbs(samp, 0.0001));

        REQUIRE(std::equal(buff2.begin(), buff2.end(), chan1.begin()));
    }

    SECTION("Container Copy",
            "Ensures container copy assignment works correctly") {

        // Create a buffer:

        StaticBuffer<double, 10> buff;

        // Copy some data in:

        buff = achan1;

        // Ensure the values are correct:

        REQUIRE(buff.channels() == 1);
        REQUIRE(buff.channel_capacity() == chan1.size());
        REQUIRE(buff.size() == chan1.size());
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(SAMPLE_RATE, 0.0001));

        REQUIRE(std::equal(buff.begin(), buff.end(), chan1.begin()));
    }

    SECTION("Container Move",
            "Ensures container move assignment works correctly") {

        // Create copy of channel data:

        std::array<double, 10> tdata = achan1;

        // Create a buffer:

        StaticBuffer<double, 10> buff;

        // Copy some data in:

        buff = std::move(tdata);

        // Ensure the values are correct:

        REQUIRE(buff.channels() == 1);
        REQUIRE(buff.channel_capacity() == chan1.size());
        REQUIRE(buff.size() == chan1.size());
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(SAMPLE_RATE, 0.0001));

        REQUIRE(std::equal(buff.begin(), buff.end(), chan1.begin()));
    }

    SECTION("Samplerate", "Ensures we can get and set the samplerate") {

        // Create the Buffer:

        StaticBuffer<double, 1> buff;

        // Ensure default sample rate is valid:

        REQUIRE(buff.get_samplerate() == 44100);

        // Set the sample rate:

        buff.set_samplerate(123456);

        // Check the sample rate:

        REQUIRE(buff.get_samplerate() == 123456);
    }

    SECTION("Retrieval", "Ensures data can be retrieved from the buffer") {

        // Construct a multi channel buffer:

        StaticBuffer<double, aidata.size()> buff(3);

        // Move the interleaved ata into the buffer:

        buff.assign(aidata);

        // First, ensure channel / sample retrieval works:

        for (int chan = 0; chan < buff.channels(); ++chan) {

            // Iterate over samples:

            for (int samp = 0; samp < buff.channel_capacity(); ++samp) {

                // Ensure value is accurate:

                REQUIRE_THAT(
                    buff.at(chan, samp),
                    Catch::Matchers::WithinAbs(data.at(chan).at(samp), 0.0001));
            }
        }

        // Now, ensure interleaved retrieval works:

        for (int i = 0; i < buff.size(); ++i) {

            REQUIRE_THAT(buff.at(i),
                         Catch::Matchers::WithinAbs(idata.at(i), 0.0001));
        }
    }

    SECTION("Channel Size", "Ensures the channels can be get/set") {

        // Create a buffer with 10 samples and 5 channels:

        Buffer<double> buff(10, 5);

        // Ensure constructor works:

        REQUIRE(buff.channel_capacity() == 10);
        REQUIRE(buff.channels() == 5);

        // Change the values:

        buff.set_channels(9);

        // Ensure these values are in place:

        REQUIRE(buff.channel_capacity() == ceil(buff.size() / buff.channels()));
        REQUIRE(buff.channels() == 9);
    }

    SECTION("Iterator Tests", "Tests the iterators included with the buffer") {

        // Create the Buffer:

        StaticBuffer<double, aidata.size()> buff(3);

        buff.assign(aidata);

        SECTION("Interleaved Iterator Read",
                "Ensures the interleaved iterator can read correctly") {

            // Iterate over said buffer sequentially:

            for (auto iter = buff.ibegin(); iter != buff.iend(); ++iter) {

                // Get the current value:

                const double val = *iter;

                // Ensure this value is correct:

                REQUIRE(val ==
                        data.at(iter.get_channel()).at(iter.get_sample()));

                // Finally, ensure data is valid against known good set:

                REQUIRE(val == idata.at(iter.get_index()));
            }
        }

        SECTION("Sequential Iterator Read",
                "Ensures the sequential iterator can read") {

            // Iterate over said buffer:

            for (auto iter = buff.sbegin(); iter != buff.send(); ++iter) {

                // Get the current value:

                const double val = *iter;

                // Ensure this value is correct:

                REQUIRE(val ==
                        data.at(iter.get_channel()).at(iter.get_sample()));

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

        SECTION("Interleaved Iterator Read Reverse",
                "Ensures the Interleaved iterator can read in reverse") {

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

        SECTION("Interleaved Iterator Constant",
                "Ensures the constant interleaved iterator can read data") {

            // Iterate over it:

            for (auto iter = buff.icbegin(); iter != buff.icend(); ++iter) {

                // Read the value:

                REQUIRE_THAT(
                    *iter,
                    Catch::Matchers::WithinAbs(
                        data.at(iter.get_channel()).at(iter.get_sample()),
                        0.0001));
            }
        }

        SECTION("Sequential Iterator Constant",
                "Ensures the sequential constant iterator can read data") {

            // Iterate over it:

            for (auto iter = buff.scbegin(); iter != buff.scend(); ++iter) {

                // Read the value:

                REQUIRE_THAT(
                    *iter,
                    Catch::Matchers::WithinAbs(
                        data.at(iter.get_channel()).at(iter.get_sample()),
                        0.0001));
            }
        }

        SECTION("Interleaved Iterator Write",
                "Ensures the interleaved iterator can write") {

            // Iterate over interleaved data:

            for (auto iter = buff.ibegin(); iter != buff.iend(); ++iter) {

                // Determine the new value:

                const double val = 10. * iter.get_index() + 1;

                // Change the current value:

                *iter = val;

                // Ensure that the value is 99:

                REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(val, 0.0001));
                REQUIRE_THAT(buff.at(iter.get_index()),
                             Catch::Matchers::WithinAbs(val, 0.0001));
            }

            // Now, use an algorithm method:

            std::fill(buff.ibegin(), buff.iend(), 454);

            // Finally, ensure that all values are 454

            bool const res =
                std::all_of(buff.ibegin(), buff.iend(),
                            [](double num) { return num == 454.0; });

            REQUIRE(res);
        }

        SECTION("Sequential Iterator Write",
                "Ensures the sequential iterator can write") {

            // Iterate over interleaved data:

            for (auto iter = buff.sbegin(); iter != buff.send(); ++iter) {

                // Determine the new value:

                const double val = 10. * iter.get_index() + 1;

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

            bool const res =
                std::all_of(buff.sbegin(), buff.send(),
                            [](double num) { return num == 454.0; });

            REQUIRE(res);
        }

        SECTION("Interleaved Iterator Seek",
                "Ensures the Interleaved Iterator can seek to data") {

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

        SECTION("Sequential Iterator Seek",
                "Ensures the sequential iterator can seek") {

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

        const RingBuffer<double> ring;

        // Ensure size is accurate:

        REQUIRE(ring.size() == 0);
    }

    SECTION("Construct Size",
            "Ensures we can construct a RingBuffer with a size") {

        // Create a RingBuffer:

        const RingBuffer<double> ring(10);

        // Ensure size is accurate:

        REQUIRE(ring.size() == 10);
    }

    SECTION("Construct Data",
            "Ensures we can construct a RingBuffer with a size") {

        // Create a RingBuffer:

        const RingBuffer<double> ring(chan1);

        // Ensure size is accurate:

        REQUIRE(ring.size() == 10);
    }

    SECTION("Reserve", "Ensures we can reserve a RingBuffer with a size") {

        // Create a RingBuffer:

        RingBuffer<double> ring;

        // Reserve some space:

        ring.reserve(10);

        // Ensure size is accurate:

        REQUIRE(ring.size() == 10);
    }

    SECTION("Retrieval", "Ensures we can retrieve data from the RingBuffer") {

        // Create a RingBuffer:

        RingBuffer<double> ring(chan1);

        // Access some values:

        for (int i = 0; i < ring.size() * 2; ++i) {

            REQUIRE_THAT(ring[i], Catch::Matchers::WithinAbs(
                                      chan1[i % ring.size()], 0.0001));
            REQUIRE_THAT(
                ring[i % ring.size()],
                Catch::Matchers::WithinAbs(chan1[i % ring.size()], 0.0001));
        }
    }

    SECTION("Iterator Tests", "Tests for iterator operations") {

        // Create a RingBuffer:

        RingBuffer<double> ring(chan1);

        SECTION("Iterator Read", "Ensures the iterator can read data") {

            // Create a RingBuffer:

            RingBuffer<double> ring(chan1);

            // Iterate over the values:

            auto ring_iter = ring.begin();

            for (int i = 0; i < ring.size() * 2; ++i) {

                REQUIRE_THAT(ring_iter[i],
                             Catch::Matchers::WithinAbs(ring[i], 0.0001));
                REQUIRE_THAT(
                    ring_iter[i % ring.size()],
                    Catch::Matchers::WithinAbs(ring[i % ring.size()], 0.0001));
            }
        }

        SECTION("Iterator Write", "Ensures the iterator can write") {

            // Iterate over the values:

            auto ring_iter = ring.begin();

            for (int i = 0; i < ring.size() * 2; ++i) {

                // Write a new value:

                ring_iter[i] = 99.;

                // Ensure the value is correct:

                REQUIRE_THAT(ring_iter[i],
                             Catch::Matchers::WithinAbs(99., 0.0001));
            }

            // Now, use an algorithm method:

            std::fill(ring.begin(), ring.end(), 454);

            // Finally, ensure that all values are 454

            bool const res =
                std::all_of(ring.begin(), ring.end(),
                            [](double num) { return num == 454.0; });

            REQUIRE(res);
        }
    }
}
