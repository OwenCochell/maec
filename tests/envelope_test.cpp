/**
 * @file envelope_test.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Various tests for envelopes
 * @version 0.1
 * @date 2023-02-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <gtest/gtest.h>

#include "envelope.hpp"

TEST(BaseEnvelopeTest, Construct) {

    BaseEnvelope env;
}

TEST(BaseEnvelopeTest, GetSet) {

    // Create a BaseEnvelope

    BaseEnvelope env;

    // Ensure default values are correct:

    ASSERT_EQ(0, env.get_start_time());
    ASSERT_EQ(0, env.get_stop_time());
    ASSERT_EQ(0, env.get_start_value());
    ASSERT_EQ(0, env.get_stop_value());

    // Ensure operations are correct:

    ASSERT_EQ(0, env.time_diff());
    ASSERT_EQ(0, env.val_diff());
    ASSERT_EQ(0, env.val_divide());

    // Set some values:

    env.set_start_time(1);
    env.set_stop_time(2);
    env.set_start_value(3);
    env.set_stop_value(4);

    // Ensure they are correct:

    ASSERT_EQ(1, env.get_start_time());
    ASSERT_EQ(2, env.get_stop_time());
    ASSERT_EQ(3, env.get_start_value());
    ASSERT_EQ(4, env.get_stop_value());

    // Ensure operations are correct:

    ASSERT_EQ(1, env.time_diff());
    ASSERT_EQ(1, env.val_diff());
    ASSERT_EQ(4 / 3, env.val_divide());

}

TEST(ConstantEnvelopeTest, Construct) {

    ConstantEnvelope cnst;
}

TEST(ConstantEnvelopeTest, Value) {

    // Create a constant envelope:

    ConstantEnvelope cnst;

    // Set the start value:

    const long double value = 0.5;

    cnst.set_start_value(value);

    // Process:

    cnst.meta_process();

    // Get the buffer:

    auto buff = cnst.get_buffer();

    // Ensure the buffer is not null:

    ASSERT_NE(buff, nullptr);

    // Ensure the buffer is the correct size:

    ASSERT_EQ(buff->size(), 440);

    // Ensure values are correct:

    for (auto& val : *buff) {

        ASSERT_EQ(val, value);

    }

    // Jump to an arbitrary time:

    cnst.get_timer()->set_sample(SAMPLE_RATE*2);

    // Process:

    cnst.meta_process();

    // Get the buffer:

    buff = cnst.get_buffer();

    // Ensure the buffer is not null:

    ASSERT_NE(buff, nullptr);

    // Ensure the buffer is the correct size:

    ASSERT_EQ(buff->size(), 440);

    // Ensure values are correct:

    for (auto& val : *buff) {

        ASSERT_EQ(val, value);

    }

}

TEST(ExponentialRampTest, Construct) {

    ExponentialRamp exp;
}

TEST(ExponentialRampTest, Value) {

    // Create an envelope:

    ExponentialRamp exp;

    // Set some values:

    exp.set_start_value(SMALL);
    exp.set_stop_time(5 * nano);
    exp.set_stop_value(1);
    exp.get_info()->buff_size = SAMPLE_RATE;
    exp.get_timer()->set_samplerate(SAMPLE_RATE);

    // Iterate:

    exp.meta_process();
}