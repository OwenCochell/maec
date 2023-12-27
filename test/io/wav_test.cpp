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

// Define some wave data to be read:
// Standard wave file
// Wave file that is shorter than reported
// Wave file with non-aligned data chunks with interruptions
// Wave file with junk data at the end
// Wave file with junk data at the start
// Wave file with different bps 

/**
 * @brief Standard wave file - data is in one huge chunk at the end of the file
 * 
 * No surprises!
 * Just a standard wave file in a trivial format.
 * 
 * This data has the following properties:
 * Channels = 2
 * Sample Rate = 44100
 * Bits Per Sample = 16
 * Size = 56
 * Number of frames = 5
 * 
 */
CharIStream wavs = {
    0x52, 0x49, 0x46, 0x46, 0x38, 0, 0, 0, 0x57, 0x41, 0x56, 0x45,  // WAVE header
    0x66, 0x6D, 0x74, 0x20, 0x10, 0, 0, 0, 0x01, 0, 0x02, 0, 0x80, 0xbb, 0, 0, 0, 0xee, 0x02, 0x00, 0x04, 0x00, 0x10, 0x00,  // FORMAT chunk
    0x64, 0x61, 0x74, 0x61, 0x14, 0x00, 0x00, 0x00, // DATA header
    0x85, 0xff, 0x9d, 0xff, // Data (till end of array)
    0x42, 0xff, 0x9b, 0xff,
    0x72, 0xff, 0x7d, 0xff,
    0xe0, 0xff, 0x07, 0xff,
    0x25, 0x00, 0xea, 0xfe,
};

/**
 * @brief Junk wave file - junk chunks present at end of file
 * 
 * This file has some chunks at the end that are not data chunks,
 * so we are not interested in them!
 * Ideally, we should encounter these chunks and then do nothing.
 * This wave reader should return a buffer of zeros and configure itself accordingly.
 * 
 * The data has the following properties:
 * Channels = 2
 * Sample Rate = 44100
 * Bits Per Sample = 16
 * Size = 84
 * Number of frames = 5
 * 
 */
CharIStream jwavs = {
    0x52, 0x49, 0x46, 0x46, 0x54, 0, 0, 0, 0x57, 0x41, 0x56, 0x45,  // WAVE header
    0x66, 0x6D, 0x74, 0x20, 0x10, 0, 0, 0, 0x01, 0, 0x02, 0, 0x80, 0xbb, 0, 0, 0, 0xee, 0x02, 0x00, 0x04, 0x00, 0x10, 0x00,  // FORMAT chunk
    0x64, 0x61, 0x74, 0x61, 0x14, 0x00, 0x00, 0x00, // DATA header
    0x85, 0xff, 0x9d, 0xff, // Data
    0x42, 0xff, 0x9b, 0xff,
    0x72, 0xff, 0x7d, 0xff,
    0xe0, 0xff, 0x07, 0xff,
    0x25, 0x00, 0xea, 0xfe,
    'J', 'U', 'N', 'K', 0x14, 0x00, 0x00, 0x00, // JUNK header
    0x85, 0xff, 0x9d, 0xff, // Junk Data (till end of array)
    0x42, 0xff, 0x9b, 0xff,
    0x72, 0xff, 0x7d, 0xff,
    0xe0, 0xff, 0x07, 0xff,
    0x25, 0x00, 0xea, 0xfe,
};


std::array<int16_t, 10> data_wavs = {-123, -99, -190, -101, -142, -131, -32, -249, 37, -278};

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

    SECTION("Stream Set", "Ensures we can set the mstream") {

        // Create a dummy mstream:

        CharIStream dumm;

        // Set it:

        wav.set_stream(&dumm);

        SECTION("Stream Get", "Ensures we can get the mstream") {

            REQUIRE(wav.get_stream() == &dumm);
        }
    }

    SECTION("Buffer Set", "Ensures we can set the buffer size") {

        // Set the buffer size to something crazy:

        wav.set_buffer_size(137);

        SECTION("Buffer Get", "Ensures we can get the buffer size") {

            REQUIRE(wav.get_buffer_size() == 137);
        }
    }

    SECTION("Standard Wave", "Ensures wave files with data in one chunk at the end can be read") {

        // Create copy of standard wave:

        auto swave = wavs;

        // Attach mstream to wave file"

        wav.set_stream(&swave);

        // Start the wave file:

        wav.start();

        SECTION("Format Decoding", "Ensures the format data has been properly decoded") {

            REQUIRE(wav.get_bits_per_sample() == 16);
            REQUIRE(wav.get_blockalign() == 4);
            REQUIRE(wav.get_byterate() == 192000);
            REQUIRE(wav.get_bytes_per_sample() == 2);
            REQUIRE(wav.get_channels() == 2);
            REQUIRE(wav.get_format() == 1);
            REQUIRE(wav.get_samplerate() == 48000);
            REQUIRE(wav.get_size() == 56+8);
        }

        SECTION("Data Extraction", "Ensures we can extract audio data") {

            // Set the buffer size to something small (just one frame):

            wav.set_buffer_size(1);

            // The wave data has 5 frames, so read 10 times:

            for (int i = 0; i < 5; ++i) {

                // Grab data:
                auto data = wav.get_data();

                // Ensure size is what we expect:

                REQUIRE(data->channels() == 2);
                REQUIRE(data->channel_capacity() == 1);

                // Ensure contents are as we expect:

                REQUIRE(int16_mf(data_wavs[i*2]) == data->at(0));
                REQUIRE(int16_mf(data_wavs[i*2+1]) == data->at(1));
            }

            // We should be done with the file:

            REQUIRE(wav.done());

            // Just for fun, try to read once more:

            auto data = wav.get_data();

            // Should just be zeros:

            REQUIRE(data->at(0) == 0);
            REQUIRE(data->at(1) == 0);
        }
    }

    SECTION("Junk Wave", "Ensures we ignore junk data at end of file") {

        // Create copy of junk wave:

        auto sjwav = jwavs;

        // Attach stream to wave reader:

        wav.set_stream(&sjwav);

        // Start wave file:
        wav.start();

        SECTION("Format Decoding",
                "Ensures the format data has been properly decoded") {

            REQUIRE(wav.get_bits_per_sample() == 16);
            REQUIRE(wav.get_blockalign() == 4);
            REQUIRE(wav.get_byterate() == 192000);
            REQUIRE(wav.get_bytes_per_sample() == 2);
            REQUIRE(wav.get_channels() == 2);
            REQUIRE(wav.get_format() == 1);
            REQUIRE(wav.get_samplerate() == 48000);
            REQUIRE(wav.get_size() == 84+8);
        }

        SECTION("Data Extraction", "Ensures we can extract audio data") {

            // Set the buffer size to something small (just one frame):

            wav.set_buffer_size(1);

            // The wave data has 5 frames, so read 10 times:

            for (int i = 0; i < 5; ++i) {

                // Grab data:
                auto data = wav.get_data();

                // Ensure size is what we expect:

                REQUIRE(data->channels() == 2);
                REQUIRE(data->channel_capacity() == 1);

                // Ensure contents are as we expect:

                REQUIRE(int16_mf(data_wavs[i * 2]) == data->at(0));
                REQUIRE(int16_mf(data_wavs[i * 2 + 1]) == data->at(1));
            }

            // We are not necessarily done with the wave file,
            // read once more to get rid of junk content:

            auto data = wav.get_data();

            // Content should be zeros:

            REQUIRE(data->at(0) == 0);
            REQUIRE(data->at(1) == 0);

            // We should be done with the file:

            REQUIRE(wav.done());
        }
    }
}
