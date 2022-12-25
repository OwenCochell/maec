/**
 * @file sink_module.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Basic tests for sink modules
 * @version 0.1
 * @date 2022-12-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <gtest/gtest.h>
#include "sink_module.hpp"
#include "base_oscillator.hpp"

TEST(SinkModuleTest, Construct) {

    SinkModule sink;
}

TEST(SinkModuleTest, Period) {

    // Create a sink module:

    SinkModule sink;

    // Ensure that the default value is correct:

    ASSERT_EQ(sink.get_period(), 1);

    // Change the value:

    sink.set_period(5);

    // Check this value is now correct:

    ASSERT_EQ(sink.get_period(), 5);
}

TEST(SinkModuleTest, MetaProcess) {

    // Create a sink module:

    SinkModule sink;

    // Create a ConstantOscillator:

    ConstantOscillator oconst(5);

    // Bind the two modules together:

    sink.bind(&oconst);

    // Meta process the sink:

    sink.meta_process();

    // Get the buffer:

    std::unique_ptr<AudioBuffer> buff = sink.get_buffer();

    // Get the audio info:

    std::shared_ptr<AudioInfo> info = oconst.get_info();

    // Ensure the size is correct:

    ASSERT_EQ(buff->size(), info->buff_size * sink.get_period());

    // Now, test with different period:

    int period = 5;
    sink.set_period(period);

    // Again, meta process the sink:

    sink.meta_process();

    // Get the buffer:

    buff = sink.get_buffer();

    // Finally, ensure size is correct:

    ASSERT_EQ(buff->size(), info->buff_size * sink.get_period());

}
