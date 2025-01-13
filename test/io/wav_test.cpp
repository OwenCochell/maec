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

#include "io/wav.hpp"

#include <array>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <random>
#include <utility>

#include "audio_buffer.hpp"
#include "io/mstream.hpp"

/**
 * @brief Standard wave file - data is in one huge chunk at the end of the file
 * 
 * No surprises!
 * Just a standard wave file in a trivial format.
 * 
 * This data has the following properties:
 * Channels = 2
 * Sample Rate = 48000
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
 * @brief Wave file with different number of bits per sample
 * 
 * This wave file utilizes unsigned chars, and thus has 8 bits per sample.
 * This is to ensure we can read files with differing formats.
 * 
 * This data has the following properties:
 * Channels = 2
 * Sample Rate = 48000
 * Bits Per Sample = 8
 * Size = 56
 * Number of frames = 10
 * 
 */
CharIStream wavb = {
    0x52, 0x49, 0x46, 0x46, 0x38, 0, 0, 0, 0x57, 0x41, 0x56, 0x45,  // WAVE header
    0x66, 0x6D, 0x74, 0x20, 0x10, 0, 0, 0, 0x01, 0, 0x02, 0, 0x80, 0xbb, 0, 0, 0, 0x77, 0x01, 0x00, 0x02, 0x00, 0x08, 0x00,  // FORMAT chunk
    0x64, 0x61, 0x74, 0x61, 0x14, 0x00, 0x00, 0x00, // DATA header
    0x85, 0xff, 0x9d, 0xff, // Data (till end of array) FAILS AT FIRST VALUE IN LINE!
    0x42, 0xff, 0x9b, 0xff,
    0x72, 0xff, 0x7d, 0xff,
    0xe0, 0xff, 0x07, 0xff,
    0x25, 0x00, 0xea, 0xfe,
};

/**
 * @brief Wave file with bad size reporting
 * 
 * This file reports that the data chunk is VERY LARGE!
 * This is to ensure we can gracefully quit when we reach the end of the file.
 * 
 * This data has the following properties:
 * Channels = 2
 * Sample Rate = 48000
 * Bits Per Sample = 16
 * Size = 283
 * 
 */
