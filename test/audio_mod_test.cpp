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
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cstddef>
#include <utility>

#include "audio_buffer.hpp"
#include "audio_module.hpp"
#include "base_module.hpp"
#include "meta_audio.hpp"
#include "sink_module.hpp"
#include "source_module.hpp"

template <typename T>
class TestModule : public AudioModule<TestModule<T>, T> {
public:
    /// Dummy process method that does nothing
    void process() override {}
};

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

    SECTION("ReserveBuffer", "Ensures we can reserve the buffer") {

        // Define some chain info

        const std::size_t size = 123;
        const std::size_t channels = 456;
        const double sr = 789;

        // Set the chain info

        auto* info = mod.get_info();

        info->channels = channels;
        info->sample_rate = sr;
        info->out_buffer = size;

        // Reserve a buffer

        mod.reserve();

        // Grab the buffer

        auto buff = mod.get_buffer();

        // Ensure the buffer has the correct values

        REQUIRE(buff.size() == size * channels);
        REQUIRE(buff.channels() == channels);
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(sr, 0.0001));
    }

    SECTION("Set Buffer", "Ensures we can set the buffer") {

        // Define the buffer values

        const int size = 5;
        const int channels = 7;
        const double sr = 512;

        // Create the buffer

        auto buff = AudioBuffer(size, channels, sr);

        // Set the buffer:

        mod.set_buffer(std::move(buff));

        // Grab the buffer from the module

        auto cbuff = mod.get_buffer();

        // Ensure the buffer values are correct

        REQUIRE(cbuff.size() == size * channels);
        REQUIRE(cbuff.channels() == channels);
        REQUIRE_THAT(cbuff.get_samplerate(),
                     Catch::Matchers::WithinAbs(sr, 0.0001));
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

            // Add a sink to prevent large sync issues

            SinkModule<> sink;

            // Set the info parameters of the sink

            auto* chain_info = sink.get_chain_info();

            chain_info->channels = 123;
            chain_info->buffer_size = 456;
            chain_info->sample_rate = 789;

            // Link the modules together

            sink.link(&mod);

            // Bind the source:

            mod2.link(&smod);

            // Preform a meta sink operation:

            sink.meta_info_sync();

            // Ensure the source is correct:

            auto* ssaudio_info = smod.get_info();

            REQUIRE(ssaudio_info->channels == 123);
            REQUIRE(ssaudio_info->in_buffer == 456);
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

    TestModule<ConstModule> mod;

    // Construct an AudioModule chain

    TestModule<TestModule<ConstModule>> modc;

    // Add some dummy chain info:

    ChainInfo dumm;
    mod.set_chain_info(&dumm);

    // Define the source module

    ConstModule smod;

    // Set the value on the source

    smod.set_value(5);

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

    SECTION("ReserveBuffer", "Ensures we can reserve the buffer") {

        // Define some chain info

        const std::size_t size = 123;
        const std::size_t channels = 456;
        const double sr = 789;

        // Set the chain info

        auto* info = mod.get_info();

        info->channels = channels;
        info->sample_rate = sr;
        info->out_buffer = size;

        // Reserve a buffer

        mod.reserve();

        // Grab the buffer

        auto buff = mod.get_buffer();

        // Ensure the buffer has the correct values

        REQUIRE(buff.size() == size * channels);
        REQUIRE(buff.channels() == channels);
        REQUIRE_THAT(buff.get_samplerate(),
                     Catch::Matchers::WithinAbs(sr, 0.0001));
    }

    SECTION("Set Buffer", "Ensures we can set the buffer") {

        // Define the buffer values

        const int size = 5;
        const int channels = 7;
        const double sr = 512;

        // Create the buffer

        auto buff = AudioBuffer(size, channels, sr);

        // Set the buffer:

        mod.set_buffer(std::move(buff));

        // Grab the buffer from the module

        auto cbuff = mod.get_buffer();

        // Ensure the buffer values are correct

        REQUIRE(cbuff.size() == size * channels);
        REQUIRE(cbuff.channels() == channels);
        REQUIRE_THAT(cbuff.get_samplerate(),
                     Catch::Matchers::WithinAbs(sr, 0.0001));
    }

    SECTION("Set Forward", "Ensures we can properly set the forward module") {

        // Define a junk module to be the forward

        AudioModule<> junk;

        // Set the forward module:

        mod.forward(&junk);

        SECTION("Get Forward",
                "Ensures we can properly get the forward module") {

            // Ensures the forward module is correct:

            REQUIRE(mod.forward() == &junk);
        }
    }

    SECTION("Link", "Ensures modules can be linked correctly") {

        // Link the modules:

        mod.link_static(std::move(smod));

        // Ensure the forward value is correct

        REQUIRE(mod.backward().forward() == &mod);

        // Ensure the chain number is correct

        // REQUIRE(mod.get_chain_info()->module_num == 2);

        SECTION("Get backward",
                "Ensures we can properly get the backward module") {

            // Ensures the forward module is correct
            // We do this by making sure the value is correct

            REQUIRE(mod.backward().get_value() == 5.);
        }
    }

    SECTION("Multi-link", "Ensures we can properly preform multi-links") {

        // Link the modules together

        modc.link_static(std::move(mod))->link_static(std::move(smod));

        // Traverse the chain

        auto& mid = modc.backward();
        auto& back = mid.backward();

        auto* midp = &mid;

        // Ensure the chain size is correct

        // REQUIRE(mod.get_chain_info()->module_num == 3);

        // Ensure source forward value is correct

        REQUIRE(back.forward() == &mid);

        // Ensure middle module values are correct

        REQUIRE(mid.forward() == &modc);
        REQUIRE(mid.backward().get_value() == 5.);

        // Ensure the forward most module is correct

        REQUIRE(modc.forward() == nullptr);
        REQUIRE(&modc.backward() == &mid);
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

        REQUIRE(mod.get_chain_info() == &dumm);

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

            mod.set_chain_info(&info);

            // Get the info that was added:

            auto* ainfo = mod.get_chain_info();

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

        mod.link_static(std::move(smod));

        // Preform the info sync on backward module

        mod.backward().info_sync();

        // Ensure the info is correct:

        auto* saudio_info = mod.get_info();

        REQUIRE(saudio_info->channels == 99);
        REQUIRE(saudio_info->in_buffer == 123);
        REQUIRE(saudio_info->out_buffer == 456);

        SECTION("MetaInfoSync", "Ensures we can preform a meta info sync") {

            // Bind the source:

            mod.link_static(std::move(smod));

            // Add a forward module for sync purposes

            AudioModule<> fmod;

            fmod.link(&mod);

            // Define some crazy AudioInfo

            auto* audio_info = fmod.get_info();

            audio_info->channels = 123;
            audio_info->in_buffer = 456;
            audio_info->out_buffer = 789;

            // Preform a meta sink operation:

            mod.meta_info_sync();

            // Ensure the source is correct:

            auto* ssaudio_info = mod.backward().get_info();

            REQUIRE(ssaudio_info->channels == 123);
            REQUIRE(ssaudio_info->in_buffer == 456);
            REQUIRE(ssaudio_info->out_buffer == 789);
        }
    }

    SECTION("Meta Process", "Ensures we can meta process a chain") {

        // Link the modules:

        mod.link_static(std::move(smod));

        // Call the meta process:

        mod.meta_process();
    }

    SECTION("Meta Start", "Ensures we can meta start") {

        // Link the two modules:

        mod.link_static(std::move(smod));

        // Meta start the first:

        mod.meta_start();

        // Ensure both have been started:

        REQUIRE(mod.get_state() == BaseModule::State::Started);
        REQUIRE(mod.backward().get_state() == BaseModule::State::Started);
    }

    SECTION("Meta Stop", "Ensures we can meta stop") {

        // Link the two modules:

        mod.link_static(std::move(smod));

        // Meta stop the first:

        mod.meta_stop();

        // Ensure both have been started:

        REQUIRE(mod.get_state() == BaseModule::State::Stopped);
        REQUIRE(mod.backward().get_state() == BaseModule::State::Stopped);
    }

    SECTION("Meta Finish", "Ensures we can meta finish") {

        // Link the two modules:

        mod.link_static(std::move(smod));

        // Meta finish the first

        mod.meta_finish();

        // Ensure both modules are finished, and the chain
        // info has been updated

        REQUIRE(mod.get_state() == BaseModule::State::Finished);
        REQUIRE(mod.backward().get_state() == BaseModule::State::Finished);
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

TEST_CASE("Static->Dynamic", "Ensures static module can be linked to dynamic") {

    // Define chain info values

    const double sra = 9999;
    const int channels = 753;
    const int buffer_size = 159;

    // Define constant module values

    const double cval = 2.3;

    // Define the static module to utilize

    TestModule<ConstModule> smod;

    // Define the dynamic module to utilize

    AudioModule dmod;

    // Define constant module to move into static module

    ConstModule cmod(cval);

    // Add a dummy module ahead of the chain
    // so sync can work correctly

    SinkModule<> sink;

    // Update the chain info accordingly

    auto* info = sink.get_chain_info();

    info->sample_rate = sra;
    info->channels = channels;
    info->buffer_size = buffer_size;

    // Preform the link

    sink.link(&dmod)->link(&smod);
    smod.link_static(std::move(cmod));

    SECTION("Link", "Ensures we can link static to dynamic") {

        // Ensure the values are correct

        REQUIRE(smod.forward() == &dmod);
        REQUIRE(&dmod.backward() == &smod);
    }

    // Preform the info sync

    sink.meta_info_sync();

    SECTION("Info Sync", "Ensures info sync values are correct") {

        // Ensure all chain info's propagated

        REQUIRE(sink.get_chain_info() == info);
        REQUIRE(dmod.get_chain_info() == info);
        REQUIRE(smod.get_chain_info() == info);
        REQUIRE(smod.backward().get_chain_info() == info);

        // Ensure all of the chain info is correct
        // For sinks

        auto* minfo = sink.get_info();

        REQUIRE(minfo->channels == channels);
        REQUIRE(minfo->out_buffer == buffer_size);
        REQUIRE(minfo->sample_rate == sra);

        // For dynamic module

        minfo = dmod.get_info();

        REQUIRE(minfo->channels == channels);
        REQUIRE(minfo->out_buffer == buffer_size);
        REQUIRE(minfo->sample_rate == sra);

        // For head static module

        minfo = smod.get_info();

        REQUIRE(minfo->channels == channels);
        REQUIRE(minfo->out_buffer == buffer_size);
        REQUIRE(minfo->sample_rate == sra);

        // For source module

        minfo = smod.backward().get_info();

        REQUIRE(minfo->channels == channels);
        REQUIRE(minfo->out_buffer == buffer_size);
        REQUIRE(minfo->sample_rate == sra);
    }

    SECTION("Meta State Operations",
            "Ensures meta state operations work correctly") {

        SECTION("Meta Start", "Meta start operations work correctly") {

            // Preform the meta start

            dmod.meta_start();

            // Ensure each module has the correct state

            REQUIRE(dmod.get_state() == BaseModule::State::Started);
            REQUIRE(smod.get_state() == BaseModule::State::Started);
            REQUIRE(smod.backward().get_state() == BaseModule::State::Started);
        }

        SECTION("Meta Stop", "Meta stop operations work correctly") {

            // Preform the meta stop

            dmod.meta_stop();

            // Ensure each module has the correct state

            REQUIRE(dmod.get_state() == BaseModule::State::Stopped);
            REQUIRE(smod.get_state() == BaseModule::State::Stopped);
            REQUIRE(smod.backward().get_state() == BaseModule::State::Stopped);
        }

        SECTION("Meta Finish", "Meta finish operations work correctly") {

            // Preform the meta finish

            dmod.meta_finish();

            // Ensure each module has the correct state

            REQUIRE(dmod.get_state() == BaseModule::State::Finished);
            REQUIRE(smod.get_state() == BaseModule::State::Finished);
            REQUIRE(smod.backward().get_state() == BaseModule::State::Finished);
        }
    }

    SECTION("Meta Process", "Ensures we can meta process correctly") {

        // Meta process the chain

        sink.meta_process();

        // Grab the buffer:

        auto buff = sink.get_buffer();

        // Ensure all the values match the constant module

        for (auto& val : buff) {

            // All values MUST match the constant value

            REQUIRE(val == cval);
        }
    }
}

TEST_CASE("Dynamic->Static", "Ensures dynamic module can be linked to static") {

    // Define chain info values

    const double sra = 9999;
    const int channels = 753;
    const int buffer_size = 159;

    // Define constant module values

    const double cval = 2.3;

    // Define the static module to utilize

    SinkModule<AudioModule<>> smod;

    // Define AudioModule to move into SinkModule

    AudioModule tmod;

    // Define constant module to link to static module

    ConstModule cmod(cval);

    // Update the chain info accordingly

    auto* info = smod.get_chain_info();

    info->sample_rate = sra;
    info->channels = channels;
    info->buffer_size = buffer_size;

    // Preform the link

    smod.link_static(std::move(tmod))->link(&cmod);

    SECTION("Link", "Ensures we can link static to dynamic") {

        // Ensure the values are correct

        REQUIRE(&smod.backward().backward() == &cmod);
        REQUIRE(cmod.forward() == &smod.backward());
    }

    // Preform the info sync

    smod.meta_info_sync();

    SECTION("Info Sync", "Ensures info sync values are correct") {

        // Ensure all chain info's propagated

        REQUIRE(smod.get_chain_info() == info);
        REQUIRE(smod.backward().get_chain_info() == info);
        REQUIRE(cmod.get_chain_info() == info);

        // Ensure all of the chain info is correct
        // For sinks

        auto* minfo = smod.get_info();

        REQUIRE(minfo->channels == channels);
        REQUIRE(minfo->out_buffer == buffer_size);
        REQUIRE(minfo->sample_rate == sra);

        // For dynamic module

        minfo = smod.backward().get_info();

        REQUIRE(minfo->channels == channels);
        REQUIRE(minfo->out_buffer == buffer_size);
        REQUIRE(minfo->sample_rate == sra);

        // For head static module

        minfo = cmod.get_info();

        REQUIRE(minfo->channels == channels);
        REQUIRE(minfo->out_buffer == buffer_size);
        REQUIRE(minfo->sample_rate == sra);
    }

    SECTION("Meta State Operations",
            "Ensures meta state operations work correctly") {

        SECTION("Meta Start", "Meta start operations work correctly") {

            // Preform the meta start

            smod.meta_start();

            // Ensure each module has the correct state

            REQUIRE(smod.get_state() == BaseModule::State::Started);
            REQUIRE(smod.backward().get_state() == BaseModule::State::Started);
            REQUIRE(cmod.get_state() == BaseModule::State::Started);
        }

        SECTION("Meta Stop", "Meta stop operations work correctly") {

            // Preform the meta stop

            smod.meta_stop();

            // Ensure each module has the correct state

            REQUIRE(smod.get_state() == BaseModule::State::Stopped);
            REQUIRE(smod.backward().get_state() == BaseModule::State::Stopped);
            REQUIRE(cmod.get_state() == BaseModule::State::Stopped);
        }

        SECTION("Meta Finish", "Meta finish operations work correctly") {

            // Preform the meta finish

            smod.meta_finish();

            // Ensure each module has the correct state

            REQUIRE(smod.get_state() == BaseModule::State::Finished);
            REQUIRE(smod.backward().get_state() == BaseModule::State::Finished);
            REQUIRE(cmod.get_state() == BaseModule::State::Finished);
        }
    }

    SECTION("Meta Process", "Ensures we can meta process correctly") {

        // Meta process the chain

        smod.meta_process();

        // Grab the buffer:

        auto buff = smod.get_buffer();

        // Ensure all the values match the constant module

        for (auto& val : buff) {

            // All values MUST match the constant value

            REQUIRE(val == cval);
        }
    }
}
