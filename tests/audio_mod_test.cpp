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

#include <gtest/gtest.h>
#include "audio_module.hpp"
#include "source_module.hpp"


TEST(AudioModuleTest, Construct) {

    // Create the AudioModule:

    AudioModule mod;

}

TEST(AudioModuleTest, CreateBuffer) {

    // Create the AudioModule:

    AudioModule mod;

    // Create the buffer:

    auto buff = mod.create_buffer();

    // Check the buffer:

    ASSERT_NE(buff, nullptr);

}

TEST(AudioModuleTest, SetBuffer) {

    // Create the AudioModule:

    AudioModule mod;

    // Create the buffer:

    auto buff = mod.create_buffer();

    // Set the buffer:

    mod.set_buffer(std::move(buff));

}

TEST(AudioModuleTest, SetForward) {

    // Create the AudioModule:

    AudioModule mod;

    // Create the AudioModule:

    AudioModule mod2;

    // Set the forward module:

    mod.set_forward(&mod2);

}

TEST(AudioModuleTest, Bind) {

    // Create the AudioModule:

    AudioModule mod;

    // Create the AudioModule:

    AudioModule mod2;

    // Bind the modules:

    mod.bind(&mod2);

}

TEST(AudioModuleTest, GetInfo) {

    // Create the AudioModule:

    AudioModule mod;

    // Get the info:

    auto* info = mod.get_info();

    // Check the info:

    ASSERT_NE(info, nullptr);

}

TEST(AudioModuleTest, SetInfo) {

    // Create the AudioModule:

    AudioModule mod;

    // Create the info:

    ModuleInfo info;

    // Set the info:

    mod.set_info(info);

}

TEST(AudioModuleTest, MetaProcess) {

    // Create the AudioModule:

    AudioModule mod;

    // Create the DeadEnd, stops backward sampling issues:

    SourceModule mod2;

    // Bind the modules:

    mod.bind(&mod2);

    // Call the meta process:

    mod.meta_process();

}
