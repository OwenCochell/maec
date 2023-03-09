/**
 * @file meta_module_test.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Tests for various meta modules
 * @version 0.1
 * @date 2023-01-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <gtest/gtest.h>
#include "meta_audio.hpp"
#include "base_oscillator.hpp"
#include "fund_oscillator.hpp"

TEST(MetaCountTest, Construct) {

    Counter count;
}

TEST(MetaCountTest, Count) {

    // Create counter:

    Counter count;

    // Create constant oscillator:

    ConstantOscillator con(5.0);

    // Bind the constant oscillator:

    count.bind(&con);

    // Process a few times:

    count.meta_process();
    count.meta_process();
    count.meta_process();

    // Ensure numbers are right:

    ASSERT_EQ(3, count.processed());
    ASSERT_EQ(3 * con.get_info()->buff_size, count.samples());
}

TEST(MetaCountTest, Reset) {

    // Create counter:

    Counter count;

    // Create constant oscillator:

    ConstantOscillator con(5.0);

    // Bind the constant oscillator:

    count.bind(&con);

    // Process a few times:

    count.meta_process();
    count.meta_process();
    count.meta_process();

    // Reset:

    count.reset();

    // Ensure values are zero:

    ASSERT_EQ(0, count.processed());
    ASSERT_EQ(0, count.samples());
}

TEST(MetaLatencyTest, Construct) {

    // Create timer:

    LatencyModule late;
}

TEST(MetaLatencyTest, MetaProcess) {

    // Create timer:

    LatencyModule late;

    // Attach module:

    ConstantOscillator osc(0.5);

    // Bind the modules:

    late.bind(&osc);

    // Meta process:

    late.meta_process();

}

TEST(MetaLatencyTest, TotalTime) {

    // Create timer:

    LatencyModule late;

    // Attach module:

    SineOscillator osc(440.0);

    // Bind the modules:

    late.bind(&osc);

    // Get expected time:

    ASSERT_EQ(late.expected(), 0);

    // Start the module:

    late.start();

    // Meta process the module:

    late.meta_process();

    // Ensure start time is not zero:

    ASSERT_GT(late.get_start_time(), 0);

    // Get total time elapsed:

    ASSERT_NEAR(late.elapsed(), get_time() - late.get_start_time(), 1000);

    // Get expected time:

    ASSERT_EQ(late.expected(), 440 * (nano / 44100));

}

TEST(MetaLatencyTest, OperationTime) {

    // Create timer:

    LatencyModule late;

    // Attach module:

    ConstantOscillator osc;

    // Bind the modules:

    late.bind(&osc);

    // Ensure initial values are zero:

    ASSERT_EQ(late.time(), 0);
    ASSERT_EQ(late.total_time(), 0);
    //ASSERT_EQ(late.average_time(), 0);

    // Meta process:

    late.start();
    late.meta_process();

    // Get the elapsed time:

    int64_t total = late.time();

    ASSERT_LT(late.time(), late.elapsed());
    ASSERT_EQ(late.time(), late.total_time());
    ASSERT_EQ(late.time(), late.average_time());

    // Meta process once more:

    late.meta_process();

    total += late.time();

    ASSERT_LT(late.time(), late.elapsed());
    ASSERT_EQ(late.total_time(), total);
    ASSERT_EQ(total / 2, late.average_time());

}

TEST(MetaLatencyTest, LatencyTime) {

    // Create timer:

    LatencyModule late;

    // Attach module:

    SineOscillator osc;

    // Bind the modules:

    late.bind(&osc);

    // Ensure initial values are zero:

    ASSERT_EQ(late.latency(), 0);
    ASSERT_EQ(late.total_latency(), 0);
    //ASSERT_EQ(late.average_latency(), 0);

    // Meta process:

    late.start();
    late.meta_process();

    // Get the elapsed time:

    int total = late.latency();

    ASSERT_LT(late.latency(), late.time());
    ASSERT_EQ(late.latency(), late.total_latency());
    ASSERT_EQ(late.latency(), late.average_latency());

    // Meta process once more:

    late.meta_process();

    total += late.latency();

    ASSERT_LT(late.latency(), late.time());
    ASSERT_EQ(late.total_latency(), total);
    ASSERT_EQ(total / 2, late.average_latency());

}

TEST(MetaLatencyTest, Reset) {

    // Create timer:

    LatencyModule late;

    // Attach module:

    ConstantOscillator osc(0.5);

    // Bind the modules:

    late.bind(&osc);

    // Meta process:

    late.meta_process();

    // Reset the module:

    late.reset();

    // Ensure values are zero:

    ASSERT_EQ(late.time(), 0);
    ASSERT_EQ(late.total_time(), 0);
    //ASSERT_EQ(late.average_time(), 0);

    ASSERT_EQ(late.latency(), 0);
    ASSERT_EQ(late.total_latency(), 0);
    //ASSERT_EQ(late.average_latency(), 0);

}

TEST(MetaBufferTest, Construct) {

    BufferModule buff;
}

TEST(MetaBufferTest, GetSet) {

    // Create a BufferModule:

    BufferModule buff;

    // Create a dummy buffer:

    std::vector<long double> data = {1,2,3,4,5,6,7,8,9,10};

    AudioBuffer rep(data);

    // Attach the dummy data:

    buff.set_rbuffer(&rep);

    // Get dummy data:

    ASSERT_EQ(&rep, buff.get_rbuffer());
}

TEST(MetaBufferTest, Process) {

    // Create a dummy buffer:

    std::vector<long double> data = {1,2,3,4,5,6,7,8,9,10};

    AudioBuffer rep(data);

    // Create a BufferModule:

    BufferModule buff(&rep);

    // Next, ensure data is accurate:

    std::shared_ptr<AudioBuffer> tdata = nullptr;

    for (int i = 0; i < 10*4; ++i) {

        if (i % 10 == 0) {
            // Process:

            buff.meta_process();

            tdata = buff.get_buffer();
        }

        ASSERT_DOUBLE_EQ(data.at(i % 10), *(tdata->ibegin() + (i % 10)));
    }
}