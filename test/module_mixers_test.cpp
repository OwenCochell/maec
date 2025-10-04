/**
 * @file module_mixers.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Basic tests for module mixers
 * @version 0.1
 * @date 2022-09-18
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "meta_audio.hpp"
#include "module_mixer.hpp"

TEST_CASE("ModuleMixUp Tests", "[mixer]") {

    // Construct mixer to utilize:

    ModuleMixUp mix;

    SECTION("Bind", "Ensures modules can be bound to the mixer") {

        // Create the modules:

        AudioModule mod;
        AudioModule mod1;

        // Bind the mixer to each module:

        mod.link(&mix);
        mod1.link(&mix);

        // Ensure the number of modules attached to valid:

        REQUIRE(mix.num_outputs() == 2);
    }

    SECTION("Process", "Ensures mixer processing works") {

        // Create the modules:

        ConstModule osc1(0.50);

        // Attach the modules:

        mix.link(&osc1);

        // Process the values:

        mix.meta_process();

        // Get the buffers:

        std::unique_ptr<AudioBuffer> buff = mix.get_buffer();
        std::unique_ptr<AudioBuffer> buff1 = mix.get_buffer();

        // Ensure they are not the same memory location:

        REQUIRE(buff != buff1);

        // Iterate over the first buffer:

        for (auto item : *buff) {

            // Ensure the value is around 0.5:

            REQUIRE_THAT(item, Catch::Matchers::WithinAbs(0.50, 0.0001));
        }

        // Iterate over the second buffer:

        for (auto item : *buff1) {

            // Ensure the value is around 0.5:

            REQUIRE_THAT(item, Catch::Matchers::WithinAbs(0.50, 0.0001));
        }
    }
}

TEST_CASE("ModuleMixDown Tests", "[mixer]") {

    // Create the mixer:

    ModuleMixDown mix;

    SECTION("Bind", "Ensures modules can be bound to the mixer") {

        // Create the modules:

        AudioModule mod;
        AudioModule mod1;

        // Bind the modules to the mixer:

        mix.link(&mod);
        mix.link(&mod1);

        // Ensure the number of inputs are valid:

        REQUIRE(mix.num_inputs() == 2);
    }

    SECTION("Process", "Ensures mixer processing works") {

        // Create the modules:

        ConstModule osc1(0.25);
        ConstModule osc2(0.25);

        // Attach the modules:

        mix.link(&osc1);
        mix.link(&osc2);

        // Process the values:

        mix.meta_process();

        // Get the buffer:

        std::unique_ptr<AudioBuffer> buff = mix.get_buffer();

        // Iterate over the buffer:

        for (auto item : *buff) {

            // Ensure the value is around 0.5:

            REQUIRE_THAT(item, Catch::Matchers::WithinAbs(0.5, 0.0001));
        }
    }
}

TEST_CASE("MultiMix Tests", "[mixer]") {

    // Create the mixer:

    MultiMix mix;

    SECTION("Bind", "Ensures modules can be bound to the mixer") {

        // Create the modules:

        AudioModule fmod;
        AudioModule fmod1;

        AudioModule bmod;
        AudioModule bmod1;

        // Bind the modules to the mixer:

        mix.link(&bmod);
        mix.link(&bmod1);

        // Bind the mixer to the modules:

        fmod.link(&mix);
        fmod1.link(&mix);

        // Ensure the number oif inputs and outputs are valid:

        REQUIRE(mix.num_inputs() == 2);
        REQUIRE(mix.num_outputs() == 2);
    }

    SECTION("Process", "Ensures mixer processing works") {

        // Create the modules:

        ConstModule osc1(0.25);
        ConstModule osc2(0.25);

        // Attach the modules:

        mix.link(&osc1);
        mix.link(&osc2);

        // Process the values:

        mix.meta_process();

        // Get the buffers:

        std::unique_ptr<AudioBuffer> buff = mix.get_buffer();
        std::unique_ptr<AudioBuffer> buff1 = mix.get_buffer();

        // Ensure they are not the same memory location:

        REQUIRE(buff != buff1);

        // Iterate over the first buffer:

        for (auto item : *buff) {

            // Ensure the value is around 0.5:

            REQUIRE_THAT(item, Catch::Matchers::WithinAbs(0.50, 0.0001));
        }

        // Iterate over the second buffer:

        for (auto item : *buff1) {

            // Ensure the value is around 0.5:

            REQUIRE_THAT(item, Catch::Matchers::WithinAbs(0.50, 0.0001));
        }
    }
}