CharIStream wavbs = {
    0x52, 0x49, 0x46, 0x46, 0x38, 0, 0, 0, 0x57, 0x41, 0x56, 0x45,  // WAVE header
    0x66, 0x6D, 0x74, 0x20, 0x10, 0, 0, 0, 0x01, 0, 0x02, 0, 0x80, 0xbb, 0, 0, 0, 0xee, 0x02, 0x00, 0x04, 0x00, 0x10, 0x00,  // FORMAT chunk
    0x64, 0x61, 0x74, 0x61, 0xff, 0xff, 0x00, 0x00, // DATA header
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
 * Sample Rate = 48000
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

/**
 * @brief Interrupting junk file - junk chunks interrupt data chunks!
 * 
 * This file has multiple data chunks
 * that are interrupted by junk chunks.
 * This is to ensure we can properly ignore junk data that lies between data chunks.
 * 
 * This data can also be used to test if we can properly read multiple chunks if necessary.
 * 
 * The data has the following properties:
 * Channels = 2
 * Sample Rate = 48000
 * Bits Per Sample = 16
 * Size = 120
 * Number of Frames = 10
 * 
 */
CharIStream jiwavs = {
    0x52, 0x49, 0x46, 0x46, 0x70, 0, 0, 0, 0x57, 0x41, 0x56, 0x45,  // WAVE header
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
    0x64, 0x61, 0x74, 0x61, 0x14, 0x00, 0x00, 0x00, // 2nd DATA header
    0x35, 0xff, 0x5c, 0xff, // Data 2nd data chunk
    0x8a, 0xff, 0x2d, 0xff,
    0x77, 0xff, 0x7a, 0xff,
    0xe3, 0xff, 0xf7, 0xff,
    0x25, 0x00, 0xeb, 0xfe,
};

std::array<int16_t, 10> data_wavs = {-123, -99, -190, -101, -142, -131, -32, -249, 37, -278};

std::array<unsigned char, 20> data_wavb = {0x85, 0xff, 0x9d, 0xff, 0x42, 0xff, 0x9b,
                                 0xff, 0x72, 0xff, 0x7d, 0xff, 0xe0, 0xff,
                                 0x07, 0xff, 0x25, 0x00, 0xea, 0xfe};

std::array<int16_t, 20> data_wavji = {-123, -99, -190, -101, -142, -131, -32, -249, 37, -278,
                                      -203, -164, -118, -211, -137, -134, -29, -9, 37, -277};

TEST_CASE("Base Wave", "[io][wav]") {

    // Create a BaseWave
    BaseWave bwave;

    SECTION("Defaults", "Ensures the default values are correct") {

        REQUIRE(bwave.get_format() == 1);
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
        bwave.set_bits_per_sample(16);
        bwave.set_size(8);
        bwave.set_byterate(5);
        bwave.set_blockalign(6);

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

    SECTION("Byterate Setter", "Ensures we can auto-set our byterate") {

        // Set some initial values:

        bwave.set_channels(1);
        bwave.set_samplerate(1);
        bwave.set_bytes_per_sample(1);

        // Ensure value is correct:

        REQUIRE(bwave.get_byterate() == 1);

        // Change Channel:

        bwave.set_channels(2);

        REQUIRE(bwave.get_byterate() == 2);

        // Change samplerate:

        bwave.set_samplerate(48000);

        REQUIRE(bwave.get_byterate() == 2 * 48000);

        // Change bytes per sample:

        bwave.set_bytes_per_sample(5);

        REQUIRE(bwave.get_byterate() == 5 * 48000 * 2);

        // Just for fun, also adjust the bits per sample:

        bwave.set_bits_per_sample(16);

        REQUIRE(bwave.get_byterate() == 2 * 48000 * 2);
    }

    SECTION("Blockalign Setter", "Ensures we can auto-set our byterate") {

        // Set some initial values:

        bwave.set_channels(1);
        bwave.set_bytes_per_sample(1);

        REQUIRE(bwave.get_blockalign() == 1);

        // Set the chanels:

        bwave.set_channels(2);

        REQUIRE(bwave.get_blockalign() == 2);

        // Set the bytes per sample:

        bwave.set_bytes_per_sample(3);

        REQUIRE(bwave.get_blockalign() == 2 * 3);

        // Just for fun, change bits per sample:

        bwave.set_bits_per_sample(16);

        REQUIRE(bwave.get_blockalign() == 2 * 2);
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

                REQUIRE(int16_mf(data_wavs.at(static_cast<size_t>(i)*2)) == data->at(0));
                REQUIRE(int16_mf(data_wavs.at(i*2+1)) == data->at(1));
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

    SECTION("Bad Wave Size", "Ensures wave files with bad size reporting can be read") {

        // Create copy of standard wave:

        auto swave = wavbs;

        // Attach mstream to wave file"

        wav.set_stream(&swave);

        // Start the wave file:

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
            REQUIRE(wav.get_size() == 64);
        }

        SECTION("Data Extraction", "Ensures we can extract data") {

            // Set buffer size to something small
            wav.set_buffer_size(1);

            // Read until we are complete:

            int index = 0;

            while (!wav.done()) {

                // Handle this frame:

                auto data = wav.get_data();

                // Ensure size is what we expect:

                REQUIRE(data->channels() == 2);
                REQUIRE(data->channel_capacity() == 1);

                // Ensure contents are as we expect:

                REQUIRE(int16_mf(data_wavs.at(static_cast<size_t>(index) * 2)) == data->at(0));
                REQUIRE(int16_mf(data_wavs.at(index * 2 + 1)) == data->at(1));

                // Increment index:

                index++;
            }

            // Ensure we have read the number of frames, not more not less!

            REQUIRE(index == 5);
        }
    }

    SECTION("8bit Wave Data", "Ensures wave files with 8 bits per sample can be read") {

        // Create copy of standard wave:

        auto swave = wavb;

        // Attach mstream to wave file"

        wav.set_stream(&swave);

        // Start the wave file:

        wav.start();

        SECTION("Format Decoding", "Ensures the format data has been properly decoded") {

            REQUIRE(wav.get_bits_per_sample() == 8);
            REQUIRE(wav.get_blockalign() == 2);
            REQUIRE(wav.get_byterate() == 96000);
            REQUIRE(wav.get_bytes_per_sample() == 1);
            REQUIRE(wav.get_channels() == 2);
            REQUIRE(wav.get_format() == 1);
            REQUIRE(wav.get_samplerate() == 48000);
            REQUIRE(wav.get_size() == 56+8);
        }

        SECTION("Data Extraction", "Ensures we can extract audio data") {

            // Set the buffer size to something small (just one frame):

            wav.set_buffer_size(1);

            // The wave data has 10 frames, so read 10 times:

            for (int i = 0; i < 10; ++i) {

                // Grab data:
                auto data = wav.get_data();

                // Ensure size is what we expect:

                REQUIRE(data->channels() == 2);
                REQUIRE(data->channel_capacity() == 1);

                // Ensure contents are as we expect:

                REQUIRE(uchar_mf(data_wavb.at(static_cast<size_t>(i)*2)) == data->at(0));
                REQUIRE(uchar_mf(data_wavb.at(i*2+1)) == data->at(1));
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

                REQUIRE(int16_mf(data_wavs.at(static_cast<size_t>(i) * 2)) == data->at(0));
                REQUIRE(int16_mf(data_wavs.at(i * 2 + 1)) == data->at(1));
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

    SECTION("Interrupting Junk", "Ensures we can ignore and correctly chunk multi-data files") {

        // Create copy of junk wave:

        auto sjwav = jiwavs;

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
            REQUIRE(wav.get_size() == 120);
        }

        SECTION("Data Extraction", "Ensures we can extract data") {

            // Set the buffer size to something small (just one frame):

            wav.set_buffer_size(1);

            // The wave data has 10 frames:

            for (int i = 0; i < 10; ++i) {

                // Grab data:
                auto data = wav.get_data();

                // Ensure size is what we expect:

                REQUIRE(data->channels() == 2);
                REQUIRE(data->channel_capacity() == 1);

                // Ensure contents are as we expect:

                REQUIRE(int16_mf(data_wavji.at(static_cast<size_t>(i) * 2)) == data->at(0));
                REQUIRE(int16_mf(data_wavji.at(i * 2 + 1)) == data->at(1));
            }

            // We should be done with wave file:

            REQUIRE(wav.done());

            // Read once more, just for fun

            auto data = wav.get_data();

            // Should just be zeros:

            REQUIRE(data->at(0) == 0);
            REQUIRE(data->at(1) == 0);
        }

        SECTION("Multi-Data Extraction", "Ensures we can read interrupted data in large chunks") {

            // Set buffer size to something WEIRD!

            wav.set_buffer_size(4);

            // We need to read this wave twice to consume all data:

            for (int i = 0; i < 3; ++i) {

                // Grab data:
                auto data = wav.get_data();

                // Ensure size is what we expect:

                REQUIRE(data->channels() == 2);
                REQUIRE(data->channel_capacity() == 4);

                // Iterate over all values in wave file

                for (int j = 0; j < data->size(); ++j) {

                    // Determine if we don't have data for this:

                    if (i*8 + j >= data_wavji.size()) {

                        // Should just be zero...
                        REQUIRE(0 == data->at(j));

                        continue;
                    }

                    // Ensure data is accurate:

                    REQUIRE(int16_mf(data_wavji.at(i * 8 + j)) == data->at(j));
                }
            }

            // As of now, we should be done:

            REQUIRE(wav.done());
        }
    }
}

TEST_CASE("Wave Writer", "[io][wav]") {

    // Create a WaveWriter:

    WaveWriter wav;

    SECTION("Stream Set", "Ensures we can set the mstream") {

        // Create a dummy mstream:

        CharOStream stream;

        // Set it:

        wav.set_stream(&stream);

        SECTION("Stream Get", "Ensures we can get the mstream") {

            REQUIRE(wav.get_stream() == &stream);
        }
    }

    SECTION("16bit Write", "Ensures we can create wave files of 16bit integers") {

        // We use the wave file examples for this test!
        // Create a stream we can write to:

        CharOStream stream;

        // Add stream to writer:

        wav.set_stream(&stream);

        // Configure the parameters:

        wav.set_bits_per_sample(16);
        wav.set_samplerate(48000);
        wav.set_channels(2);

        // Start the writer:

        wav.start();

        // Create a buffer with audio data to write:

        auto thing = data_wavs.size();

        BufferPointer buff = std::make_unique<AudioBuffer>(data_wavs.size());

        // Convert values:

        for (int i = 0; i < data_wavs.size(); ++i) {

            // Convert into mf:

            buff->at(i) = int16_mf(data_wavs.at(i));
        }

        // Write to writer:

        wav.write_data(std::move(buff));

        // Stop the writer:

        wav.stop();

        // Ensure contents are equal:

        for (int i = 0; i < wavs.get_array().size(); ++i) {

            REQUIRE(wavs.get_array().at(i) == stream.get_array().at(i));
        }
    }

    SECTION("8bit Wave", "Ensures we can create wave files of 8bit integers") {

        // We use the wave file examples for this test!
        // Create a stream we can write to:

        CharOStream stream;

        // Add stream to writer:

        wav.set_stream(&stream);

        // Configure the parameters:

        wav.set_bits_per_sample(8);
        wav.set_samplerate(48000);
        wav.set_channels(2);

        // Start the writer:

        wav.start();

        // Create a buffer with audio data to write:

        BufferPointer buff = std::make_unique<AudioBuffer>(data_wavb.size());

        // Convert values:

        for (int i = 0; i < data_wavb.size(); ++i) {

            // Convert into mf:

            buff->at(i) = uchar_mf(data_wavb.at(i));
        }

        // Write to writer:

        wav.write_data(std::move(buff));

        // Stop the writer:

        wav.stop();

        // Ensure contents are equal:

        // Fails at i=44

        auto blah = wavb.get_array().at(44);

        for (int i = 0; i < wavb.get_array().size(); ++i) {

            REQUIRE(wavb.get_array().at(i) == stream.get_array().at(i));
        }
    }

    SECTION("8bit Wave Multi", "Ensures we can write multiple buffers correctly") {

        // We use the wave file examples for this test!
        // Create a stream we can write to:

        CharOStream stream;

        // Add stream to writer:

        wav.set_stream(&stream);

        // Configure the parameters:

        wav.set_bits_per_sample(8);
        wav.set_samplerate(48000);
        wav.set_channels(2);

        // Start the writer:

        wav.start();

        // Create a buffer with audio data to write:

        BufferPointer buff1 = std::make_unique<AudioBuffer>(data_wavb.size() / 2);
        BufferPointer buff2 = std::make_unique<AudioBuffer>(data_wavb.size() / 2);

        // Convert values:

        for (int i = 0; i < data_wavb.size() / 2; ++i) {

            // Convert into mf:

            buff1->at(i) = uchar_mf(data_wavb.at(i));
            buff2->at(i) = uchar_mf(data_wavb.at(i + data_wavb.size() / 2));
        }

        // Write to writer:

        wav.write_data(std::move(buff1));
        wav.write_data(std::move(buff2));

        // Stop the writer:

        wav.stop();

        // Ensure contents are equal:

        for (int i = 0; i < wavb.get_array().size(); ++i) {

            REQUIRE(wavb.get_array().at(i) == stream.get_array().at(i));
        }
    }
}

TEST_CASE("Random Wave", "[io][wav]") {

    // We create a random wave file and read it using our components

    // Create an output stream:

    CharOStream ostream;

    // Create writer:

    WaveWriter wwav;

    // Bind stream to writer:

    wwav.set_stream(&ostream);

    // Create a random number generator:

    std::default_random_engine rand;

    // Seed random number generator:

    std::mt19937 gen(rand());

    // We are working with 8bit integers for space reasons:

    wwav.set_bits_per_sample(16);

    // Determine a sample rate:

    std::uniform_int_distribution sdist(44100, 48000);
    const int sample_rate = sdist(gen);

    // Determine channel count:

    std::uniform_int_distribution cdist(1, 8);
    const int channels = cdist(gen);

    // Configure the writer:

    wwav.set_samplerate(sample_rate);
    wwav.set_channels(channels);

    // Determine a number of samples to be generated:
    // (Must be a multiple of channel count)

    std::uniform_int_distribution ndist(1, 100);
    const int num_frames = ndist(gen) * channels;

    // Generate the number of frames:

    BufferPointer inb = std::make_unique<AudioBuffer>(num_frames / channels, channels);
    AudioBuffer backup(num_frames / channels, channels);

    std::uniform_int_distribution gdist(0, 255);

    for (auto iter = inb->ibegin(); iter != inb->iend(); ++iter) {

        // Create the frame:

        const long double frame = uchar_mf(gdist(gen));

        *iter = frame;
        backup.at(iter.get_index()) = frame;
    }

    // Start the wave writer:

    wwav.start();

    // Write buffer to writer:

    wwav.write_data(std::move(inb));

    // Stop the writer:

    wwav.stop();

    // Create input stream:

    CharIStream istream;

    // Set output array as input array:

    istream.get_array() = ostream.get_array();

    // Create wave reader:

    WaveReader rwav;

    // Bind stream to wave reader:

    rwav.set_stream(&istream);

    // Start the reader:

    rwav.start();

    // Set the buffer size to hold entire collection of frames:

    rwav.set_buffer_size(num_frames / channels);

    // Get data:

    auto outb = rwav.get_data();

    // Now ensure the parameters are the same:

    REQUIRE(wwav.get_bits_per_sample() == rwav.get_bits_per_sample());
    REQUIRE(wwav.get_blockalign() == rwav.get_blockalign());
    REQUIRE(wwav.get_byterate() == rwav.get_byterate());
    REQUIRE(wwav.get_bytes_per_sample() == rwav.get_bytes_per_sample());
    REQUIRE(wwav.get_channels() == rwav.get_channels());
    REQUIRE(wwav.get_format() == rwav.get_format());
    REQUIRE(wwav.get_samplerate() == rwav.get_samplerate());
    REQUIRE(wwav.get_size() == rwav.get_size());

    // Ensure frames are the same:

    REQUIRE(outb->channels() == backup.channels());
    REQUIRE(outb->channel_capacity() == backup.channel_capacity());
    REQUIRE(outb->size() == backup.size());

    for (int i = 0; i < outb->size(); ++i) {

        // Ensure samples are the same:

        REQUIRE_THAT(outb->at(i), Catch::Matchers::WithinAbs(backup.at(i), 0.0001));
    }
}
