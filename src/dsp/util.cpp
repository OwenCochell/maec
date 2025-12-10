/**
 * @file util.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Implementations of DSP util functions
 * @version 0.1
 * @date 2023-07-27
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "dsp/util.hpp"

#include <cmath>

double sinc(double x) {

    // Calculate and return:

    return std::sin(x) / x;
}
