/**
 * @file envelope_test.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Various tests for envelopes
 * @version 0.1
 * @date 2023-02-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <cmath>

#include "envelope.hpp"

TEST_CASE("BaseEnvelopeTest", "[env]") {

    // Ensure we can construct a BaseEnvelope:

    BaseEnvelope env;

    // Ensure default values are correct:

    REQUIRE(0 == env.get_start_time());
    REQUIRE(0 == env.get_stop_time());
    REQUIRE(0 == env.get_start_value());
    REQUIRE(0 == env.get_stop_value());

    // Ensure operations are correct:

    REQUIRE(0 == env.time_diff());
    REQUIRE(0 == env.val_diff());
    REQUIRE(0 == env.val_divide());

    SECTION("GetSet", "Ensures setters work properly") {

        // Set some values:

        env.set_start_time(1);
        env.set_stop_time(2);
        env.set_start_value(3);
        env.set_stop_value(4);

        // Ensure operations are correct:

        REQUIRE(1 == env.time_diff());
        REQUIRE(1 == env.val_diff());
        REQUIRE_THAT(4. / 3., Catch::Matchers::WithinAbs(env.val_divide(), 0.0001));
    }

    SECTION("Time", "Ensures timing operations work correctly") {

        // Check number of samples left:

        env.set_start_time(0);
        env.set_stop_time(NANO * 5);

        REQUIRE(220507 == env.remaining_samples());

        // Check time increment is valid:

        auto time = env.get_time();

        REQUIRE(env.get_time_inc() == time);
        REQUIRE(env.get_time_inc() > time);
    }
}

TEST_CASE("DurationEnvelope Test", "[env]") {

    // Ensure we can construct a DurationEnvelope:

    DurationEnvelope dur;

    // Ensure default value is correct:

    REQUIRE(nullptr == dur.get_envelope());

    SECTION("GetSet", "Ensures setters work properly") {

        // Create a SetValue:

        SetValue val;

        val.set_start_value(1);
        val.set_stop_value(5);
        val.get_info()->out_buffer = 100;
        val.get_info()->sample_rate = 100;
        val.get_timer()->set_samplerate(100);

        // Set the envelope:

        dur.set_envelope(&val);

        // Ensure value is correct:

        REQUIRE(&val == dur.get_envelope());\

        SECTION("Process", "Ensures processing works correctly") {

            // Configure the duration envelope:

            dur.set_duration(3 * NANO);

            // Start the envelope:

            dur.start();

            SECTION("Start", "Ensures the time values are correct when started") {

                // Ensure start and stop times are valid:

                REQUIRE(0 == dur.get_envelope()->get_start_time());
                REQUIRE(3 * NANO == dur.get_envelope()->get_stop_time());

                // Set the timer to a funky time:

                dur.get_timer()->set_sample(100);

                // Start the envelope once more:

                dur.start();

                // Ensure start and stop times are valid:

                const int64_t time = dur.get_timer()->get_npf() * 100;

                REQUIRE(time == dur.get_envelope()->get_start_time());
                REQUIRE(3 * NANO + time == dur.get_envelope()->get_stop_time());
            }

            int index = 0;

            while (index < 3) {

                dur.meta_process();

                auto buff = dur.get_buffer();

                for (auto iter = buff->ibegin(); iter != buff->iend(); ++iter) {

                    REQUIRE(1 == *iter);
                }

                ++index;
            }

            dur.meta_process();

            auto buff = dur.get_buffer();

            for (auto iter = buff->ibegin(); iter != buff->iend(); ++iter) {

                REQUIRE(5 == *iter);
            }
        }
    }
}

TEST_CASE("ConstantEnvelope Test", "[env]") {

    // Construct a ConstantEnvelope:

    ConstantEnvelope cnst;

    SECTION("Value", "Ensures the returned values are correct") {

        // Set the start value:

        const long double value = 0.5;

        cnst.set_start_value(value);

        // Process:

        cnst.meta_process();

        // Get the buffer:

        auto buff = cnst.get_buffer();

        // Ensure the buffer is not null:

        REQUIRE(nullptr != buff);

        // Ensure the buffer is the correct size:

        REQUIRE(440 == buff->size());

        // Ensure values are correct:

        for (auto& val : *buff) {

            REQUIRE_THAT(val, Catch::Matchers::WithinAbs(value, 0.0001));
        }

        // Jump to an arbitrary time:

        cnst.get_timer()->set_sample(SAMPLE_RATE*2);

        // Process:

        cnst.meta_process();

        // Get the buffer:

        buff = cnst.get_buffer();

        // Ensure the buffer is not null:

        REQUIRE(nullptr != buff);

        // Ensure the buffer is the correct size:

        REQUIRE(440 == buff->size());

        // Ensure values are correct:

        for (auto& val : *buff) {

            REQUIRE_THAT(val, Catch::Matchers::WithinAbs(value, 0.0001));
        }
    }
}

TEST_CASE("ExponentialRamp Test", "[env]") {

    // Construct an ExponentialRamp:

    ExponentialRamp exp;

    SECTION("Value", "Ensures the returned values are correct") {

        // Seconds it will take to ramp to value:

        const int seconds = 1;

        // Final value to ramp to:

        const long double final_value = 1;

        // Set some values:

        exp.set_start_value(SMALL);
        exp.set_stop_time(NANO * seconds);
        exp.set_stop_value(final_value);
        exp.get_info()->out_buffer = 1000;
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

                REQUIRE(last < *iter);

                if (last != -1) {

                    // Ensure delta is correct, should always be bigger
                    // Not a super great way to ensure this ramp is truly exponential,
                    // but as long as the delta keeps getting higher than it is close enough (for now)

                    REQUIRE(delta < *iter - last);

                    delta = *iter - last;
                }

                last = *iter;
            }
        }

        // Finally, ensure last value is very close to final:

        REQUIRE_THAT(last, Catch::Matchers::WithinAbs(final_value, 0.05));
    }

    SECTION("ValueLarge", "Ensures the returned values are correct with large inputs") {

        // Seconds it will take to ramp to value:

        int seconds = 120;

        // Final value to ramp to:

        long double final_value = 1;

        // Set some values:

        exp.set_start_value(SMALL);
        exp.set_stop_time(NANO * seconds);
        exp.set_stop_value(final_value);
        exp.get_info()->out_buffer = 5000;
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

                REQUIRE(*iter > last);

                if (last != -1) {

                    // Ensure delta is correct, should always be bigger:
                    // Not a super great way to ensure this ramp is truly
                    // exponential, but as long as the delta keeps getting
                    // higher than it is close enough (for now)

                    REQUIRE(*iter - last > delta);

                    delta = *iter - last;
                }

                last = *iter;
            }
        }

        // Finally, ensure last value is very close to final:

        REQUIRE_THAT(last, Catch::Matchers::WithinAbs(final_value, 0.05));
    }
}

TEST_CASE("LinearRamp Test", "[env]") {

    // Construct a LinearRamp:

    LinearRamp lin;

    SECTION("Value", "Ensures we produce correct values") {

        // Create an envelope:

        LinearRamp lin;

        // Seconds it will take to ramp to value:

        int seconds = 1;

        // Final value to ramp to:

        long double final_value = 1;

        // Set some values:

        lin.set_start_value(SMALL);
        lin.set_stop_time(NANO * seconds);
        lin.set_stop_value(final_value);
        lin.get_info()->out_buffer = 1000;
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

                REQUIRE(*iter > last);

                if (iter.get_index() > 1) {

                    // Ensure the delta is the same
                    // (or very close)

                    REQUIRE_THAT(*iter - last, Catch::Matchers::WithinAbs(delta, 0.0001));
                }

                delta = *iter - last;
                last = *iter;
            }
        }

        // Finally, ensure last value is very close to final:

        REQUIRE_THAT(last, Catch::Matchers::WithinAbs(final_value, 0.05));

        lin.meta_process();

        auto buff = lin.get_buffer();
    }

    SECTION("Value Large", "Ensures we return correct values with large inputs") {

        // Seconds it will take to ramp to value:

        int seconds = 120;

        // Final value to ramp to:

        long double final_value = 1;

        // Set some values:

        lin.set_start_value(SMALL);
        lin.set_stop_time(NANO * seconds);
        lin.set_stop_value(final_value);
        lin.get_info()->out_buffer = 5000;
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

                REQUIRE(*iter > last);

                if (iter.get_index() > 1) {

                    // Ensure the delta is the same
                    // (or very close)

                    REQUIRE_THAT(*iter - last, Catch::Matchers::WithinAbs(delta, 0.0001));
                }

                delta = *iter - last;
                last = *iter;
            }
        }

        // Finally, ensure last value is very close to final:

        REQUIRE_THAT(last, Catch::Matchers::WithinAbs(final_value, 0.05));
    }
}

TEST_CASE("SetValue Test", "[env]") {

    // Construct a SetValue envelope:

    SetValue val;

    SECTION("Value", "Ensures SetValue works with some generic values") {

        // Create an envelope:

        SetValue val;

        // Seconds it will take to ramp to value:

        int seconds = 1;

        // Final value to ramp to:

        long double final_value = 1;

        // Set some values:

        val.set_start_value(0);
        val.set_stop_time(NANO * seconds);
        val.set_stop_value(final_value);
        val.get_info()->out_buffer = 1000;
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

                    REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(0, 0.0001));
                }

                if (i == 1) {

                    // Ensure we are the stop value:

                    REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(final_value, 0.0001));
                }
            }
        }
    }

    SECTION("Value Offset", "Ensures we work correctly with a weird, offset value") {

        // Seconds it will take to ramp to value:
        // (Choose a super weird value here)

        double seconds = 0.3486;

        // Final value to ramp to:

        long double final_value = 1;

        // Set some values:

        val.set_start_value(0);
        val.set_stop_time(static_cast<int64_t>(NANO * seconds));
        val.set_stop_value(final_value);
        val.get_info()->out_buffer = 1000;
        val.get_timer()->set_samplerate(1000);

        // Meta process:

        val.meta_process();

        // Grab the buffer:

        auto buff = val.get_buffer();

        for (auto iter = buff->ibegin(); iter != buff->iend(); ++iter) {

            // Determine the value to check:

            if (iter.get_index() < static_cast<int>(1000 * seconds)) {

                REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(0, 0.0001));
            }

            else {

                REQUIRE_THAT(*iter, Catch::Matchers::WithinAbs(final_value, 0.0001));
            }
        }
    }
}

TEST_CASE("ChainEnvelope", "[env]") {

    SECTION("AddEnvelope", "Ensures we can add an envelope to the chain") {

        // Construct the chain envelope:

        ChainEnvelope chain;

        // Create an envelope:

        SetValue env;

        // Add the envelope:

        chain.add_envelope(&env);

        // Get the next envelope:

        chain.start();

        // Ensure it is the one we added:

        ASSERT_EQ(chain.get_current(), &env);
    }
}

TEST(ChainEnvelopeTest, RepeatEnvelope) {

    /**
     * @brief Determines if the ChainEnvelope repeats envelope with -1 stop time
     *
     * We ensure that envelopes with stop time of -1 are correctly repeated
     * indefinitely, until the user manually advances.
     *
     */

    // Construct the chain envelope:

    ChainEnvelope chain;

    chain.get_timer()->set_samplerate(100);
    chain.get_info()->out_buffer = 100;

    // Create an envelope:

    ConstantEnvelope cnst;

    // Set the values:

    cnst.set_stop_time(-1);
    cnst.set_start_value(5);

    // Add it to the envelope:

    chain.add_envelope(&cnst);

    // Sample once:

    chain.start();

    chain.meta_process();

    auto buff = chain.get_buffer();

    for (auto iter = buff->ibegin(); iter != buff->iend(); ++iter) {

        ASSERT_EQ(*iter, 5);
    }

    // Sample twice:

    chain.meta_process();

    buff = chain.get_buffer();

    for (auto iter = buff->ibegin(); iter != buff->iend(); ++iter) {

        ASSERT_EQ(*iter, 5);
    }

    // Sample THRICE!

    chain.meta_process();

    buff = chain.get_buffer();

    for (auto iter = buff->ibegin(); iter != buff->iend(); ++iter) {

        ASSERT_EQ(*iter, 5);
    }
}

