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
#include <utility>

#include "audio_module.hpp"
#include "base_module.hpp"
#include "source_module.hpp"

TEST_CASE("AudioModule Test", "[mod]") {

    // Construct the AudioModule:

    AudioModule mod;

    // Add some dummy chain info:

    ChainInfo dumm;
    mod.set_chain_info(&dumm);

    SECTION("State", "Ensures the module will set it's own state correctly") {

        // Ensure initial state is valid:

        REQUIRE(mod.get_state() == BaseModule::State::Created);

        // Start the module:

        mod.start();

        // Ensure our state is correct:

        REQUIRE(mod.get_state() == BaseModule::State::Started);

        // Finish the module:

        mod.finish();

        // Ensure our state is correct:
        // (Automatically skips finishing state)

        REQUIRE(mod.get_state() == BaseModule::State::Finished);

        // Finally, stop the module:

        mod.stop();

        // Ensure the state is correct:

        REQUIRE(mod.get_state() == BaseModule::State::Stopped);
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

        mod.forward(&mod2);

        SECTION("Get Forward",
                "Ensures we can properly get the forward module") {

            // Ensures the forward module is correct:

            REQUIRE(mod.forward() == &mod2);
        }
    }

    SECTION("Link", "Ensures modules can be linked correctly") {

        // Link the modules:

        mod.link(&mod2);

        // Ensure the forward value is correct

        REQUIRE(mod2.forward() == &mod);

        // Ensure the chain number is correct

        // REQUIRE(mod.get_chain_info()->module_num == 2);

        SECTION("Get backward",
                "Ensures we can properly get the backward module") {

            // Ensures the forward module is correct
            // We do this by making sure the address of backwards is the same

            REQUIRE(&mod.backward() == &mod2);
        }
    }

    SECTION("Multi-link", "Ensures we can properly preform multi-links") {

        // Link multiple modules together

        mod.link(&mod2)->link(&smod);

        // Ensure the chain size is correct

        // REQUIRE(mod.get_chain_info()->module_num == 3);

        // Ensure source forward value is correct

        REQUIRE(smod.forward() == &mod2);

        // Ensure middle module values are correct

        REQUIRE(mod2.forward() == &mod);
        REQUIRE(&mod2.backward() == &smod);

        // Ensure the forward most module is correct

        REQUIRE(mod.forward() == nullptr);
        REQUIRE(&mod.backward() == &mod2);
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

            // Ensures the info is correct:

            REQUIRE(mod.get_info()->in_buffer == 0);
        }
    }

    SECTION("GetChainInfo", "Ensures we can properly get and set ChainInfo") {

        // Ensure default is correct:

        REQUIRE(mod2.get_chain_info() == nullptr);

        SECTION("SetChainInfo", "Ensures we can properly set the chain info") {

            // Create some info:

            ChainInfo info;

            // Change some attributes:

            info.buffer_size = 989;
            info.channels = 123;
            info.module_finish = 2;
            info.module_num = 3;
            info.sample_rate = 3456;

            // Set the info:

            mod2.set_chain_info(&info);

            // Get the info that was added:

            auto* ainfo = mod2.get_chain_info();

            // Ensure the values are correct:

            REQUIRE(ainfo->buffer_size == info.buffer_size);
            REQUIRE(ainfo->channels == info.channels);
            REQUIRE(ainfo->module_finish == info.module_finish);
            REQUIRE(ainfo->module_num == info.module_num);
            REQUIRE(ainfo->sample_rate == info.sample_rate);
        }
    }

    SECTION("InfoSync", "Ensures we can correctly preform a sync operation") {

        // Define some crazy AudioInfo:

        auto* audio_info = mod.get_info();

        audio_info->channels = 99;
        audio_info->in_buffer = 123;
        audio_info->out_buffer = 456;

        // Link the modules together:

        mod.link(&mod2);

        // Preform the info sync:

        mod2.info_sync();

        // Ensure the info is correct:

        auto* saudio_info = mod2.get_info();

        REQUIRE(saudio_info->channels == 99);
        REQUIRE(saudio_info->in_buffer == 123);
        REQUIRE(saudio_info->out_buffer == 456);

        SECTION("MetaInfoSync", "Ensures we can preform a meta info sync") {

            // Bind the source:

            mod2.link(&smod);

            // Preform a meta sink operation:

            mod2.meta_info_sync();

            // Ensure the source is correct:

            auto* ssaudio_info = smod.get_info();

            REQUIRE(ssaudio_info->channels == 99);
            REQUIRE(ssaudio_info->in_buffer == 123);
            REQUIRE(ssaudio_info->out_buffer == 456);
        }
    }

    SECTION("Meta Process", "Ensures we can meta process a chain") {

        // Link the modules:

        mod.link(&smod);

        // Call the meta process:

        mod.meta_process();
    }

    SECTION("Meta Start", "Ensures we can meta start") {

        // Link the two modules:

        mod.link(&smod);

        // Meta start the first:

        mod.meta_start();

        // Ensure both have been started:

        REQUIRE(mod.get_state() == BaseModule::State::Started);
        REQUIRE(smod.get_state() == BaseModule::State::Started);
    }

    SECTION("Meta Stop", "Ensures we can meta stop") {

        // Link the two modules:

        mod.link(&smod);

        // Meta stop the first:

        mod.meta_stop();

        // Ensure both have been started:

        REQUIRE(mod.get_state() == BaseModule::State::Stopped);
        REQUIRE(smod.get_state() == BaseModule::State::Stopped);
    }

    SECTION("Meta Finish", "Ensures we can meta finish") {

        // Link the two modules:

        mod.link(&smod);

        // Meta finish the first

        mod.meta_finish();

        // Ensure both modules are finished, and the chain
        // info has been updated

        REQUIRE(mod.get_state() == BaseModule::State::Finished);
        REQUIRE(smod.get_state() == BaseModule::State::Finished);
        REQUIRE(mod.get_chain_info()->module_finish == 2);
    }

    SECTION("Finish", "Ensures we finished modules will immediately be done") {

        // Finish the module

        mod.finish();

        // Afterwards, this module should immediately be finished

        REQUIRE(mod.get_state() == BaseModule::State::Finished);

        // Ensure the finished count is present

        REQUIRE(mod.get_chain_info()->module_finish == 1);
    }

    SECTION("State Methods", "Ensures all state methods work correctly") {

        // Ensures default state is created

        REQUIRE(mod.get_state() == BaseModule::State::Created);

        SECTION("Start", "Ensures start method works correctly") {

            mod.start();

            REQUIRE(mod.get_state() == BaseModule::State::Started);
        }

        SECTION("Stop", "Ensures stop method works correctly") {

            mod.stop();

            REQUIRE(mod.get_state() == BaseModule::State::Stopped);
        }

        SECTION("Finish", "Ensures we can finish correctly") {

            mod.finish();

            // We immediately enter the done state by default

            REQUIRE(mod.get_state() == BaseModule::State::Finished);
        }

        SECTION("Done", "Ensures we can done correctly") {

            mod.done();

            REQUIRE(mod.get_state() == BaseModule::State::Finished);
        }
    }
}

