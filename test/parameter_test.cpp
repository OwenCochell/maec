/**
 * @file parameter_test.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Tests for ModuleParameters
 * @version 0.1
 * @date 2023-02-03
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <memory>
#include <vector>

#include "audio_buffer.hpp"
#include "audio_module.hpp"
#include "meta_audio.hpp"
#include "module_param.hpp"
#include "sink_module.hpp"
#include "source_module.hpp"

TEST_CASE("Module Parameter Tests", "[param]") {

    // Construct the parameters:

    ModuleParam const_param(5.0);

    SECTION("ModuleConfig", "Ensures we can preform module configuration") {

        // Create a module to be used:

        AudioModule mod;

        // Set some crazy values:

        auto* info = mod.get_info();

        info->channels = 123;
        info->in_buffer = 456;
        info->sample_rate = 789;

        // Preform the configuration:

        const_param.conf_mod(&mod);

        // Ensure the info sync worked correctly:

        auto* chain_info = const_param.get_chain_info();
        auto* module_info = const_param.get_info();

        REQUIRE(chain_info->buffer_size == info->in_buffer);
        REQUIRE(chain_info->channels == info->channels);
        REQUIRE(chain_info->sample_rate == info->sample_rate);

        REQUIRE(module_info->channels == info->channels);
        REQUIRE(module_info->sample_rate == info->sample_rate);
        REQUIRE(module_info->in_buffer == info->in_buffer);
        REQUIRE(module_info->out_buffer == info->in_buffer);
    }

    SECTION("Constant Parameter Functionality",
            "Ensure we can use constant parameters") {

        // Ensures we can get a constant value:

        auto buff = const_param.get();

        for (auto iter = buff.ibegin(); iter != buff.iend(); ++iter) {

            REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(5., 0.0001));
        }

        // Now, change the value:

        const_param.set_constant(3.0);

        // Test the values multiple times:

        for (int i = 0; i < 5; ++i) {

            buff = const_param.get();

            for (auto iter = buff.ibegin(); iter != buff.iend(); ++iter) {

                REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(3., 0.0001));
            }
        }
    }

    SECTION("Module Parameter Functionality",
            "Ensure we can bind modules to parameters") {

        // Dummy data to utilize

        std::vector<double> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

        // Create a module to use:

        ConstModule osc(3.0);

        // Create a parameter:

        ModuleParam mod_param(&osc);

        // Ensure the value is accurate:

        auto buffp = mod_param.get();

        for (auto iter = buffp.ibegin(); iter < buffp.iend(); ++iter) {

            REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(3.0, 0.0001));
        }

        // Create a dummy module:

        BufferModule src;

        // Bind dummy data buffer:

        AudioBuffer buff(data);

        src.set_rbuffer(&buff);

        // Set the source module

        mod_param.link(&src);

        // Now, ensure values are accurate:

        buffp = mod_param.get();

        for (int i = 0; i < data.size(); ++i) {

            REQUIRE_THAT(buffp.at(i),
                         Catch::Matchers::WithinAbs(data.at(i), 0.0001));
        }
    }
}

TEST_CASE("BaseParamTest", "[param]") {

    // Create the parameters:

    ModuleParam par1(0.);
    ModuleParam par2(1.);
    ModuleParam par3(2.);

    // Create the module:

    BaseParamModule<3> mod(&par1, &par2, &par3);

    SECTION("GetArray", "Ensures array contents are valid") {

        // Get the array:

        auto* arr = mod.get_array();

        // Ensure array contents are correct:

        REQUIRE(arr->at(0) == &par1);
        REQUIRE(arr->at(1) == &par2);
        REQUIRE(arr->at(2) == &par3);
    }

    SECTION("Start", "Ensures parameters are started correctly") {

        // Start the module:

        mod.param_start();

        // Iterate over params:

        auto* arr = mod.get_array();

        for (ModuleParam* param : *arr) {

            // Ensure this param is started:

            REQUIRE(param->get_state() == ModuleParam::State::Started);
        }
    }

    SECTION("Stop", "Ensures parameters are stopped correctly") {

        // Stop the module:

        mod.param_stop();

        // Iterate over params:

        auto* arr = mod.get_array();

        for (ModuleParam* param : *arr) {

            // Ensure this param is stopped:

            REQUIRE(param->get_state() == ModuleParam::State::Stopped);
        }
    }

    SECTION("Info Sync", "Ensures parameters are synced correctly") {

        const int channels = 33;
        const int bsize = 123;
        const int sampler = 789;

        // Define module to utilize:

        SinkModule tmod;

        // Get the chain info:

        auto* info = tmod.get_chain_info();

        // Set some values:

        info->channels = channels;
        info->buffer_size = bsize;
        info->sample_rate = sampler;

        // Preform sync on sink:

        tmod.info_sync();

        // Preform the sync:

        mod.param_info(&tmod);

        auto* arr = mod.get_array();

        // Iterate over each module:

        for (ModuleParam* param : *arr) {

            // Get the info:

            auto* info = param->get_info();
            auto* mcinfo = param->get_chain_info();

            // Ensure values are accurate:

            REQUIRE(mcinfo->buffer_size == bsize);
            REQUIRE(mcinfo->channels == channels);
            REQUIRE(mcinfo->sample_rate == sampler);

            REQUIRE(info->in_buffer == bsize);
            REQUIRE(info->out_buffer == bsize);
            REQUIRE(info->channels == channels);
            REQUIRE(info->sample_rate == sampler);

            // Just for fun, ensure previous module is synced correctly:

            auto& back = param->backward();

            info = back.get_info();
            mcinfo = back.get_chain_info();

            REQUIRE(mcinfo->buffer_size == bsize);
            REQUIRE(mcinfo->channels == channels);
            REQUIRE(mcinfo->sample_rate == sampler);

            REQUIRE(info->in_buffer == bsize);
            REQUIRE(info->out_buffer == bsize);
            REQUIRE(info->channels == channels);
            REQUIRE(info->sample_rate == sampler);
        }
    }
}

TEST_CASE("ParamModule Test", "[param][mod]") {

    // Create the parameters:

    ModuleParam par1(0.);
    ModuleParam par2(1.);
    ModuleParam par3(2.);

    // Create the module:

    ParamModule<3> mod(&par1, &par2, &par3);

    // Create backward module:

    SourceModule source;

    mod.link(&source);

    SECTION("GetArray", "Ensures array contents are valid") {

        // Get the array:

        auto* arr = mod.get_array();

        // Ensure array contents are correct:

        REQUIRE(arr->at(0) == &par1);
        REQUIRE(arr->at(1) == &par2);
        REQUIRE(arr->at(2) == &par3);
    }

    SECTION("Start", "Ensures parameters are started correctly") {

        // Start the module:

        mod.meta_start();

        // Iterate over params:

        auto* arr = mod.get_array();

        for (ModuleParam* param : *arr) {

            // Ensure this param is started:

            REQUIRE(param->get_state() == ModuleParam::State::Started);
        }
    }

    SECTION("Stop", "Ensures parameters are stopped correctly") {

        // Stop the module:

        mod.meta_stop();

        // Iterate over params:

        auto* arr = mod.get_array();

        for (ModuleParam* param : *arr) {

            // Ensure this param is stopped:

            REQUIRE(param->get_state() == ModuleParam::State::Stopped);
        }
    }

    SECTION("Info Sync", "Ensures parameters are synced correctly") {

        // Define values to test:

        const int bsize = 1234;
        const int channels = 22;
        const int sampler = 753;

        // Define a dummy sink:

        SinkModule sink;

        // Define a dummy source:

        SourceModule source;

        // Bind the sink/source:

        sink.link(&mod)->link(&source);

        // Set some crazy values:

        auto* cinfo = sink.get_chain_info();

        cinfo->buffer_size = bsize;
        cinfo->channels = channels;
        cinfo->sample_rate = sampler;

        // Preform chain info sync:

        sink.meta_info_sync();

        // Ensure sync has occurred:

        auto* arr = mod.get_array();

        for (ModuleParam* param : *arr) {

            // Get the info:

            auto* info = param->get_info();
            auto* mcinfo = param->get_chain_info();

            // Ensure values are accurate:

            REQUIRE(mcinfo->buffer_size == bsize);
            REQUIRE(mcinfo->channels == channels);
            REQUIRE(mcinfo->sample_rate == sampler);

            REQUIRE(info->in_buffer == bsize);
            REQUIRE(info->out_buffer == bsize);
            REQUIRE(info->channels == channels);
            REQUIRE(info->sample_rate == sampler);

            // Just for fun, ensure previous module is synced correctly:

            auto& back = param->backward();

            info = back.get_info();
            mcinfo = back.get_chain_info();

            REQUIRE(mcinfo->buffer_size == bsize);
            REQUIRE(mcinfo->channels == channels);
            REQUIRE(mcinfo->sample_rate == sampler);

            REQUIRE(info->in_buffer == bsize);
            REQUIRE(info->out_buffer == bsize);
            REQUIRE(info->channels == channels);
            REQUIRE(info->sample_rate == sampler);
        }
    }
}

TEST_CASE("ParamSink Test", "[param][mod][sink]") {

    // Create the parameters:

    ModuleParam par1(0.);
    ModuleParam par2(1.);
    ModuleParam par3(2.);

    // Create the module:

    ParamSink<3> mod(&par1, &par2, &par3);

    // Create backward module:

    SourceModule source;

    mod.link(&source);

    SECTION("GetArray", "Ensures array contents are valid") {

        // Get the array:

        auto* arr = mod.get_array();

        // Ensure array contents are correct:

        REQUIRE(arr->at(0) == &par1);
        REQUIRE(arr->at(1) == &par2);
        REQUIRE(arr->at(2) == &par3);
    }

    SECTION("Start", "Ensures parameters are started correctly") {

        // Start the module:

        mod.meta_start();

        // Iterate over params:

        auto* arr = mod.get_array();

        for (ModuleParam* param : *arr) {

            // Ensure this param is started:

            REQUIRE(param->get_state() == ModuleParam::State::Started);
        }
    }

    SECTION("Stop", "Ensures parameters are stopped correctly") {

        // Stop the module:

        mod.meta_stop();

        // Iterate over params:

        auto* arr = mod.get_array();

        for (ModuleParam* param : *arr) {

            // Ensure this param is stopped:

            REQUIRE(param->get_state() == ModuleParam::State::Stopped);
        }
    }

    SECTION("Info Sync", "Ensures parameters are synced correctly") {

        // Define values to test:

        const int bsize = 1234;
        const int channels = 22;
        const int sampler = 753;

        // Define a dummy source:

        SourceModule source;

        // Bind the sink/source:

        mod.link(&source);

        // Set some crazy values:

        auto* cinfo = mod.get_chain_info();

        cinfo->buffer_size = bsize;
        cinfo->channels = channels;
        cinfo->sample_rate = sampler;

        // Preform chain info sync:

        mod.meta_info_sync();

        // Ensure sync has occurred:

        auto* arr = mod.get_array();

        for (ModuleParam* param : *arr) {

            // Get the info:

            auto* info = param->get_info();
            auto* mcinfo = param->get_chain_info();

            // Ensure values are accurate:

            REQUIRE(mcinfo->buffer_size == bsize);
            REQUIRE(mcinfo->channels == channels);
            REQUIRE(mcinfo->sample_rate == sampler);

            REQUIRE(info->in_buffer == bsize);
            REQUIRE(info->out_buffer == bsize);
            REQUIRE(info->channels == channels);
            REQUIRE(info->sample_rate == sampler);

            // Just for fun, ensure previous module is synced correctly:

            auto& back = param->backward();

            info = back.get_info();
            mcinfo = back.get_chain_info();

            REQUIRE(mcinfo->buffer_size == bsize);
            REQUIRE(mcinfo->channels == channels);
            REQUIRE(mcinfo->sample_rate == sampler);

            REQUIRE(info->in_buffer == bsize);
            REQUIRE(info->out_buffer == bsize);
            REQUIRE(info->channels == channels);
            REQUIRE(info->sample_rate == sampler);
        }
    }
}

TEST_CASE("ParamSource Test", "[param][mod][source]") {

    // Create the parameters:

    ModuleParam par1(0.);
    ModuleParam par2(1.);
    ModuleParam par3(2.);

    // Create the module:

    ParamSource<3> mod(&par1, &par2, &par3);

    SECTION("GetArray", "Ensures array contents are valid") {

        // Get the array:

        auto* arr = mod.get_array();

        // Ensure array contents are correct:

        REQUIRE(arr->at(0) == &par1);
        REQUIRE(arr->at(1) == &par2);
        REQUIRE(arr->at(2) == &par3);
    }

    SECTION("Start", "Ensures parameters are started correctly") {

        // Start the module:

        mod.meta_start();

        // Iterate over params:

        auto* arr = mod.get_array();

        for (ModuleParam* param : *arr) {

            // Ensure this param is started:

            REQUIRE(param->get_state() == ModuleParam::State::Started);
        }
    }

    SECTION("Stop", "Ensures parameters are stopped correctly") {

        // Stop the module:

        mod.meta_stop();

        // Iterate over params:

        auto* arr = mod.get_array();

        for (ModuleParam* param : *arr) {

            // Ensure this param is stopped:

            REQUIRE(param->get_state() == ModuleParam::State::Stopped);
        }
    }

    SECTION("Info Sync", "Ensures parameters are synced correctly") {

        // Define values to test:

        const int bsize = 1234;
        const int channels = 22;
        const int sampler = 753;

        // Define a dummy sink:

        SinkModule sink;

        // Bind the sink/source:

        sink.link(&mod);

        // Set some crazy values:

        auto* cinfo = sink.get_chain_info();

        cinfo->buffer_size = bsize;
        cinfo->channels = channels;
        cinfo->sample_rate = sampler;

        // Preform chain info sync:

        sink.meta_info_sync();

        // Ensure sync has occurred:

        auto* arr = mod.get_array();

        for (ModuleParam* param : *arr) {

            // Get the info:

            auto* info = param->get_info();
            auto* mcinfo = param->get_chain_info();

            // Ensure values are accurate:

            REQUIRE(mcinfo->buffer_size == bsize);
            REQUIRE(mcinfo->channels == channels);
            REQUIRE(mcinfo->sample_rate == sampler);

            REQUIRE(info->in_buffer == bsize);
            REQUIRE(info->out_buffer == bsize);
            REQUIRE(info->channels == channels);
            REQUIRE(info->sample_rate == sampler);

            // Just for fun, ensure previous module is synced correctly:

            auto& back = param->backward();

            info = back.get_info();
            mcinfo = back.get_chain_info();

            REQUIRE(mcinfo->buffer_size == bsize);
            REQUIRE(mcinfo->channels == channels);
            REQUIRE(mcinfo->sample_rate == sampler);

            REQUIRE(info->in_buffer == bsize);
            REQUIRE(info->out_buffer == bsize);
            REQUIRE(info->channels == channels);
            REQUIRE(info->sample_rate == sampler);
        }
    }
}
