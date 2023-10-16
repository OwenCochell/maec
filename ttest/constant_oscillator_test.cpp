/**
 * @file oscillator_test.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Basic tests for oscillators
 * @version 0.1
 * @date 2022-09-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <catch2/catch_test_macros.hpp>

#include "base_oscillator.hpp"

TEST_CASE("ConstantOscillator Test", "[osc]") {

    // Create the oscillator:

    ConstantOscillator osc;

    SECTION("Construct") {

        // Ensure the initial value is 0.0:

        REQUIRE(osc.get_value() == 0.0);

        // Construct a new oscillator with a value of 1.0:

        ConstantOscillator osc2(1.0);

        // Ensure the initial value is 1.0:

        REQUIRE(osc2.get_value() == 1.0);
    }

    // Set the value:

    osc.set_value(1.0);

    SECTION("Getter") {

        // Ensure value has been properly set:

        REQUIRE(osc.get_value() == 1.0);
    }

    // Process the oscillator:

    osc.process();

    // Get the buffer:

    auto buff = osc.get_buffer();

    // Ensure the buffer is not null:

    REQUIRE(buff != nullptr);

    // Ensure the buffer is the correct size:

    REQUIRE(buff->size() == 440);

    // Ensure the buffer is filled with the correct value:

    for (auto& val : *buff) {

        REQUIRE(val == 1.0);

    }

}
