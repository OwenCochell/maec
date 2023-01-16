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

#include <algorithm>
#include "audio_buffer.hpp"


AudioBuffer::AudioBuffer(int size, int channels) {
     
    // Create the underlying vector:

    this->buff.reserve(channels);

    // Next, create each channel and append it:

    for(int i = 0; i < channels; i++) {

        // Create an empty vector:

        AudioChannel vect(size);

        // Reserve the size:

        vect.reserve(size);

        // Finally, append the vector:

        this->buff.push_back(vect);
    }
}

AudioBuffer::AudioBuffer(AudioChannel vect) {

    // First, reserve the underlying buffer with a size of 1:

    this->buff.reserve(1);

    // Next, append the given vector to the buffer:

    this->buff.push_back(vect);
}

template <typename It, typename Func>
void squish_seq(AudioBuffer* buff, It iter, Func op) {

    // Copy the content over:

    std::transform(buff->ibegin(), buff->send(), iter, op);

}

template<typename It, typename Func>
void squish_null(AudioBuffer* buff, const It& iter, Func op) {}

float mf_float(long double val) { return float(val); }

long double mf_null(long double val) { return val; }
