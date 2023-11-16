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
#include <cstdio>

#include "io/mstream.hpp"

TEST_CASE("Base mstream", "[io][mstream]") {

    // Create a BaseMStream:

    BaseMStream<false, false> test;

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

        REQUIRE(!BaseMOStream::is_output());
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

    bool igood = istream.good();
    bool ogood = ostream.good();

    SECTION("Write to file", "Ensures we can write to a file") {
        
        // Define some content to be written:

        std::array<char, 5> cont = {1, 2, 3, 4, 5};

        // Write content:

        ostream.write(cont.data(), 5);

        ogood = ostream.good();

        SECTION("Read from file", "Ensures we can read from file") {

            // Define content to be stored:

            std::array<char, 5> ind = {0, 0, 0, 0, 0};

            // Read content:

            istream.read(ind.data(), 5);

            // Ensure content is the same:

            for (int i = 0; i < ind.size(); ++i) {

                // Compare values:

                REQUIRE(ind.at(i) == cont.at(i));
            }
        }

    }

    // Finally, close file:

    istream.stop();
    ostream.stop();

    // Delete file:

    std::remove(path.c_str());
}
