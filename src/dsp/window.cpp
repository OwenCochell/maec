/**
 * @file window.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Implementations of window functions
 * @version 0.1
 * @date 2023-07-28
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "dsp/window.hpp"

#include <cmath>

long double window_rectangle(int num, int size) {

    // Just return 1!

    return 1.;
}

long double window_hann(int num, int size, double a0) {

    // Calculate and return:

    return a0 - (1 - a0) * std::cos(2 * M_PI * num / (size - 1));
}

long double window_hamming(int num, int size, double a0) {

    // Just run through Hann window with new a0 value:

    return window_hann(num, size, a0);
}

long double window_blackmanc(int num, int size, double alpha) {

    // Calculate and return:

    return ((1 - alpha) / 2) - 0.5 * std::cos(2 * M_PI * num / (size - 1)) +
           (alpha / 2) * std::cos(4 * M_PI * num / (size - 1));
}

long double window_blackman(int num, int size) {

    // Just call function and return:

    return window_blackmanc(num, size, 0.16);
}