TEST(ChainEnvelopeTest, MultiEnvelopeTest) {

    // Construct the chain envelope:

    ChainEnvelope chain;

    chain.get_timer()->set_samplerate(100);
    chain.get_info()->out_buffer = 100;

    // Create the envelope:

    ConstantEnvelope cnst;

    cnst.set_start_value(5);
    cnst.set_stop_value(10);
    cnst.set_stop_time(NANO);

    // Add the envelope to the chain:

    chain.add_envelope(&cnst);

    // Add another envelope:

    ConstantEnvelope cnst2;

    cnst2.set_start_value(20);
    cnst2.set_start_time(NANO * 2);
    cnst2.set_stop_value(30);
    cnst2.set_stop_time(NANO * 3);

    chain.add_envelope(&cnst2);

    // Start the chain:

    chain.start();

    // Get a buffer:

    chain.meta_process();

    auto buff = chain.get_buffer();

    // Ensure buffer is all 5:

    for (auto iter = buff->sbegin();
         static_cast<unsigned int>(iter.get_index()) < buff->size(); ++iter) {

        ASSERT_DOUBLE_EQ(*iter, 5);
    }

    // Grab next buffer:

    chain.meta_process();

    buff = chain.get_buffer();

    // Ensure buffer is all 10:

    for (auto iter = buff->sbegin();
         static_cast<unsigned int>(iter.get_index()) < buff->size(); ++iter) {

        ASSERT_DOUBLE_EQ(*iter, 10);
    }

    // Grab next buffer:

    chain.meta_process();

    buff = chain.get_buffer();

    // Ensure buffer is all 20:

    for (auto iter = buff->sbegin();
         static_cast<unsigned int>(iter.get_index()) < buff->size(); ++iter) {

        ASSERT_DOUBLE_EQ(*iter, 20);
    }

    // Grab next buffer:

    chain.meta_process();

    buff = chain.get_buffer();

    // Ensure buffer is all 30:

    for (auto iter = buff->sbegin();
         static_cast<unsigned int>(iter.get_index()) < buff->size(); ++iter) {

        ASSERT_DOUBLE_EQ(*iter, 30);
    }
}

