/**
 * @file module_param.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Implementations for module parameters
 * @version 0.1
 * @date 2023-02-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "module_param.hpp"

#include <memory>

#include "audio_buffer.hpp"
#include "meta_audio.hpp"

BufferPointer ModuleParameter::get() {

    // First, meta process:

    this->meta_process();

    // Next, return buffer:

    return this->get_buffer();
}

void ModuleParameter::set_constant(long double val) {

    // Set the underlying value:

    this->value = val;

    // Set the constant module:

    this->const_mod = std::make_unique<ConstModule>(val);

    this->bind(this->const_mod.get());
}
