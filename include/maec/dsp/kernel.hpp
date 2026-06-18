/**
 * @file kernel.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Tools for generating and manipulating kernels
 * @version 0.1
 * @date 2023-07-26
 *
 * @copyright Copyright (c) 2023
 *
 * A 'kernel' is a finite impulse response that is being used
 * to filter a signal via convolution.
 * Filtering a signal is to preform signal separation
 * or signal restoration.
 *
 * Signal separation is the process of separating (or removing)
 * certain components to achieve an outcome.
 * Signal restoration is the process of restoring components
 * that may have been distorted or corrupted.
 *
 * This file contains tools and components for generating
 * and manipulating these filter kernels,
 * allowing you to use them in your operations.
 */

#pragma once

#include <cmath>
#include <complex>
#include <numbers>

#include "dsp/util.hpp"
#include "dsp/window.hpp"

namespace dsp::kern {

using window_functiont = double (*)(int, int);

/**
 * @brief Preforms spectral inversion on a filter kernel.
 *
 * Spectral inversion flips the frequency response of
 * the given kernel top-for-bottom.
 * This has the process of inverting a filter to preform
 * the opposite operation:
 *
 * High Pass <-> Low Pass
 * Band Reject <-> Band Pass
 *
 * From a purely mathematical perspective, we are applying
 * the filter to two systems in parallel, negating the output of one,
 * and summing their results.
 * One system applies the input signal to the original filter,
 * while the other does nothing.
 * Finally, the output is defined as the output of the all pass system
 * minus the output of the original filter system.
 * This means that the original frequency components are subtracted from the
 * signal, thus resulting in output that is the negation of the original filter
 * kernel.
 *
 * This operation can be simplified by combining the two signal filters in
 * our theoretical system.
 * This leaves us with two steps:
 *
 * 1. Negate the incoming filter
 * 2. Add 1 to the sample at the center of symmetry.
 *
 * This process does have some caveats.
 * The incoming filter MUST have left-right symmetry,
 * and the filter kernel MUST be of an odd length.
 *
 * This function works in place!
 * Simply provide your filter kernel and it will be inverted.
 *
 * @tparam T Filter iterator type
 * @param input Iterator to filter data
 * @param size Size of filter kernel
 */
template <typename I>
void spectral_inversion(I input, int size) {

    // First, negate each sample:

    for (int i = 0; i < size; ++i) {

        // Negate this sample:

        *(input + i) *= -1;
    }

    // Add one to the center sample:

    *(input + (size / 2)) += 1;
}

template <typename I>
void spectral_reversal(I input, int size) {

    // Iterate over every other sample:

    for (int i = 0; i < size; i += 2) {

        // Negate this sample:

        *(input + i) *= -1;
    }
}

/**
 * @brief Computes the group delay of a FIR filter at a radian
 *
 * Group delay is a measurement of how much a filter delays a specific
 * frequency.
 * This can be used to determine if a filter is linear (constant group delay)
 * or has phase distortion (varying group delay).
 *
 * @tparam T Kernel type (must support .size() and operator[])
 * @param freq Radian to compute in radians per sample
 * @param kernel Filter kernel to compute group delay of
 * @return constexpr double Group delay at the given frequency in samples
 */
template <typename T>
constexpr double group_delay(double rad, const T& kernel) {

    // Final sums to be used in the division

    double denom = 0;
    double num = 0;

    for (std::size_t i = 0; i < kernel.size(); ++i) {

        // We need to compute the complex exponential of -i

        auto z_val = std::exp(std::complex<double>(0, -rad * i));

        denom += kernel[i] * z_val;
        num += i * kernel[i] * z_val;
    }

    // Final result is the real part of fraction

    return std::real(num / denom);
}

/**
 * @brief Creates a delay kernel
 *
 * A delay kernel will delay a signal by a specified number of samples.
 * This is done by creating a kernel that has a value of 1 at the index of the
 * delay, and 0 before it.
 *
 * @tparam O Output iterator type
 * @param delay Number of samples to delay, must be greater than or equal to 0
 * @param output Iterator to output kernel data, must have at least delay + 1
 * space available
 */
template <typename O>
void delay_kernel(std::size_t delay, O output) {

    // Add zeros for the delay,
    // it should ne delay count of zeros followed by 1:

    for (std::size_t i = 0; i < delay; ++i) {

        *(output + i) = 0;
    }

    *(output + delay) = 1;
}

template <typename O>
void sinc_kernel(double freq, int size, O output,
                 window_functiont window = win::window_blackman) {

    // Determine some constants:

    const int size2 = (size - 1) / 2;
    const double inner = 2 * std::numbers::pi_v<double> * freq;

    double sum = 1.;

    // Iterate over first half:

    for (int i = 0; i < size2; ++i) {

        // Determine some constants:

        const int half = i - size2;

        // Calculate value:

        const double value = util::sinc(inner * half) * window(i, size);

        // Set value on each half:

        *(output + i) = value;
        *(output + (size - i - 1)) = value;

        // Keep a running sum of each value:

        sum += value * 2;
    }

    // Calculate center value:

    *(output + size2) = 1. * window(size2, size);

    // Normalize all values:

    for (int i = 0; i < size; ++i) {

        // Divide by total sum:

        *(output + i) /= sum;
    }
}

}  // namespace dsp::kern
