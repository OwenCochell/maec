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

#include <cmath>
#include <cstdint>
#include <memory>

std::unique_ptr<AudioBuffer> create_buffer(int size, int channels) {

    // Allocate the new buffer:

    return std::make_unique<AudioBuffer>(size, channels);
}

std::unique_ptr<AudioBuffer> create_buffer(int size, int channels, int sample_rate) {

    // Allocate the new buffer:

    return std::make_unique<AudioBuffer>(size, channels, sample_rate);
}

float mf_float(long double val) { return static_cast<float>(val); }

long double mf_null(long double val) { return val; }

int16_t mf_int16(long double val) {

    if (val < 0) {

        return static_cast<int16_t>(val * 32768);
    }

    // Multiply and return:

    return static_cast<int16_t>(val * 32767);
}

uint16_t mf_uint16(long double val) {

    // Add, multiply, and return:

    return static_cast<uint16_t>(std::round(((val + 1) / 2) * 65535));
}

char mf_char(long double val) {

    if (val < 0) {

        // return special value:

        return static_cast<char>(val * 128.);
    }

    // Multiply and return:

    return static_cast<char>(val * 127.);
}

unsigned char mf_uchar(long double val) {

    // Add, divide, multiply:

    return static_cast<unsigned char>(std::round(((val + 1.) / 2.) * 255.));
}

long double int16_mf(int16_t val) {

    // Divide by largest value:

    return static_cast<long double>(val) / 32767.;
}

long double uint16_mf(uint16_t val) {

    // Normalize, double, subtract:

    return (static_cast<long double>(val) / 65535.) * 2. - 1.;
}

long double char_mf(char val) {

    if (val < 0) {

        // Divide by largest value:

        return static_cast<long double>(val) / 128.;
    }

    // Divide by largest value:

    return static_cast<long double>(val) / 127.;
}

long double uchar_mf(unsigned char val) {

    // Normalize, double, subtract:

    return (static_cast<long double>(val) / 255.) * 2. - 1.;
}
