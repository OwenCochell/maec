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

#include "dsp/ft.hpp"

#include <cmath>
#include <numbers>

double dsp::ft::index_freq(std::size_t index, std::size_t size, double sample_rate) {

    return static_cast<double>(index) *
           (sample_rate / static_cast<double>(size));
}

double dsp::ft::cos_basis(int phase, int total, double freq) {

    // Preform operation and return:

    return cos((2 * std::numbers::pi_v<double> * freq * phase) / total);
}

double dsp::ft::sin_basis(int phase, int total, double freq) {

    // Preform operation and return:

    return sin((2 * std::numbers::pi_v<double> * freq * phase) / total);
}

std::size_t dsp::ft::length_ft(std::size_t size) {

    // Simply calculate and return:

    return (size / 2) + 1;
}

std::size_t dsp::ft::length_ift(std::size_t size) {

    // Simply calculate and return:

    return (size - 1) * 2;
}
