/**
 * @file kernel.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Tools for generating and manipulating kernels
 * @version 0.1
 * @date 2023-07-26
 * 
 * @copyright Copyright (c) 2023
 * 
 * A 'kernel' is an impulse response that is being used
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

#include <algorithm>

#include "dsp/window.hpp"
#include "dsp/util.hpp"

using window_functiont = long double(*)(int, int);

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
 * This means that the original frequency components are subtracted from the signal,
 * thus resulting in output that is the negation of the original filter kernel.
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
template<typename I>
void spectral_inversion(I input, int size) {

    // First, negate each sample:

    for (int i = 0; i < size; ++i) {

        // Negate this sample:

        *(input+i) *= -1;
    }

    // Add one to the center sample:

    *(input + ((size/ 2) + 1)) += 1;
}

template <typename I>
void spectral_reversal(I input, int size) {

    // Iterate over every other sample:

    for (int i = 0; i < size; i += 2) {

        // Negate this sample:

        *(input + i) *= -1;
    }
}

template <typename O>
void sinc_kernel(double freq, int size, O output, window_functiont window = window_blackman) {

    // Determine some constants:

    const int size2 = (size - 1) / 2;
    const long double inner = 2 * M_PI * freq;

    long double sum = 1.;

    // Iterate over first half:

    for (int i = 0; i < size2; ++i) {

        // Determine some constants:

        const int half = i - size2;

        // Calculate value:

        long double value = sinc(inner * half) * window(i, size);

        // Set value on each half:

        *(output + i) = value;
        *(output + (size - i - 1)) = value;

        // Keep a running sum of each value:

        sum += value * 2;
    }

    // Calculate center value:

    *(output + size2) = 1;

    // Normalize all values:

    for (int i = 0; i < size; ++i) {

        // Divide by total sum:

        *(output + i) /= sum;
    }
}
