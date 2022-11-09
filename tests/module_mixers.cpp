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

#include <gtest/gtest.h>
#include "module_mixer.hpp"
#include "base_oscillator.hpp"


TEST(ModuleMixUpTest, Construct) {

    // Create the mixer:

    ModuleMixUp mix;

}

TEST(ModuleMixDownTest, Construct) {

    // Create the mixer:

    ModuleMixDown mix;

}

TEST(ModuleMixMultiTest, Construct) {

    // Create the mixer:

    MultiMix mix;

}

TEST(ModuleMixDownTest, Bind) {

    // Create the mixer:

    ModuleMixDown mix;

    // Create the modules:

    AudioModule mod;
    AudioModule mod1;

    // Bind the modules to the mixer:

    mix.bind(&mod);
    mix.bind(&mod1);

    // Ensure the number of inputs are valid:

    ASSERT_EQ(mix.num_inputs(), 2);

}

TEST(ModuleMixUpTest, Bind) {

    // Create the mixer:

    ModuleMixUp mix;

    // Create the modules:

    AudioModule mod;
    AudioModule mod1;

    // Bind the mixer to each module:

    mod.bind(&mix);
    mod1.bind(&mix);

    // Ensure the number of modules attached to valid:

    ASSERT_EQ(mix.num_outputs(), 2);
}

TEST(ModuleMixMultiTest, Bind) {

    // Create the mixer:

    MultiMix mix;

    // Create the modules:

    AudioModule fmod;
    AudioModule fmod1;

    AudioModule bmod;
    AudioModule bmod1;

    // Bind the modules to the mixer:

    mix.bind(&bmod);
    mix.bind(&bmod1);

    // Bind the mixer to the modules:

    fmod.bind(&mix);
    fmod1.bind(&mix);

    // Ensure the number oif inputs and outputs are valid:

    ASSERT_EQ(mix.num_inputs(), 2);
    ASSERT_EQ(mix.num_outputs(), 2);

}

TEST(ModuleMixDownTest, Process) {

    // Create the mixer:

    ModuleMixDown mix;

    // Create the modules:

    ConstantOscillator osc1(0.25);
    ConstantOscillator osc2(0.25);

    // Attach the modules:

    mix.bind(&osc1);
    mix.bind(&osc2);

    // Process the values:

    mix.meta_process();

    // Get the buffer:

    AudioBuffer buff = mix.get_buffer();

    // Iterate over the buffer:

    for (auto item : *buff) {

        // Ensure the value is around 0.5:

        ASSERT_DOUBLE_EQ(item, 0.5);

    }
}

TEST(ModuleMixUpTest, Process) {

    // Create the mixer:

    ModuleMixUp mix;

    // Create the modules:

    ConstantOscillator osc1(0.50);

    // Attach the modules:

    mix.bind(&osc1);

    // Process the values:

    mix.meta_process();

    // Get the buffers:

    AudioBuffer buff = mix.get_buffer();
    AudioBuffer buff1 = mix.get_buffer();

    // Ensure they are not the same memory location:

    ASSERT_TRUE(buff != buff1);

    // Iterate over the first buffer:

    for (auto item : *buff) {

        // Ensure the value is around 0.5:

        ASSERT_DOUBLE_EQ(item, 0.50);

    }

    // Iterate over the second buffer:

    for (auto item : *buff1) {

        // Ensure the value is around 0.5:

        ASSERT_DOUBLE_EQ(item, 0.50);

    }
}

TEST(ModuleMixMultiTest, Process) {

    // Create the mixer:

    MultiMix mix;

    // Create the modules:

    ConstantOscillator osc1(0.25);
    ConstantOscillator osc2(0.25);

    // Attach the modules:

    mix.bind(&osc1);
    mix.bind(&osc2);

    // Process the values:

    mix.meta_process();

    // Get the buffers:

    AudioBuffer buff = mix.get_buffer();
    AudioBuffer buff1 = mix.get_buffer();

    // Ensure they are not the same memory location:

    ASSERT_TRUE(buff != buff1);

    // Iterate over the first buffer:

    for (auto item : *buff) {

        // Ensure the value is around 0.5:

        ASSERT_DOUBLE_EQ(item, 0.50);

    }

    // Iterate over the second buffer:

    for (auto item : *buff1) {

        // Ensure the value is around 0.5:

        ASSERT_DOUBLE_EQ(item, 0.50);

    }

}
