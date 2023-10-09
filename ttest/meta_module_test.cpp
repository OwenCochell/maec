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

#include <catch2/catch_test_macros.hpp>

#include "meta_audio.hpp"
#include "base_oscillator.hpp"
#include "fund_oscillator.hpp"

TEST_CASE("MetaCount Test", "[meta][count]") {

    // Construct a counter:

    Counter count;

    // Create constant oscillator:

    ConstantOscillator con(5.0);

    // Bind the constant oscillator:

    count.bind(&con);

    SECTION("Count processing") {

        // Process a few times:

        count.meta_process();
        count.meta_process();
        count.meta_process();

        // Ensure numbers are right:

        REQUIRE(3 == count.processed());
        REQUIRE(3 * con.get_info()->out_buffer == count.samples());
    }

    SECTION("Reset a counter")
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

    ASSERT_EQ(late.expected(), 440 * (NANO / 44100));
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
    ASSERT_EQ(late.sum_latency(), 0);
    //ASSERT_EQ(late.average_latency(), 0);
    ASSERT_EQ(late.total_latency(), 0);

    // Meta process:

    late.start();
    late.meta_process();

    // Get the elapsed time:

    int64_t total = late.latency();

    ASSERT_LT(late.latency(), late.time());
    ASSERT_EQ(late.latency(), late.sum_latency());
    ASSERT_EQ(late.latency(), late.average_latency());
    ASSERT_EQ(late.total_latency(), late.total_time() - late.expected());

    // Meta process once more:

    late.meta_process();

    total += late.latency();

    ASSERT_LT(late.latency(), late.time());
    ASSERT_EQ(late.sum_latency(), total);
    ASSERT_EQ(total / 2, late.average_latency());
    ASSERT_EQ(late.total_latency(), late.total_time() - late.expected());
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
    ASSERT_EQ(late.sum_latency(), 0);
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

/**
 * @brief Ensures the UniformBuffer module constructs properly
 * 
 */
TEST(UniformBufferTest, Construct) {

    UniformBuffer uni;

}

/**
 * @brief Ensures UniformBuffer module handles equal input and output sizes
 * 
 */
TEST(UniformBufferTest, SameSize) {

    // Create modules:

    UniformBuffer uni;

    BufferModule mbuf;

    // Configure modules:

    int size = 247;

    mbuf.get_info()->out_buffer = size;
    uni.get_info()->out_buffer = size;

    // Bind the modules:

    uni.bind(&mbuf);

    // Create buffer:

    AudioBuffer buff(size);
    auto iter = buff.ibegin();

    for (int i = 0; i < size; ++i) {

        *iter++ = i;
    }

    // Set buffer:

    mbuf.set_rbuffer(&buff);

    // Finally, meta process:

    for (int i = 0; i < 4; ++i) {

        // Meta process:

        uni.meta_process();

        // Grab the buffer:

        auto buff = uni.get_buffer();

        // Ensure buffer is valid:

        iter = buff->ibegin();

        for (int j = 0; j < size; ++j) {

            ASSERT_DOUBLE_EQ(*iter++, j);
        }
    }

}

/**
 * @brief Ensures UniformBuffer handles smaller input buffer
 * 
 */
TEST(UniformBufferTest, SamllerSize) {

    // Create modules:

    UniformBuffer uni;

    BufferModule mbuff;

    // Configure modules:

    int osize = 100;
    int isize = 20;

    mbuff.get_info()->out_buffer = isize;
    uni.get_info()->out_buffer = osize;

    // Bind the modules:

    uni.bind(&mbuff);

    // Loop a certain number of times:

    long double last = 0;

    // Create buffer:

    AudioBuffer buff(isize);
    auto iter = buff.ibegin();

    for (int i = 0; i < isize; ++i) {

        *iter++ = i;
    }

    // Set buffer:

    mbuff.set_rbuffer(&buff);

    // Meta process:

    uni.meta_process();

    // Grab buffer:

    auto obuff = uni.get_buffer();

    // Ensure buffer is accurate:

    for (auto iter = obuff->ibegin(); iter != obuff->iend(); ++iter) {

        ASSERT_DOUBLE_EQ(*iter, iter.get_index() % isize);
    }

}

/**
 * @brief Ensures UniformBuffer handles larger input buffer
 * 
 */
TEST(UniformBufferTest, BiggerSize) {

    // Create the modules:

    UniformBuffer uni;
    BufferModule mbuff;

    // Configure the module:

    int isize = 100;
    int osize = 20;

    uni.get_info()->out_buffer = osize;
    mbuff.get_info()->out_buffer = isize;

    // Bind the modules:

    uni.bind(&mbuff);

    // Create a buffer:

    AudioBuffer buff(isize);
    auto iter = buff.ibegin();

    // Add values to buffer:

    for (int j = 0; j < isize; ++j) {

        *iter++ = j;
    }

    // Add buffer to module:

    mbuff.set_rbuffer(&buff);

    // Iterate a certain number of time:

    for (int i = 0; i < (isize / 20); ++i) {

        // Meta process:

        uni.meta_process();

        // Grab the buffer:

        auto obuff = uni.get_buffer();

        // Ensure buffer is accurate:

        for (auto tier = obuff->ibegin(); tier != obuff->iend(); ++tier) {

            // Check value:

            ASSERT_DOUBLE_EQ(*tier, tier.get_index() + (i * osize));

        }

    }

}
