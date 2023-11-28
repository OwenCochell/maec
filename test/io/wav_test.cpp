/**
 * @file wav_test.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Tests for wave file components
 * @version 0.1
 * @date 2023-11-24
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <catch2/catch_test_macros.hpp>

#include "io/wav.hpp"
#include "io/mstream.hpp"

TEST_CASE("Base Wave", "[io][wav]") {

    // Create a BaseWave
    BaseWave bwave;

    SECTION("Defaults", "Ensures the default values are correct") {

        REQUIRE(bwave.get_format() == 0);
        REQUIRE(bwave.get_channels() == 1);
        REQUIRE(bwave.get_samplerate() == 44100);
        REQUIRE(bwave.get_byterate() == 0);
        REQUIRE(bwave.get_blockalign() == 0);
        REQUIRE(bwave.get_bits_per_sample() == 0);
        REQUIRE(bwave.get_bytes_per_sample() == 0);
        REQUIRE(bwave.get_size() == 0);
    }
    
    SECTION("Setter", "Ensures basic wave info can be set") {

        // Set some crazy values:

        bwave.set_format(1);
        bwave.set_channels(3);
        bwave.set_samplerate(4);
        bwave.set_byterate(5);
        bwave.set_blockalign(6);
        bwave.set_bits_per_sample(16);
        bwave.set_size(8);

        SECTION("Getter", "Ensures changes can be retrieved") {

            REQUIRE(bwave.get_format() == 1);
            REQUIRE(bwave.get_channels() == 3);
            REQUIRE(bwave.get_samplerate() == 4);
            REQUIRE(bwave.get_byterate() == 5);
            REQUIRE(bwave.get_blockalign() == 6);
            REQUIRE(bwave.get_bits_per_sample() == 16);
            REQUIRE(bwave.get_bytes_per_sample() == 2);
            REQUIRE(bwave.get_size() == 8);
        }
    }
}

TEST_CASE("Wave Reader", "[io][wav]") {

    // Create a wave reader:

    WaveReader wav;

    // Create mstream to read from a source:
    // TODO: 

    FIStream file;
    file.set_path("/home/owencochell/Music/ballz.wav");

    // Set the mstream:

    wav.set_stream(&file);

    SECTION("Get Stream", "Ensures the internal stream is the same we set") {

        REQUIRE(wav.get_stream() == &file);
    }

    // Start the wave reader:

    wav.start();

    SECTION("Start", "Ensures the wave format data is correct") {

        // Ensure the values from the wave file are correct:
        // TODO: Finish This
    }

    SECTION("Read", "Ensures we can read valid audio data") {

        wav.get_data();
    }
}