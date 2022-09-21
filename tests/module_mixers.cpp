/**
 * @file module_mixdown_test.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Basic tests for mix down modules
 * @version 0.1
 * @date 2022-09-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <gtest/gtest.h>
#include "module_mixer.hpp"


TEST(ModuleMixUpTest, Construct) {

    // Create the mixer:

    ModuleMixUp mix;

}


TEST(ModuleMixDownTest, Construct) {

    // Create the mixer:

    ModuleMixDown mix;

}

TEST(ModuleMixDownTest, Bind) {

    // Create the mixer:

    ModuleMixDown mix;

    // Create a module:

    AudioModule mod;

    // Bind the module to the mixer:

    mix.bind(&mod);

}