TEST(ChainEnvelopeTest, MultiEnvelopeTestOneBuffer) {

    // Construct the chain envelope:

    ChainEnvelope chain;

    chain.get_timer()->set_samplerate(100);
    chain.get_info()->out_buffer = 400;

    // Create the envelope:

    ConstantEnvelope cnst;

    cnst.set_start_value(5);
    cnst.set_stop_value(10);
    cnst.set_stop_time(NANO);

    // Add the envelope to the chain:

    chain.add_envelope(&cnst);

    // Add another envelope:

    ConstantEnvelope cnst2;

    cnst2.set_start_value(20);
    cnst2.set_start_time(NANO * 2);
    cnst2.set_stop_value(30);
    cnst2.set_stop_time(NANO * 3);

    chain.add_envelope(&cnst2);

    // Start the chain:

    chain.start();

    // Get a buffer:

    chain.meta_process();

    auto buff = chain.get_buffer();

    // Ensure buffer is all 5:

    for (auto iter = buff->sbegin();
         static_cast<unsigned int>(iter.get_index()) < 100; ++iter) {

        ASSERT_DOUBLE_EQ(*iter, 5);
    }

    // Ensure buffer is all 10:

    for (auto iter = buff->sbegin() + 100;
         static_cast<unsigned int>(iter.get_index()) < 200; ++iter) {

        ASSERT_DOUBLE_EQ(*iter, 10);
    }

    // Ensure buffer is all 20:

    for (auto iter = buff->sbegin() + 200;
         static_cast<unsigned int>(iter.get_index()) < 300; ++iter) {

        ASSERT_DOUBLE_EQ(*iter, 20);
    }

    // Ensure buffer is all 30:

    for (auto iter = buff->sbegin() + 300;
         static_cast<unsigned int>(iter.get_index()) < 400; ++iter) {

        ASSERT_DOUBLE_EQ(*iter, 30);
    }
}

