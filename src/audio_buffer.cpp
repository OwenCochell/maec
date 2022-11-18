/**
 * @file audio_buffer.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Implementations for AudioBuffers
 * @version 0.1
 * @date 2022-11-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "audio_buffer.hpp"

AudioBuffer::AudioBuffer(int size) {

    // Pre-allocate the buffer to a given size:

    this->reserve(size);
}