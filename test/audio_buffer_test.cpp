/**
 * @file audio_buffer.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Various tests for AudioBuffer components
 * @version 0.1
 * @date 2024-05-26
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "audio_buffer.hpp"

#include <algorithm>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cstdint>

TEST_CASE("Create Functions", "[buffer]") {

    SECTION("Create Buffer",
            "Ensures we can create buffers of a given size and channel count") {

        // Create a buffer:

        auto buff = create_buffer(4, 5);

        // Ensure buffer size is correct:

        REQUIRE(buff->channel_capacity() == 4);
        REQUIRE(buff->channels() == 5);
    }

    SECTION("Create Buffer Samplerate",
            "Ensures we can create buffers of a given size, channel, and "
            "samplerate") {

        // Create a buffer:

        auto buff = create_buffer(4, 5, 6);

        // Ensure buffer values are correct:

        REQUIRE(buff->channel_capacity() == 4);
        REQUIRE(buff->channels() == 5);
        REQUIRE(buff->get_samplerate() == 6);
    }
}

TEST_CASE("Conversion", "[byte]") {

    // Values to test:

    double const min = -1.;
    double const hmin = -0.5;
    double const zero = 0;
    double const hmax = 0.5;
    double const max = 1.;

    SECTION("Converts", "Conversion functions from mf to others") {

        SECTION("char", "MF to char") {

            REQUIRE(mf_char(min) == -128);
            REQUIRE(mf_char(hmin) == -64);
            REQUIRE(mf_char(zero) == 0);
            REQUIRE(mf_char(hmax) == 63);
            REQUIRE(mf_char(max) == 127);
        }

        SECTION("uchar", "MF to uchar") {

            REQUIRE(mf_uchar(min) == 0);
            REQUIRE(mf_uchar(hmin) == 64);
            REQUIRE(mf_uchar(zero) == 128);
            REQUIRE(mf_uchar(hmax) ==
                    191);  // Floating point :( needs to be 191 instead of 192
            REQUIRE(mf_uchar(max) == 255);
        }

        SECTION("int16", "MF to int16") {

            REQUIRE(mf_int16(min) == -32768);
            REQUIRE(mf_int16(hmin) == -16384);
            REQUIRE(mf_int16(zero) == 0);
            REQUIRE(mf_int16(hmax) == 16383);
            REQUIRE(mf_int16(max) == 32767);
        }

        SECTION("uint16", "MF to uint16") {

            REQUIRE(mf_uint16(min) == 0);
            REQUIRE(mf_uint16(hmin) == 16384);
            REQUIRE(mf_uint16(zero) == 32768);
            REQUIRE(mf_uint16(hmax) ==
                    49151);  // More floating point :( can't be 49152
            REQUIRE(mf_uint16(max) == 65535);
        }
    }

    SECTION("Reverts", "Reversions from others to mf") {

        SECTION("char", "char to MF") {

            REQUIRE_THAT(char_mf(-128), Catch::Matchers::WithinAbs(min, 0.001));
            REQUIRE_THAT(char_mf(-64), Catch::Matchers::WithinAbs(hmin, 0.001));
            REQUIRE_THAT(char_mf(0), Catch::Matchers::WithinAbs(zero, 0.001));
            REQUIRE_THAT(char_mf(64), Catch::Matchers::WithinAbs(hmax, 0.01));
            REQUIRE_THAT(char_mf(127), Catch::Matchers::WithinAbs(max, 0.001));
        }

        SECTION("uchar", "uchar to MF") {

            REQUIRE_THAT(uchar_mf(0), Catch::Matchers::WithinAbs(min, 0.001));
            REQUIRE_THAT(uchar_mf(64), Catch::Matchers::WithinAbs(hmin, 0.01));
            REQUIRE_THAT(uchar_mf(128), Catch::Matchers::WithinAbs(zero, 0.01));
            REQUIRE_THAT(uchar_mf(192), Catch::Matchers::WithinAbs(hmax, 0.01));
            REQUIRE_THAT(uchar_mf(255), Catch::Matchers::WithinAbs(max, 0.001));
        }

        SECTION("int16", "int16 to MF") {

            REQUIRE_THAT(int16_mf(-32768),
                         Catch::Matchers::WithinAbs(min, 0.001));
            REQUIRE_THAT(int16_mf(-16384),
                         Catch::Matchers::WithinAbs(hmin, 0.001));
            REQUIRE_THAT(int16_mf(0), Catch::Matchers::WithinAbs(zero, 0.001));
            REQUIRE_THAT(int16_mf(16383),
                         Catch::Matchers::WithinAbs(hmax, 0.001));
            REQUIRE_THAT(int16_mf(32767),
                         Catch::Matchers::WithinAbs(max, 0.001));
        }

        SECTION("uint16", "uint16 to MF") {

            REQUIRE_THAT(uint16_mf(0), Catch::Matchers::WithinAbs(min, 0.001));
            REQUIRE_THAT(uint16_mf(16384),
                         Catch::Matchers::WithinAbs(hmin, 0.001));
            REQUIRE_THAT(uint16_mf(32768),
                         Catch::Matchers::WithinAbs(zero, 0.001));
            REQUIRE_THAT(uint16_mf(49152),
                         Catch::Matchers::WithinAbs(hmax, 0.001));
            REQUIRE_THAT(uint16_mf(65535),
                         Catch::Matchers::WithinAbs(max, 0.001));
        }
    }
}

TEST_CASE("Squishers", "[buff][byte]") {

    // Define some buffers:

    std::array<double, 9> idata = {1, 4, 7, 2, 5, 8, 3, 6, 9};
    std::array<double, 9> sidata = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    // Copy to AudioBuffer:

    AudioBuffer buff(3, 3);

    std::copy_n(idata.begin(), 9, buff.ibegin());

    // Define output buffer:

    std::array<double, 9> odata = {};

    SECTION("Inter", "Ensures interleaved squishers work correctly") {

        squish_inter(&buff, odata.begin(), mf_null);

        // Ensure the data is accurate:

        for (int i = 0; i < odata.size(); ++i) {

            // Ensure the value is correct:

            REQUIRE(idata.at(i) == odata.at(i));
        }
    }

    SECTION("Seq", "Ensures sequential squishers work correctly") {

        squish_seq(&buff, odata.begin(), mf_null);

        // Ensure data is accurate:

        for (int i = 0; i < odata.size(); ++i) {

            REQUIRE(sidata.at(i) == odata.at(i));
        }
    }
}

TEST_CASE("Byte Conversion", "[byte]") {

    std::array<unsigned char, 8> char_data = {196, 129, 78, 142,
                                              112, 132, 33, 196};
    std::array<unsigned char, 8> odata = {};

    const int16_t int16 = 33220;

    const int32_t int32 = -1907457596;
    const uint32_t uint32 = 2.3875097e9;

    SECTION("From Char", "Ensures we can create data from chars") {

        SECTION("int16", "Create int16 from char") {

            REQUIRE(char_int16(char_data.data()) == int16);
        }

        SECTION("int32", "Create int32 from char") {

            REQUIRE(char_int32(char_data.data()) == int32);
        }

        SECTION("uint32", "Create uint32 from char") {

            REQUIRE(char_uint32(char_data.data()) == uint32);
        }
    }

    SECTION("To Char", "Ensures we can convert data to chars") {

        SECTION("int16", "Create char from int16") {

            int16_char(int16, odata.begin());

            for (int i = 0; i < 2; ++i) {

                REQUIRE(char_data.at(i) == odata.at(i));
            }
        }

        SECTION("int32", "Create char from int32") {

            int32_char(int32, odata.begin());

            for (int i = 0; i < 4; ++i) {

                REQUIRE(char_data.at(i) == odata.at(i));
            }
        }

        SECTION("uint32", "Create char from uint32") {

            uint32_char(uint32, odata.begin());

            for (int i = 0; i < 4; ++i) {

                REQUIRE(char_data.at(i) == odata.at(i));
            }
        }
    }
}
