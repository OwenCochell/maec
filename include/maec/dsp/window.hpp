/**
 * @file window.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Various DSP window functions
 * @version 0.1
 * @date 2023-07-28
 *
 * @copyright Copyright (c) 2023
 *
 * In DSP a window function is a function that is zero-valued
 * outside of a choosen interval.
 * Mathematically, these functions are designed
 * to remove all components outside of an interval,
 * and the contents that overlap are tapered to the
 * contents of the function.
 * When multiplying signals to these functions,
 * the result is the tapered overlapping content,
 * thus allowing you to 'view through the window!'
 *
 * Practically, window functions are just used for tapering,
 * and are not often used for selecting values in an interval.
 *
 * We provide some useful window functions that can compute
 * values at any given point!
 * Sometimes, we offer window functions that expose parameters
 * that can alter how these windows are generated.
 * However, this is incompatible with maec kernel functions,
 * which expect only two int parameters, the current value and total size.
 * We provide two implementations of these functions,
 * allowing you to use the defaults easily with maec kernels,
 * and configure the windows to your needs.
 *
 */

#pragma once

/**
 * @brief Generates a rectangular window
 *
 * This function always returns 1!
 * This can be useful if you want to generate
 * filter kernels without a window being applied.
 *
 * We ignore the input parameters.
 * We just return 1.
 *
 * @param num Current value to compute
 * @param size Size of window
 * @return double 1, will always be 1
 */
double window_rectangle(int num, int size);

/**
 * @brief Generates a Hann window
 *
 * This function generates a Hann window,
 * which is described by the following equation:
 *
 * w(n) = a0 - (1 - a0) * cos(2 * PI * num / size)
 *
 * A Hann window has a0 set to 0.5,
 * but this is configurable.
 * This produces a taper formed by using a raised cosine or
 * sine-squared with ends that touch zero.
 *
 * @param num Current value to compute
 * @param size Size of the window
 * @param a0 a0 value to be used in above equation
 * @return double Computed value
 */
double window_hann(int num, int size, double a0 = 0.5);

/**
 * @brief Generates a hamming window
 *
 * This function generates a Hamming window,
 * which is described by the following equation:
 *
 * w(n) = a0 - (1 - a0) * cos(2 * PI * num / size)
 *
 * A Hamming window has a0 set to 0.54,
 * but this is configurable.
 * This produces a taper formed by using a raised cosine with
 * non-zero endpoints, optimized to minimize the nearest side lobe.
 *
 * @param num Current value to compute
 * @param size Size of the window
 * @param a0 a0 value to be used in above equation
 * @return double Computed value
 */
double window_hamming(int num, int size, double a0 = 0.54);

/**
 * @brief Generates a blackman window, with configurable alpha value
 *
 * This function generates a Blackman window,
 * which is described bu the following equation:
 *
 * w(n) = a0 - a1 * cos(2 * PI * num / size) + a2 * cos(4 * PI * num / size)
 *
 * Where:
 *
 * a0 = (1 - alpha) / 2
 * a1 = 1 / 2
 * a2 = alpha / 2
 *
 * A blackman has an alpha value of 0.16,
 * but this is configurable.
 * This produces a taper formed by using the first three
 * terms of a summation of cosines
 *
 * @param num Current value to calculate
 * @param size Size of the window
 * @param alpha alpha value to use in above equation
 * @return double Computed value
 */
double window_blackmanc(int num, int size, double alpha = 0.16);

/**
 * @brief Generates a blackman window with a pre-configured alpha value
 *
 * We are identical to the configurable blackman window function,
 * with the exception that we do not offer the option to
 * configure the alpha value.
 * This is necessary for compatibility for maec filter kernel generation.
 *
 * @param num Current value to calculate
 * @param size Size of the window
 * @return double Computed value
 */
double window_blackman(int num, int size);