TEST(ChainEnvelopeTest, FullTest) {

    // Create a ChainEnvelope:

    ChainEnvelope chain;

    // Set the configuration values:

    int sample_rate = SAMPLE_RATE;

    int64_t time1_start = 0;
    int64_t time1_stop = 0;

    int64_t time2_start = 0;
    int64_t time2_stop = 0;

    int64_t time3_start = 0;
    int64_t time3_stop = 0;

    long double value1_start = 0;
    long double value1_stop = 0;

    long double value2_start = 0;
    long double value2_stop = 0;

    long double value3_start = 0;
    long double value3_stop = 0;

    int loops = std::ceil(time3_stop /
                          sample_rate);  // By default, we loop until complete

    auto* timer = chain.get_timer();

    timer->set_samplerate(sample_rate);

    ChainTimer ttime;
    ttime.set_samplerate(sample_rate);

    // Add the envelopes:

    ConstantEnvelope env1;

    env1.set_start_time(time1_start);
    env1.set_stop_time(time1_stop);
    env1.set_start_value(value1_start);
    env1.set_stop_value(value1_stop);

    ConstantEnvelope env2;

    env2.set_start_time(time2_start);
    env2.set_stop_time(time2_stop);
    env2.set_start_value(value2_start);
    env2.set_stop_value(value2_stop);

    ConstantEnvelope env3;

    env3.set_start_time(time3_start);
    env3.set_stop_time(time3_stop);
    env3.set_start_value(value3_start);
    env3.set_stop_value(value3_stop);

    // Add each envelope:

    chain.add_envelope(&env1);
    chain.add_envelope(&env2);
    chain.add_envelope(&env3);

    std::vector<ConstantEnvelope*> envs = {&env1, &env2, &env3};
    int current_envelope = 0;

    ConstantEnvelope* cenv = &env1;

    int loop = 0;

    while (loop < loops) {

        // Grab the buffer:

        chain.meta_process();

        auto buff = chain.get_buffer();

        // Iterate over the buffer:

        for (auto iter = buff->ibegin(); iter != buff->iend(); ++iter) {

            // Determine if the time is valid:

            if (ttime.get_time() > cenv->get_stop_time()) {

                // Update to the the next envelope:

                cenv = envs[++current_envelope];
            }

            // Otherwise, ensure current value is accurate:

            ASSERT_EQ(*iter, cenv->get_start_value());

            // Update timer:

            ttime.add_sample(1);
        }

        // Update the loop:

        loop++;
    }
}

