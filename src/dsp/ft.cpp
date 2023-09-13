/**
 * @file ft.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Implementations of FT algorithms
 * @version 0.1
 * @date 2023-04-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#define _USE_MATH_DEFINES

#include <cmath>

#include "dsp/ft.hpp"

long double cos_basis(int phase, int total, long double freq) {

    // Preform operation and return:

    return cos((2 * M_PI * freq * phase) / total);

}

long double sin_basis(int phase, int total, long double freq) {

    // Preform operation and return:

    return sin((2 * M_PI * freq * phase) / total);
}

int length_ft(std::size_t size) {

    // Simply calculate and return:

    return (static_cast<int>(size) / 2) + 1;
}

int length_ift(std::size_t size) {

    // Simply calculate and return:

    return (static_cast<int>(size) - 1) * 2;
}
