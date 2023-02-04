/**
 * @file parameter_test.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Tests for ModuleParameters
 * @version 0.1
 * @date 2023-02-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <gtest/gtest.h>

#include "module_param.hpp"

TEST(ParamTest, Construct) {

    ModuleParameter param;
}

TEST(ParamTest, Constant) {

    // Create the parameter:

    ModuleParameter param(5.0);

    // Ensure we can get a value:

    ASSERT_DOUBLE_EQ(5.0, param.get());

    // Now, change the value:

    param.set_constant(3.0);

    // Test the values multiple times:

    for(int i = 0; i < 20; ++i) {

        ASSERT_DOUBLE_EQ(3.0, param.get());
    }

}
