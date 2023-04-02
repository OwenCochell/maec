/**
 * @file conv_test.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Tests for Convolution operations
 * @version 0.1
 * @date 2023-04-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <gtest/gtest.h>

#include "dsp/conv.hpp"

// Sample data to test convolution

std::vector<long double> input = {1, -2, 4.3, 5.62, -6.74, 0};
std::vector<long double> kernel = {0.5, 2, 0.3};
std::vector<long double> output = {0.5, 1, -1.5500000000000003, 10.81, 9.16, -11.794, -2.022, 0 };

/**
 * @brief Ensures the output size is accurate for various inputs
 * 
 */
TEST(ConvolutionTest, Length) {

    // Size with smallest first:

    ASSERT_EQ(length_conv(3, 4), 6);

    // Size with largest first:

    ASSERT_EQ(length_conv(8, 2), 9);

}

/**
 * @brief Ensures the input side convolution algorithm works with iterators and sizes works
 * 
 */
TEST(ConvolutionTest, InputIterSize) {

    // Create buffer of results:

    AudioBuffer buff(length_conv(input.size(), kernel.size()));

    // Do operation:

    input_conv(input.begin(), input.size(), kernel.begin(), kernel.size(), buff.ibegin());

    // Check that output matches expected:

    for (auto iter = buff.ibegin(); iter != buff.iend(); ++iter) {

        ASSERT_DOUBLE_EQ(*iter, output.at(iter.get_index()));
    }
}

/**
 * @brief Ensures the input side convolution algorithm works with iterators
 * 
 */
TEST(ConvolutionTest, InputIter) {

    // Create buffer of results:

    AudioBuffer buff(length_conv(input.size(), kernel.size()));

    // Do operation:

    input_conv(input.begin(), input.end(), kernel.begin(), kernel.end(), buff.ibegin());

    // Check that output matches expected:

    for (auto iter = buff.ibegin(); iter != buff.iend(); ++iter) {

        ASSERT_DOUBLE_EQ(*iter, output.at(iter.get_index()));
    }

}

/**
 * @brief Ensures the input side convolution algorithm works with AudioBuffers
 * 
 */
TEST(ConvolutionTest, InputBuffer) {

    // Create buffers:

    BufferPointer binput = std::make_unique<AudioBuffer>(input);
    BufferPointer bkernel = std::make_unique<AudioBuffer>(kernel);

    // Do operation:

    auto buff = input_conv(std::move(binput), std::move(bkernel));

    // Check that output matches expected:

    for (auto iter = buff->ibegin(); iter != buff->iend(); ++iter) {

        ASSERT_DOUBLE_EQ(*iter, output.at(iter.get_index()));
    }

}
