/**
 * @file conv.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Implementations for convolution operations
 * @version 0.1
 * @date 2023-04-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "dsp/conv.hpp"

int length_conv(int size1, int size2) {

    // Calculate and return

    return size1 + size2 - 1;

}

BufferPointer input_conv(BufferPointer input, BufferPointer kernel) {

    // First, create an output buffer:

    BufferPointer buff = std::make_unique<AudioBuffer>(length_conv(static_cast<int>(input->size()), static_cast<int>(kernel->size())));

    // Pass the data along to the other function:

    input_conv(input->ibegin(), input->size(), kernel->ibegin(), kernel->size(), buff->ibegin());

    // Finally, return the output buffer:

    return buff;

}
