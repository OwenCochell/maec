/**
 * @file const.hpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Various constants in use by MAEC
 * @version 0.1
 * @date 2023-02-25
 * 
 * @copyright Copyright (c) 2023
 * 
 * This file contains constants that are used in various places.
 * 
 */

#pragma once

/// Default sample rate
const int SAMPLE_RATE = 44100;

/// Default buffer size
const int BUFF_SIZE = 440;

/// Very small value (usually used in place of zero in division)
const double SMALL = 0.000001;

/// Nanoseconds per second
const int64_t NANO = 1000000000;
