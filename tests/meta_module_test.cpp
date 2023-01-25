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