TEST(ChainEnvelopeTest, AddAfter) {

    // Create a ChainEnvelope:

    ChainEnvelope chain;

    chain.get_timer()->set_samplerate(100);
    chain.get_info()->out_buffer = 100;

    // Create an envelope:

    ConstantEnvelope cnst;

    cnst.set_stop_time(NANO * 2);
    cnst.set_start_value(1);
    cnst.set_stop_value(5);

    chain.add_envelope(&cnst);

    chain.start();

    // Sample twice:

    for (int i = 0; i < 2; ++i) {

        chain.meta_process();

        auto buff = chain.get_buffer();

        // Ensure values are all 1:

        for (auto iter = buff->ibegin(); iter != buff->iend(); ++iter) {

            ASSERT_EQ(*iter, 1);
        }
    }

    // Sample twice again:

    for (int i = 0; i < 2; ++i) {

        chain.meta_process();

        auto buff = chain.get_buffer();

        // Ensure all values are 5:

        for (auto iter = buff->ibegin(); iter != buff->iend(); ++iter) {

            ASSERT_EQ(*iter, 5);
        }
    }

    // Now, add another envelope:

    ConstantEnvelope cnst2;

    cnst2.set_start_time(chain.get_time());
    cnst2.set_stop_time(chain.get_time() + (2 * NANO));
    cnst2.set_start_value(10);
    cnst2.set_stop_value(20);

    // Add the envelope:

    chain.add_envelope(&cnst2);
    // chain.optimize();
    chain.next_envelope();

    // Sample twice:

    for (int i = 0; i < 2; ++i) {

        chain.meta_process();

        auto buff = chain.get_buffer();

        // Ensure values are all 1:

        for (auto iter = buff->ibegin(); iter != buff->iend(); ++iter) {

            ASSERT_EQ(*iter, 10);
        }
    }

    // Sample twice again:

    for (int i = 0; i < 2; ++i) {

        chain.meta_process();

        auto buff = chain.get_buffer();

        // Ensure all values are 5:

        for (auto iter = buff->ibegin(); iter != buff->iend(); ++iter) {

            ASSERT_EQ(*iter, 20);
        }
    }
}
