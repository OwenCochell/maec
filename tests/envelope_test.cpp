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
    ASSERT_DOUBLE_EQ(4.0 / 3.0, env.val_divide());

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

    // Seconds it will take to ramp to value:

    int seconds = 1;

    // Final value to ramp to:

    long double final_value = 1;

    // Set some values:

    exp.set_start_value(SMALL);
    exp.set_stop_time(nano * seconds);
    exp.set_stop_value(final_value);
    exp.get_info()->buff_size = 1000;
    exp.get_timer()->set_samplerate(1000);

    long double last = -1;
    long double delta = 0;

    for (int i = 0; i < seconds; ++i) {

        // Meta process:

        exp.meta_process();

        // Grab the buffer:

        auto buff = exp.get_buffer();

        // Ensure buffer is accurate

        for (auto iter = buff->ibegin(); iter != buff->iend(); ++iter) {

            // Ensure this value is bigger than the last:

            ASSERT_GT(*iter, last);

            if (last != -1) {

                // Ensure delta is correct, should always be bigger:
                // Not a super great way to ensure this ramp is truly exponential,
                // but as long as the delta keeps getting higher than it is close enough (for now)

                ASSERT_GT(*iter - last, delta);

                delta = *iter - last;
            }

            last = *iter;

        }
    }

    // Finally, ensure last value is very close to final:

    ASSERT_NEAR(last, final_value, 0.05);

}

TEST(ExponentialRampTest, ValueLarge) {

    // Create an envelope:

    ExponentialRamp exp;

    // Seconds it will take to ramp to value:

    int seconds = 120;

    // Final value to ramp to:

    long double final_value = 1;

    // Set some values:

    exp.set_start_value(SMALL);
    exp.set_stop_time(nano * seconds);
    exp.set_stop_value(final_value);
    exp.get_info()->buff_size = 5000;
    exp.get_timer()->set_samplerate(1000);

    long double last = -1;
    long double delta = 0;

    for (int i = 0; i < seconds / 5; ++i) {

        // Meta process:

        exp.meta_process();

        // Grab the buffer:

        auto buff = exp.get_buffer();

        // Ensure buffer is accurate

        for (auto iter = buff->ibegin(); iter != buff->iend(); ++iter) {

            // Ensure this value is bigger than the last:

            ASSERT_GT(*iter, last);

            if (last != -1) {

                // Ensure delta is correct, should always be bigger:
                // Not a super great way to ensure this ramp is truly exponential,
                // but as long as the delta keeps getting higher than it is close enough (for now)

                ASSERT_GT(*iter - last, delta);

                delta = *iter - last;
            }

            last = *iter;

        }
    }

    // Finally, ensure last value is very close to final:

    ASSERT_NEAR(last, final_value, 0.05);

}

TEST(LinearRampTest, Construct) {

    LinearRamp lin;
}

TEST(LinearRampTest, Value) {

    // Create an envelope:

    LinearRamp lin;

    // Seconds it will take to ramp to value:

    int seconds = 1;

    // Final value to ramp to:

    long double final_value = 1;

    // Set some values:

    lin.set_start_value(SMALL);
    lin.set_stop_time(nano * seconds);
    lin.set_stop_value(final_value);
    lin.get_info()->buff_size = 1000;
    lin.get_timer()->set_samplerate(1000);

    long double last = 0;
    long double delta = 0;

    for (int i = 0; i < seconds; ++i) {

        // Meta process:

        lin.meta_process();

        // Grab the buffer:

        auto buff = lin.get_buffer();

        // Ensure buffer is accurate

        for (auto iter = buff->ibegin(); iter != buff->iend(); ++iter) {

            // Ensure this value is bigger than the last:

            ASSERT_GT(*iter, last);

            if (iter.get_index() > 1) {

                // Ensure the delta is the same
                // (or very close)

                ASSERT_DOUBLE_EQ(*iter - last, delta);

            }

            delta = *iter - last;
            last = *iter;

        }
    }

    // Finally, ensure last value is very close to final:

    ASSERT_NEAR(last, final_value, 0.05);

}

TEST(LinearRampTest, ValueLarge) {

    // Create an envelope:

    LinearRamp lin;

    // Seconds it will take to ramp to value:

    int seconds = 120;

    // Final value to ramp to:

    long double final_value = 1;

    // Set some values:

    lin.set_start_value(SMALL);
    lin.set_stop_time(nano * seconds);
    lin.set_stop_value(final_value);
    lin.get_info()->buff_size = 5000;
    lin.get_timer()->set_samplerate(1000);

    long double last = 0;
    long double delta = 0;

    for (int i = 0; i < seconds / 5; ++i) {

        // Meta process:

        lin.meta_process();

        // Grab the buffer:

        auto buff = lin.get_buffer();

        // Ensure buffer is accurate

        for (auto iter = buff->ibegin(); iter != buff->iend(); ++iter) {

            // Ensure this value is bigger than the last:

            ASSERT_GT(*iter, last);

            if (iter.get_index() > 1) {

                // Ensure the delta is the same
                // (or very close)

                ASSERT_NEAR(*iter - last, delta, 0.0001);

            }

            delta = *iter - last;
            last = *iter;

        }
    }

    // Finally, ensure last value is very close to final:

    ASSERT_NEAR(last, final_value, 0.05);

}

TEST(SetValueTest, Construct) {

    SetValue val;
}

TEST(SetValueTest, Value) {

    // Create an envelope:

    SetValue val;

    // Seconds it will take to ramp to value:

    int seconds = 1;

    // Final value to ramp to:

    long double final_value = 1;

    // Set some values:

    val.set_start_value(0);
    val.set_stop_time(nano * seconds);
    val.set_stop_value(final_value);
    val.get_info()->buff_size = 1000;
    val.get_timer()->set_samplerate(1000);

    for (int i = 0; i < 2; ++i) {

        // Meta process:

        val.meta_process();

        // Grab the buffer:

        auto buff = val.get_buffer();

        for (auto iter = buff->ibegin(); iter != buff->iend(); ++iter) {

            // Determine if we are start or stop:

            if (i == 0) {

                // Ensure we are the start value:

                ASSERT_DOUBLE_EQ(0, *iter);
            }

            if (i == 1) {

                // Ensure we are the stop value:

                ASSERT_DOUBLE_EQ(final_value, *iter);
            }
        }
    }

}

TEST(SetValueTest, ValueOffset) {

    // Create an envelope:

    SetValue val;

    // Seconds it will take to ramp to value:
    // (Choose a super weird value here)

    double seconds = 0.3486;

    // Final value to ramp to:

    long double final_value = 1;

    // Set some values:

    val.set_start_value(0);
    val.set_stop_time(nano * seconds);
    val.set_stop_value(final_value);
    val.get_info()->buff_size = 1000;
    val.get_timer()->set_samplerate(1000);

    // Meta process:

    val.meta_process();

    // Grab the buffer:

    auto buff = val.get_buffer();

    for (auto iter = buff->ibegin(); iter != buff->iend(); ++iter) {

        // Determine the value to check:

        if (iter.get_index() < static_cast<int>(1000 * seconds)) {

            ASSERT_DOUBLE_EQ(*iter, 0);
        }

        else {
        
            ASSERT_DOUBLE_EQ(*iter, 1);
        }
    }

}
