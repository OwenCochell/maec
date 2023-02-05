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
#include <vector>
#include <memory>

#include "module_param.hpp"
#include "base_oscillator.hpp"
#include "fund_oscillator.hpp"
#include "source_module.hpp"

// Dummy data for testing:

std::vector<long double> data = {1,2,3,4,5,6,7,8,9,10};

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

TEST(ParamTest, Module) {

    // Create a module to use:

    ConstantOscillator osc(3.0);

    // Create a parameter:

    ModuleParameter param(&osc);

    // Ensure the value is accurate:

    ASSERT_DOUBLE_EQ(param.get(), 3.0);

    // Do something a bit crazier:

    SineOscillator sine(440.0);

    // Create a dummy module:

    SourceModule src;

    // Bind dummy data buffer:

    std::unique_ptr<AudioBuffer> buff = std::make_unique<AudioBuffer>(data);

    src.set_buffer(std::move(buff));

    // Set the source module

    param.set_module(&src);

    // Now, ensure values are accurate:
    // TODO: Use better module then constant oscillator...

    for(int i = 0; i < 10 * 4; ++i) {

        ASSERT_DOUBLE_EQ(param.get(), data.at(i % 10));
    }
}
