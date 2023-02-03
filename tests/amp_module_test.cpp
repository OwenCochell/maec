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

#include <gtest/gtest.h>

#include "amp_module.hpp"
#include "base_oscillator.hpp"

TEST(BaseAmplitudeTest, Construct) {

    // Test default constructor:

    BaseAmplitude amp;
}

TEST(BaseAmplitudeTest, Value) {

    // test constructor:

    BaseAmplitude amp(0.5);

    // test getter:

    ASSERT_DOUBLE_EQ(amp.get_value(), 0.5);

    // Test setter:

    amp.set_value(0.25);

    // Finally, test value changed:

    ASSERT_DOUBLE_EQ(amp.get_value(), 0.25);
}


TEST(AmplitudeScaleTest, Construct) {

    // Test constructor:

    AmplitudeScale amp;
}

TEST(AmplitudeScaleTest, Process) {

    // Create with value of 0.5:

    AmplitudeScale amp(0.5);

    // Create constant oscillator:

    ConstantOscillator osc(1);

    // Bind the module together:

    amp.bind(&osc);

    // Process the data:

    amp.meta_process();

    // Get the buffer:

    auto buff = amp.get_buffer();

    // Ensure data is accurate:

    for(auto iter = buff->ibegin(); iter != buff->send(); ++iter) {

        // Ensure value is correct:

        ASSERT_DOUBLE_EQ(*iter, 0.5);
    }
}

TEST(AmplitudeAddTest, Process) {

    // Create with value of 0.5:

    AmplitudeAdd amp(0.5);

    // Create constant oscillator:

    ConstantOscillator osc(1);

    // Bind the module together:

    amp.bind(&osc);

    // Process the data:

    amp.meta_process();

    // Get the buffer:

    auto buff = amp.get_buffer();

    // Ensure data is accurate:

    for(auto iter = buff->ibegin(); iter != buff->send(); ++iter) {

        // Ensure value is correct:

        ASSERT_DOUBLE_EQ(*iter, 1.5);
    }

}
