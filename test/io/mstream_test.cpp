/**
 * @file mstream_test.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Tests for mstreams
 * @version 0.1
 * @date 2023-11-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <catch2/catch_test_macros.hpp>

#include <array>

#include "io/mstream.hpp"

template<bool first, bool second>
class DummyBaseIOStream : public BaseMStream<first, second> {

    /**
     * @brief Just do nothing...
     * 
     * @param pos Position to be ignored
     */
    void seek(int pos) override {}
};

TEST_CASE("Base mstream", "[io][mstream]") {

    // Create a BaseMStream:

    DummyBaseIOStream<false, false> test;

    // Determine default state is correct:

    REQUIRE(test.get_state() == BaseMStream<false, false>::init);

    SECTION("IO Identification", "Ensures the mstream reports the correct IO setting") {

        // Ensure both false is correct:

        REQUIRE(!BaseMStream<false, false>::is_input());
        REQUIRE(!BaseMStream<false, false>::is_output());

        // Ensure input is correct:

        REQUIRE(BaseMStream<true, false>::is_input());
        REQUIRE(!BaseMStream<true, false>::is_output());

        // Ensure output is correct:

        REQUIRE(!BaseMStream<false, true>::is_input());
        REQUIRE(BaseMStream<false, true>::is_output());

        // Ensure both is correct:

        REQUIRE(BaseMStream<true, true>::is_input());
        REQUIRE(BaseMStream<true, true>::is_output());

        // Ensure functions are correct for classes:

        REQUIRE(BaseMIStream::is_input());
        REQUIRE(!BaseMIStream::is_output());

        REQUIRE(!BaseMOStream::is_input());
        REQUIRE(BaseMOStream::is_output());
    }

    SECTION("Start", "Determines if the mstream can be started") {

        // Start the mstream

        test.start();

        // Determine if the mstream has been started

        REQUIRE(test.get_state() == BaseMStream<false, false>::started);
    }

    SECTION("Stop", "Determines if the mstream can be stopped") {

        // Stop the mstream

        test.stop();

        // Determine if the mstream has been stopped

        REQUIRE(test.get_state() == BaseMStream<false, false>::stopped);
    }
}

TEST_CASE("Char mstream", "[io][mstream]") {

    SECTION("Input Stream", "Ensures input streams work correctly") {

        CharIStream stream{0, 1, 2, 3, 4, 5};

        SECTION("Get Array", "Ensures the internal array can be retrieved") {

            // Get the array
            auto arr = stream.get_array();

            // Ensures the array contents are correct:

            for (int i = 0; i < arr.size(); ++i) {

                // Ensure value is what we expect:

                REQUIRE(arr[i] == i);
            }
        }

        // Now, ensure we can pull all values from array:

        std::array<char, 6> out{};

        stream.read(out.data(), 6);

        // Ensure values are what we expect:

        for (int i = 0; i < out.size(); ++i) {

            // Ensure value is what we expect:

            REQUIRE(out.at(i) == i);
        }
    }

    SECTION("Output Stream", "Ensures output streams work correctly") {

        // Define some data to fill with:

        std::array<char, 10> data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

        SECTION("Insertion", "Ensures an empty char mstream can be filled") {

            CharOStream stream;

            // Fill the stream:

            stream.write(data.begin(), data.size());

            // Ensure stream is valid:

            for (int i = 0; i < data.size(); ++i) {

                // Ensure value is accurate:

                REQUIRE(i == stream.get_array().at(i));
            }
        }

        SECTION("Overwrite", "Ensures existing data can be overwritten") {

            CharOStream stream = {0,0,0,0,0,0,0,0,0,0};

            // Fill the data:

            stream.write(data.begin(), data.size());

            // Ensure stream is valid:

            for (int i = 0; i < data.size(); ++i) {

                // Ensure value is accurate:

                REQUIRE(i == stream.get_array().at(i));
            }
        }

        SECTION("Both", "Ensures we can overwrite and add to a stream") {

            CharOStream stream = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

            // Seek to a position:

            stream.seek(4);

            // Fill stream with content:

            stream.write(data.begin(), data.size());

            // Ensure stream is valid:

            for (int i = 0; i < data.size() + 4; ++i) {

                // Determine if we should be reading zeros:

                if (i < 5) {

                    // Ensure we are reading zeros:

                    REQUIRE(0 == stream.get_array().at(i));

                    continue;
                }

                // Otherwise, get the value:

                REQUIRE(i - 4 == stream.get_array().at(i));
            }
        }
    }
}

TEST_CASE("File mstream", "[io][mstream]") {

    // Create a file mstream:

    FIStream istream;
    FOStream ostream;

    // Create temporary file to work with:
    // Screw it, just create a file in the CWD!

    std::string path = "MSTREAM_FILE_TEST.txt";

    istream.set_path(path);
    ostream.set_path(path);

    SECTION("Get path", "Ensures we can retrieve a path to a file") {

        REQUIRE(istream.get_path() == path);
        REQUIRE(ostream.get_path() == path);
    }

    // Start components:

    istream.start();
    ostream.start();

    SECTION("Write to file", "Ensures we can write to a file") {

        // Define some content to be written:

        std::array<char, 5> cont = {1, 2, 3, 4, 5};

        // Write content:

        ostream.write(cont.begin(), cont.size());
        ostream.stop();

        SECTION("Read from file", "Ensures we can read from file") {

            // Define content to be stored:

            std::array<char, cont.size()> ind = {};

            // Read content:

            istream.read(ind.begin(), cont.size()+1);

            // Ensure istream is closed:

            REQUIRE(istream.get_state() == FIStream::mstate::stopped);

            // Ensure content is the same:

            for (int i = 0; i < cont.size(); ++i) {

                // Compare values:

                REQUIRE(ind.at(i) == cont.at(i));
            }
        }
    }

    // Delete file:

    //std::remove(path.c_str());
}
