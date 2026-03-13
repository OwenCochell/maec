/**
 * @file const.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Various DSP constants in use by MAEC
 * @version 0.1
 * @date 2023-08-28
 *
 * @copyright Copyright (c) 2023
 *
 * This file contains DSP constants that are used in various places.
 */

#pragma once

#include <cstdint>
#include <numbers>

namespace dsp::consts {

/// Default sample rate
const int SAMPLE_RATE = 44100;

/// PI constant multiplied by 2
const double pi_2 = std::numbers::pi_v<double> * 2;

/// Filter types:
enum class FilterType : uint8_t { LowPass, HighPass, BandPass, BandReject };

}  // namespace dsp::consts
