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

/// Default sample rate
const int SAMPLE_RATE = 44100;

/// Filter types:
enum class FilterType { LowPass, HighPass, BandPass, BandReject };