TEST_CASE("Static AudioModule Test", "[mod]") {

    // Construct the AudioModule:

    AudioModule<> mod;

    // Add some dummy chain info:

    ChainInfo dumm;
    mod.set_chain_info(&dumm);

    SECTION("State", "Ensures the module will set it's own state correctly") {

        // Ensure initial state is valid:

        REQUIRE(mod.get_state() == BaseModule::State::Created);

        // Start the module:

        mod.start();

        // Ensure our state is correct:

        REQUIRE(mod.get_state() == BaseModule::State::Started);

        // Finish the module:

        mod.finish();

        // Ensure our state is correct:
        // (Automatically skips finishing state)

        REQUIRE(mod.get_state() == BaseModule::State::Finished);

        // Finally, stop the module:

        mod.stop();

        // Ensure the state is correct:

        REQUIRE(mod.get_state() == BaseModule::State::Stopped);
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

        mod.forward(&mod2);

        SECTION("Get Forward",
                "Ensures we can properly get the forward module") {

            // Ensures the forward module is correct:

            REQUIRE(mod.forward() == &mod2);
        }
    }

    SECTION("Link", "Ensures modules can be linked correctly") {

        // Link the modules:

        mod.link(&mod2);

        // Ensure the forward value is correct

        REQUIRE(mod2.forward() == &mod);

        // Ensure the chain number is correct

        // REQUIRE(mod.get_chain_info()->module_num == 2);

        SECTION("Get backward",
                "Ensures we can properly get the backward module") {

            // Ensures the forward module is correct
            // We do this by making sure the address of backwards is the same

            REQUIRE(&mod.backward() == &mod2);
        }
    }

    SECTION("Multi-link", "Ensures we can properly preform multi-links") {

        // Link multiple modules together

        mod.link(&mod2)->link(&smod);

        // Ensure the chain size is correct

        // REQUIRE(mod.get_chain_info()->module_num == 3);

        // Ensure source forward value is correct

        REQUIRE(smod.forward() == &mod2);

        // Ensure middle module values are correct

        REQUIRE(mod2.forward() == &mod);
        REQUIRE(&mod2.backward() == &smod);

        // Ensure the forward most module is correct

        REQUIRE(mod.forward() == nullptr);
        REQUIRE(&mod.backward() == &mod2);
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

            // Ensures the info is correct:

            REQUIRE(mod.get_info()->in_buffer == 0);
        }
    }

    SECTION("GetChainInfo", "Ensures we can properly get and set ChainInfo") {

        // Ensure default is correct:

        REQUIRE(mod2.get_chain_info() == nullptr);

        SECTION("SetChainInfo", "Ensures we can properly set the chain info") {

            // Create some info:

            ChainInfo info;

            // Change some attributes:

            info.buffer_size = 989;
            info.channels = 123;
            info.module_finish = 2;
            info.module_num = 3;
            info.sample_rate = 3456;

            // Set the info:

            mod2.set_chain_info(&info);

            // Get the info that was added:

            auto* ainfo = mod2.get_chain_info();

            // Ensure the values are correct:

            REQUIRE(ainfo->buffer_size == info.buffer_size);
            REQUIRE(ainfo->channels == info.channels);
            REQUIRE(ainfo->module_finish == info.module_finish);
            REQUIRE(ainfo->module_num == info.module_num);
            REQUIRE(ainfo->sample_rate == info.sample_rate);
        }
    }

    SECTION("InfoSync", "Ensures we can correctly preform a sync operation") {

        // Define some crazy AudioInfo:

        auto* audio_info = mod.get_info();

        audio_info->channels = 99;
        audio_info->in_buffer = 123;
        audio_info->out_buffer = 456;

        // Link the modules together:

        mod.link(&mod2);

        // Preform the info sync:

        mod2.info_sync();

        // Ensure the info is correct:

        auto* saudio_info = mod2.get_info();

        REQUIRE(saudio_info->channels == 99);
        REQUIRE(saudio_info->in_buffer == 123);
        REQUIRE(saudio_info->out_buffer == 456);

        SECTION("MetaInfoSync", "Ensures we can preform a meta info sync") {

            // Bind the source:

            mod2.link(&smod);

            // Preform a meta sink operation:

            mod2.meta_info_sync();

            // Ensure the source is correct:

            auto* ssaudio_info = smod.get_info();

            REQUIRE(ssaudio_info->channels == 99);
            REQUIRE(ssaudio_info->in_buffer == 123);
            REQUIRE(ssaudio_info->out_buffer == 456);
        }
    }

    SECTION("Meta Process", "Ensures we can meta process a chain") {

        // Link the modules:

        mod.link(&smod);

        // Call the meta process:

        mod.meta_process();
    }

    SECTION("Meta Start", "Ensures we can meta start") {

        // Link the two modules:

        mod.link(&smod);

        // Meta start the first:

        mod.meta_start();

        // Ensure both have been started:

        REQUIRE(mod.get_state() == BaseModule::State::Started);
        REQUIRE(smod.get_state() == BaseModule::State::Started);
    }

    SECTION("Meta Stop", "Ensures we can meta stop") {

        // Link the two modules:

        mod.link(&smod);

        // Meta stop the first:

        mod.meta_stop();

        // Ensure both have been started:

        REQUIRE(mod.get_state() == BaseModule::State::Stopped);
        REQUIRE(smod.get_state() == BaseModule::State::Stopped);
    }

    SECTION("Meta Finish", "Ensures we can meta finish") {

        // Link the two modules:

        mod.link(&smod);

        // Meta finish the first

        mod.meta_finish();

        // Ensure both modules are finished, and the chain
        // info has been updated

        REQUIRE(mod.get_state() == BaseModule::State::Finished);
        REQUIRE(smod.get_state() == BaseModule::State::Finished);
        REQUIRE(mod.get_chain_info()->module_finish == 2);
    }

    SECTION("Finish", "Ensures we finished modules will immediately be done") {

        // Finish the module

        mod.finish();

        // Afterwards, this module should immediately be finished

        REQUIRE(mod.get_state() == BaseModule::State::Finished);

        // Ensure the finished count is present

        REQUIRE(mod.get_chain_info()->module_finish == 1);
    }

    SECTION("State Methods", "Ensures all state methods work correctly") {

        // Ensures default state is created

        REQUIRE(mod.get_state() == BaseModule::State::Created);

        SECTION("Start", "Ensures start method works correctly") {

            mod.start();

            REQUIRE(mod.get_state() == BaseModule::State::Started);
        }

        SECTION("Stop", "Ensures stop method works correctly") {

            mod.stop();

            REQUIRE(mod.get_state() == BaseModule::State::Stopped);
        }

        SECTION("Finish", "Ensures we can finish correctly") {

            mod.finish();

            // We immediately enter the done state by default

            REQUIRE(mod.get_state() == BaseModule::State::Finished);
        }

        SECTION("Done", "Ensures we can done correctly") {

            mod.done();

            REQUIRE(mod.get_state() == BaseModule::State::Finished);
        }
    }
}
