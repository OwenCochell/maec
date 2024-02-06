/**
 * @file audio_mod_test.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Basic tests for audio modules
 * @version 0.1
 * @date 2022-09-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <catch2/catch_test_macros.hpp>

#include "audio_module.hpp"
#include "source_module.hpp"

TEST_CASE("AudioModule Test", "[mod]") {

    // Construct the AudioModule:

    AudioModule mod;

    SECTION("State", "Ensures the module will set it's own state correctly") {

        // Ensure initial state is valid:

        REQUIRE(mod.get_state() == AudioModule::State::Created);

        // Start the module:

        mod.start();

        // Ensure our state is correct:

        REQUIRE(mod.get_state() == AudioModule::State::Started);

        // Finish the module:

        mod.finish();

        // Ensure our state is correct:
        // (Automatically skips finishing state)

        REQUIRE(mod.get_state() == AudioModule::State::Finished);

        // Finally, stop the module:

        mod.stop();

        // Ensure the state is correct:

        REQUIRE(mod.get_state() == AudioModule::State::Stopped);
    }

    SECTION("CreateBuffer", "Ensures we can create a buffer") {

        // Create the buffer:

        auto buff = mod.create_buffer();

        // Ensure the buffer is not null:

        REQUIRE(buff != nullptr);

        SECTION("Set Buffer", "Ensures we can set the buffer") {

            // Set the buffer:

            mod.set_buffer(std::move(buff));

            // Ensure the buffer is null:

            REQUIRE(buff == nullptr);
        }
    }

    // Create another module for testing:

    AudioModule mod2;

    // Create the SourceModule, stops backward sampling issues:

    SourceModule smod;

    SECTION("Set Forward", "Ensures we can properly set the forward module") {

        // Set the forward module:

        mod.set_forward(&mod2);

        SECTION("Get Forward", "Ensures we can properly get the forward module") {

            // Ensures the forward module is correct:

            REQUIRE(mod.get_forward() == &mod2);
        }
    }

    SECTION("Bind", "Ensures modules can be bound correctly") {

        // Bind the modules:

        mod.bind(&mod2);

        SECTION("Get backward", "Ensures we can properly get the backward module") {

            // Ensures the forward module is correct:

            REQUIRE(mod.get_backward() == &mod2);
        }
    }

    SECTION("GetInfo", "Ensures we can properly get the info") {

        // Get the info:

        auto* info = mod.get_info();

        // Check the info:

        REQUIRE(info != nullptr);

        SECTION("Set Info", "Ensures we can properly set the info") {

            // Create some info:

            ModuleInfo info2;
            info2.in_buffer = 0;

            // Set the info:

            mod.set_info(info2);
        }
    }

    SECTION("Meta Process", "Ensures we can meta process a chain") {

        // Bind the modules:

        mod.bind(&smod);

        // Call the meta process:

        mod.meta_process();
    }

    SECTION("Meta Start", "Ensures we can meta start") {

        // Set some crazy info in the start module:

        auto* info = smod.get_info();

        info->channels = 99;
        info->in_buffer = 0;
        info->out_buffer = 12345;
        info->sample_rate = 6789;

        // Link the two modules:

        mod.bind(&smod);

        // Meta start the first:

        mod.meta_start();

        // Ensure both have been started:

        REQUIRE(mod.get_state() == AudioModule::State::Started);
        REQUIRE(smod.get_state() == AudioModule::State::Started);

        // Ensure module info is valid:

        auto* info2 = mod.get_info();

        REQUIRE(info2->channels == 99);
        REQUIRE(info2->in_buffer == 0);
        REQUIRE(info2->out_buffer == 12345);
        REQUIRE(info2->sample_rate == 6789);
    }

    SECTION("Meta Stop", "Ensures we can meta stop") {

        // Link the two modules:

        mod.bind(&smod);

        // Meta stop the first:

        mod.meta_stop();

        // Ensure both have been started:

        REQUIRE(mod.get_state() == AudioModule::State::Stopped);
        REQUIRE(smod.get_state() == AudioModule::State::Stopped);
    }
}
