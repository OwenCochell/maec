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

float mf_float(long double val) { return static_cast<float>(val); }

long double mf_null(long double val) { return val; }

long double int16_mf(int16_t val) {

    // Divide by largest value:

    return static_cast<long double>(val) / 32767.;
}
