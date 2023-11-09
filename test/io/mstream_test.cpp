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

#include "io/mstream.hpp"

/**
 * @brief A dummy class for testing mstream components
 * 
 */
// class MStreamTest : public BaseMStream<MStreamTest> {
// private:

//     /// Determines if we have been started
//     bool started = false;

//     /// Determines if we have been stopped
//     bool stopped = false;

// public:

//     void start() { this->started = true; }
//     bool is_start() const { return this->started; }

//     void stop() { this->stopped = true; }
//     bool is_stop() const { return this->stopped; }
// };

TEST_CASE("Dummy test thing", "[dum]") {

    bool inpi = BaseMIStream<>::is_input();
    bool inpo = BaseMIStream<>::is_output(); 

    bool outi = BaseMOStream<>::is_input();
    bool outo = BaseMOStream<>::is_output();

    bool ioi = BaseMIOStream::is_input();
    bool ioo = BaseMIOStream::is_output();
}

// TEST_CASE("Base mstream", "[io][mstream]") {

//     // Create a BaseMStream:

//     MStreamTest temp;

//     BaseMStream<MStreamTest>* test = &temp;

//     SECTION("Start", "Determines if the mstream can be started") {

//         // Start the mstream
//         test->start();

//         // Determine if the mstream has been started

//         REQUIRE(temp.is_start());

//         SECTION("State", "Determines the state changes after start") {

//             REQUIRE(test->get_state() == MStreamTest::mstate::started);
//         }
//     }

//     SECTION("Stop", "Determines if the mstream can be stopped") {

//         // Stop the mstream
//         test->stop();

//         // Determine if the mstream has been stopped

//         REQUIRE(temp.is_stop());

//         SECTION("State", "Determines if the state changes after stop") {

//             REQUIRE(test->get_state() == MStreamTest::mstate::stopped);
//         }
//     }
// }
