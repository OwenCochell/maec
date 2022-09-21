/**
 * @file oscillator_test.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Basic tests for oscillators
 * @version 0.1
 * @date 2022-09-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <gtest/gtest.h>
#include "base_oscillator.hpp"

TEST(ConstantOscillatorTest, Construct) {

    // Create the oscillator:

    ConstantOscillator osc;

    // Ensure the initial value is 0.0:

    ASSERT_EQ(osc.get_value(), 0.0);

}

TEST(ConstantOscillatorTest, ConstructWithValue) {

    // Create the oscillator:

    ConstantOscillator osc(1.0);

    // Ensure the initial value is 1.0:

    ASSERT_EQ(osc.get_value(), 1.0);

}

TEST(ConstantOscillatorTest, SetGetValue) {

    // Create the oscillator:

    ConstantOscillator osc;

    // Set the value:

    osc.set_value(1.0);

    // Ensure the value is 1.0:

    ASSERT_EQ(osc.get_value(), 1.0);

}

TEST(ConstantOscillatorTest, Process) {

    // Specify the value to use:

    long double value = 0.5;

    // Create the oscillator:

    ConstantOscillator osc(value);

    // Process the oscillator:

    osc.process();

    // Get the buffer:

    auto buff = osc.get_buffer();

    // Ensure the buffer is not null:

    ASSERT_NE(buff, nullptr);

    // Ensure the buffer is the correct size:

    ASSERT_EQ(buff->size(), 440);

    // Ensure the buffer is filled with the correct value:

    for (auto& val : *buff) {

        ASSERT_EQ(val, value);

    }

}
