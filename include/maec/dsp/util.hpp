/**
 * @file util.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Utilities and components useful for DSP operations
 * @version 0.1
 * @date 2023-06-12
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <algorithm>
#include <chrono>
#include <complex>
#include <iterator>
#include <utility>

#include "dsp/meta.hpp"

namespace dsp::util {

template <typename I>
void bit_reverse(int size, I iter) {

    // Iterate over all components:

    for (int i = 0, j = 1; j < size - 1; j++) {

        for (int k = size >> 1; k > (i ^= k); k >>= 1) {
        }

        if (i < j) {

            // Swap iter[i] <> iter[j]

            std::swap(iter[i], iter[j]);
        }
    }
}

/**
 * @brief Implementation of the sinc function
 *
 * The sinc function (pronounced sink) is defined as:
 *
 * sin(x) / x
 *
 * Which produces a sine wave that decays in amplitude
 * as x becomes bigger.
 * Of course, 0 should never be provided to this function!
 *
 * @param x Current value to compute
 */
double sinc(double x);

template <typename I1, typename O>
void multiply_signals(int size, I1 input1, I1 input2, O output) {

    // Determine the type:

    typedef typename std::iterator_traits<I1>::value_type iter_type;

    // Multiply the two signals together:

    std::transform(input1, input1 + size, input2, output,
                   [](iter_type val1, iter_type val2) {
                       // Multiply the two signals:

                       return val1 * val2;
                   });
}

template <typename I, typename O>
void real_complex_naive(I input, int size, O output) {

    // Determine type:

    typedef typename std::iterator_traits<I>::value_type iter_type;

    // Transform real data into complex data:

    std::transform(input, input + size, output, [](iter_type val) {
        // Return new complex value:

        return std::complex<iter_type>(val, 0);
    });
}

template <typename I, typename O>
void real_eop_complex(I input, int size, O output) {

    // Iterate over real data:

    for (int i = 0; i < size / 2; ++i) {

        // Add data to output:

        *(output + i) =
            std::complex<double>(*(input + (2 * i)), *(input + (2 * i + 1)));
    }
}

/**
 * @brief Computes the sign of a value
 *
 * Given a value, we compute the sign,
 * which is either -1, 0, or 1, depending on if the value is negative, zero, or
 * positive, respectively.
 *
 * @tparam T Numerical type
 * @param val Value to compute the sign of
 * @return int Sign of the value
 */
template <typename T>
int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

/**
 * @brief Converts degrees into radians
 *
 * The degrees should be between 0 and 360,
 * and the result will be from 0 to 2pi radians.
 *
 * @tparam D Degree type
 * @tparam R Radian type
 * @param deg Degree to convert
 * @return R Radian result
 */
template <meta::is_numerical D, meta::is_numerical R = D>
constexpr R deg_radian(D deg) {

    return deg * (std::numbers::pi_v<R> / 180);
}

/**
 * @brief Converts radians to degrees
 *
 * The radians should be between 0 and 2pi,
 * and the result will be from 0 to 360 degrees.
 *
 * @tparam R Radian type
 * @tparam D Degree type
 * @param rad Radian to convert
 * @return D Degree result
 */
template <meta::is_numerical R, meta::is_numerical D = R>
constexpr D radian_deg(R rad) {

    return rad * (180 / std::numbers::pi_v<R>);
}

/**
 * @brief Converts hertz to period in seconds
 *
 * By default, we return the period in seconds, but users can specify any
 * duration type they wish, such as milliseconds or microseconds. The results
 * will be in the numerical type specified by the user, but internally we use
 * double for accuracy.
 *
 * @tparam T Hertz type
 * @tparam D Duration type for result, seconds by default
 * @param hertz Hertz to convert, must be greater than 0
 * @return constexpr T Period in seconds
 */
template <meta::is_numerical T, typename D = std::chrono::seconds>
constexpr T hertz_period(T hertz) {

    const std::chrono::duration<double> secs{1.0 / static_cast<double>(hertz)};
    return static_cast<T>(
        std::chrono::duration_cast<
            std::chrono::duration<double, typename D::period>>(secs)
            .count());
}

/**
 * @brief Unwraps an array of angular values
 *
 * This function is used to unwrap an array of angular values, such as phase
 * data. This is done by ensuring that there are no discontinuities in the data,
 * meaning that the difference between adjacent values is not greater than a
 * specified discontinuity value. If a discontinuity is detected, then the
 * offset value is added or subtracted from the current value until it is within
 * the discontinuity threshold.
 *
 * The discontinuity value should be between 0 and the offset value, and the
 * offset value should be greater than 0. You can think of these values
 * as the difference until action is taken, and the size of the range over which
 * the input wraps.
 *
 * @tparam A Array type to utilize, must have size() and operator[] defined
 * @param arr Array to unwrap, will be modified in place
 * @param discont Discontinuity threshold, must be between 0 and offset
 * @param offset Offset to add or subtract when discontinuity is detected, must
 * be greater than 0
 */
template <typename A>
constexpr void unwrap(A& arr, double discont, double offset) {

    // We will iterate over the array, and add or subtract offset whenever we
    // encounter a discontinuity:

    double prev = arr[0];

    for (std::size_t i = 1; i < arr.size(); ++i) {

        double current = arr[i];
        double diff = current - prev;

        if (std::abs(diff) > discont) {

            // Round to the nearest multiple of offset so the corrected diff
            // lands in (-offset/2, offset/2]:

            int num_offsets =
                static_cast<int>(std::round(std::abs(diff) / offset));

            // Determine the sign of the difference,
            // if it's negative then we need to add the offset, otherwise we
            // need to subtract the offset:

            double sign = sgn(diff);

            // Apply the offset the necessary number of times

            current -= sign * offset * num_offsets;
        }

        arr[i] = current;
        prev = current;
    }
}

}  // namespace dsp::util
