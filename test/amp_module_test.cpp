/**
 * @file amp_module_test.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Tests for amplitude modules
 * @version 0.1
 * @date 2023-02-02
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "amp_module.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "meta_audio.hpp"

TEST_CASE("BaseAmplitude Test", "[amp]") {

    // Create BaseAmplitude

    BaseAmplitude amp;

    SECTION("Default", "Ensures the default value is correct") {

        REQUIRE(amp.get_value() == 1.0);
    }

    SECTION("Constructor", "Ensures the constructor value is correct") {

        // Create BaseAmplitude

        BaseAmplitude amp2(0.5);

        // Ensure the value is correct:

        REQUIRE(amp2.get_value() == 0.5);
    }

    SECTION("Setter", "Ensures we can correctly set the amplitude") {

        // Set the value:

        amp.set_value(0.5);

        // Ensure the value is correct:

        REQUIRE(amp.get_value() == 0.5);
    }
}

TEST_CASE("AmplitudeScale Test", "[amp]") {

    // Create an AmplitudeScale:

    AmplitudeScale<> amp;

    SECTION("Process", "Ensures the amplitude modules processes correctly") {

        // Create constant oscillator:

        ConstModule osc(0.5);

        // Bind the module together:

        amp.link(&osc);

        // Process the data:

        amp.meta_process();

        // Get the buffer:

        auto buff = amp.get_buffer();

        // Ensure data is accurate:

        for (auto iter = buff.ibegin(); iter != buff.send(); ++iter) {

            // Ensure value is correct:

            REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(0.5, 0.0001));
        }
    }
}

TEST_CASE("AmplitudeAdd Test", "[amp]") {

    // Create with a value of 0.5:

    AmplitudeAdd<> amp(0.5);

    SECTION("Process", "Ensures the amplitude module processes correctly") {

        // Create constant oscillator:

        ConstModule osc(1);

        // Bind the module together:

        amp.link(&osc);

        // Process the data:

        amp.meta_process();

        // Get the buffer:

        auto buff = amp.get_buffer();

        // Ensure data is accurate:

        for (auto iter = buff.ibegin(); iter != buff.send(); ++iter) {

            // Ensure value is correct:

            REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(1.5, 0.0001));
        }
    }
}
