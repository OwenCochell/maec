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

#include <memory>

std::unique_ptr<AudioBuffer> create_buffer(int size, int channels) {

    // Allocate the new buffer:

    return std::make_unique<AudioBuffer>(size, channels);
}

float mf_float(long double val) { return static_cast<float>(val); }

long double mf_null(long double val) { return val; }

int16_t mf_int16(long double val) {

    // Multiply and return:

    return static_cast<int16_t>(val * 32767);
}

unsigned char mf_uchar(long double val) {

    // Multiply and return:

    return static_cast<unsigned char>(val * 255);
}

long double int16_mf(int16_t val) {

    // Divide by largest value:

    return static_cast<long double>(val) / 32767.;
}

long double uchar_mf(unsigned char val) {

    // Divide by largest value:

    return static_cast<long double>(val) / 255;
}
