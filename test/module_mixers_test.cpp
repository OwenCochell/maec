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

#include "base_module.hpp"
#include "meta_audio.hpp"
#include "module_mixer.hpp"
#include "sink_module.hpp"
#include "source_module.hpp"

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

        auto buff = mix.get_buffer();
        auto buff1 = mix.get_buffer();

        // Ensure they are not the same memory location:

        // REQUIRE(buff != buff1);

        // Iterate over the first buffer:

        for (auto item : buff) {

            // Ensure the value is around 0.5:

            REQUIRE_THAT(item, Catch::Matchers::WithinAbs(0.50, 0.0001));
        }

        // Iterate over the second buffer:

        for (auto item : buff1) {

            // Ensure the value is around 0.5:

            REQUIRE_THAT(item, Catch::Matchers::WithinAbs(0.50, 0.0001));
        }
    }
}

TEST_CASE("ModuleMixDown Tests", "[mixer]") {

    // Create the mixer:

    ModuleMixDown mix;

    SECTION("Link", "Ensures modules can be linked to the mixer") {

        // Create the modules:

        AudioModule mod;
        AudioModule mod1;

        // Bind the modules to the mixer:

        mix.link(&mod);
        mix.link(&mod1);

        // Ensure the number of inputs are valid:

        REQUIRE(mix.num_inputs() == 2);
    }

    SECTION("Meta Methods", "Ensures meta methods work correctly") {

        // Create the modules:

        SourceModule mod;
        SourceModule mod1;

        // Create some chain info for sync processes

        mix.set_chain_info(new ChainInfo);

        // Bind the modules to the mixer:

        mix.link(&mod);
        mix.link(&mod1);

        SECTION("Meta Start", "Ensures chain meta start works") {

            // Meta start the mixer

            mix.meta_start();

            // Ensure each of the states are started

            REQUIRE(mix.get_state() == BaseModule::State::Started);
            REQUIRE(mod.get_state() == BaseModule::State::Started);
            REQUIRE(mod1.get_state() == BaseModule::State::Started);
        }

        SECTION("Meta Stop", "Ensures chain meta stop works") {

            // Meta stop the mixer

            mix.meta_stop();

            // Ensure each of the states are stopped

            REQUIRE(mix.get_state() == BaseModule::State::Stopped);
            REQUIRE(mod.get_state() == BaseModule::State::Stopped);
            REQUIRE(mod1.get_state() == BaseModule::State::Stopped);
        }

        SECTION("Meta Finish", "Ensures chain meta finish works") {

            // Meta finish the mixer

            mix.meta_finish();

            // Ensure each of the states are finished

            REQUIRE(mix.get_state() == BaseModule::State::Finished);
            REQUIRE(mod.get_state() == BaseModule::State::Finished);
            REQUIRE(mod1.get_state() == BaseModule::State::Finished);

            // Ensure the finished count is valid

            REQUIRE(mix.get_chain_info()->module_finish == 3);
        }

        SECTION("Meta Info Sync", "Ensures we can sync info") {

            // Define module parameters

            const int chans = 123;
            const int in_buff = 456;
            const int out_buff = 789;
            const double sample = 999;

            // Add a sink to prevent segfault

            SinkModule sink;

            // Link the sink to the chain

            sink.link(&mix);

            // Set the module info to something crazy

            auto* info = sink.get_info();

            info->channels = chans;
            info->in_buffer = in_buff;
            info->out_buffer = out_buff;
            info->sample_rate = sample;

            // Preform the info sync

            mix.meta_info_sync();

            // Ensure each module info is correct

            info = mix.get_info();

            REQUIRE(info->channels == chans);
            REQUIRE(info->in_buffer == in_buff);
            REQUIRE(info->out_buffer == out_buff);
            REQUIRE_THAT(info->sample_rate,
                         Catch::Matchers::WithinAbs(sample, 0.0001));

            info = mod.get_info();

            REQUIRE(info->channels == chans);
            REQUIRE(info->in_buffer == in_buff);
            REQUIRE(info->out_buffer == out_buff);
            REQUIRE_THAT(info->sample_rate,
                         Catch::Matchers::WithinAbs(sample, 0.0001));

            info = mod1.get_info();

            REQUIRE(info->channels == chans);
            REQUIRE(info->in_buffer == in_buff);
            REQUIRE(info->out_buffer == out_buff);
            REQUIRE_THAT(info->sample_rate,
                         Catch::Matchers::WithinAbs(sample, 0.0001));
        }
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

        auto buff = mix.get_buffer();

        // Iterate over the buffer:

        for (auto item : buff) {

            // Ensure the value is around 0.5:

            REQUIRE_THAT(item, Catch::Matchers::WithinAbs(0.5, 0.0001));
        }
    }

    SECTION("Chain Process",
            "Ensures larger chains can be properly processed") {

        // Create the sources

        ConstModule osc1(0.25);
        ConstModule osc2(0.25);

        // Create the counters

        Counter count1;
        Counter count2;

        // Link the modules together

        mix.link(&count1)->link(&osc1);
        mix.link(&count2)->link(&osc2);

        // Process the values

        mix.meta_process();

        // Get the buffer

        auto buff = mix.get_buffer();

        // Iterate over the buffer:

        for (auto item : buff) {

            // Ensure the value is around 0.5:

            REQUIRE_THAT(item, Catch::Matchers::WithinAbs(0.5, 0.0001));
        }

        // Ensure the count values are valid

        REQUIRE(count1.processed() == 1);
        REQUIRE(count2.processed() == 1);
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

        auto buff = mix.get_buffer();
        auto buff1 = mix.get_buffer();

        // Ensure they are not the same memory location:

        // REQUIRE(buff != buff1);

        // Iterate over the first buffer:

        for (auto item : buff) {

            // Ensure the value is around 0.5:

            REQUIRE_THAT(item, Catch::Matchers::WithinAbs(0.50, 0.0001));
        }

        // Iterate over the second buffer:

        for (auto item : buff1) {

            // Ensure the value is around 0.5:

            REQUIRE_THAT(item, Catch::Matchers::WithinAbs(0.50, 0.0001));
        }
    }
}
