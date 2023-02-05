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
