/**
 * @file base_oscillator.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Implementations for base oscillator classes and tools
 * @version 0.1
 * @date 2022-09-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <algorithm>
#include "base_oscillator.hpp"


void ConstantOscillator::process() {

    // Create a new buffer:

    this->set_buffer(this->create_buffer());

    // Fill the buffer with the value:

    std::fill(this->buff->sbegin(), this->buff->send(), this->value);

}
