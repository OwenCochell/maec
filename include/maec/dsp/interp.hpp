/**
 * @file interp.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Interpolation components
 * @version 0.1
 * @date 2026-05-26
 *
 * @copyright Copyright (c) 2026
 *
 * 'Interpolation' is the process of estimating values between known data
 * points. We offer a few methods here, each with their own advantages and
 * disadvantages.
 */

#pragma once

#include <cmath>
#include <cstdint>

namespace dsp::interp {

// clang-format off
// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers): Many interpolation constants
// clang-format on

/**
 * @brief Preforms rounding interpolation between two values
 *
 * We simply round the fraction to determine which value to choose.
 * This is a very simple method, but it leads to discontinuities
 * which can introduce some very serious distortion.
 *
 * @tparam T Numerical type
 * @param start Starting value
 * @param end Stopping value
 * @param frac Fraction between start and end to interpolate at, from 0 to 1
 * @return T Interpolated value between start and end at the given fraction
 */
template <typename T>
constexpr T round(double frac, T start, T end) {

    // First, round the fraction to determine which sample to choose

    uint8_t val = std::lround(frac);

    if (val) {
        // Rounded to 1, return end
        return end;
    }

    // Rounded to zero, return start

    return start;
}

/**
 * @brief Preforming linear interpolation between two values
 *
 * We simply take a weighted average of the two values, where the weights are
 * determined by the fraction.
 * This essentially draws a line between the two values,
 * and then finds the value on that line at the given fraction.
 * This is a very simple method, and it leads to a continuous output.
 *
 * @tparam T Numerical type
 * @param start Starting value
 * @param end Stopping value
 * @param frac Fraction between start and end to interpolate at, from 0 to 1
 * @return T Interpolated value between start and end at the given fraction
 */
template <typename T>
constexpr T linear(double frac, T start, T end) {

    return start + ((end - start) * frac);
}

/**
 * @brief 3 point, 3rd order B-spline interpolation
 *
 * This method uses four points and the B-spline method
 * to interpolate between the middle two points.
 * It guarantees continuity of the first and second derivatives, which leads to
 * a very smooth output at the cost of computational complexity.
 * B-splines are NOT guaranteed to pass through the middle two points, but they
 * will be close.
 *
 * @tparam T Numerical type
 * @param x Fraction between y1 and y2 to interpolate at, from 0 to 1
 * @param y0 Value at x = -1
 * @param y1 Value at x = 0
 * @param y2 Value at x = 1
 * @param y3 Value at x = 2
 * @return T Interpolated value between y1 and y2 at the given fraction
 */
template <typename T>
constexpr T bspline4(T x, T y0, T y1, T y2, T y3) {

    T ym1py1 = y0 + y2;
    T c0 = (1 / 6 * ym1py1) + (2 / 3 * y1);
    T c1 = 1 / 2 * (y2 - y0);
    T c2 = (1 / 2 * ym1py1) - y1;
    T c3 = (1 / 2 * (y1 - y2)) + (1 / 6 * (y3 - y0));
    return (((((c3 * x) + c2) * x) + c1) * x) + c0;
}

/**
 * @brief 6 point, 5th order B-spline interpolation
 *
 * This method uses six points and the B-spline method to interpolate between
 * the middle two points. It guarantees continuity of the first four
 * derivatives, which leads to a very smooth output at the cost of computational
 * complexity. B-splines are NOT guaranteed to pass through the provided
 * points, but they will be close.
 *
 * @tparam T Numerical type
 * @param x Fraction between y2 and y3 to interpolate at, from 0 to 1
 * @param y0 Value at x = -2
 * @param y1 Value at x = -1
 * @param y2 Value at x = 0
 * @param y3 Value at x = 1
 * @param y4 Value at x = 2
 * @param y5 Value at x = 3
 * @return T Interpolated value between y2 and y3 at the given fraction
 */
template <typename T>
constexpr T bspline6(T x, T y0, T y1, T y2, T y3, T y4, T y5) {
    // 6-point, 5th-order B-spline (x-form)
    T ym2py2 = y0 + y4;
    T ym1py1 = y1 + y3;
    T y2mym2 = y4 - y0;
    T y1mym1 = y3 - y1;
    T sixthym1py1 = 1 / 6.0 * ym1py1;
    T c0 = (1 / 120.0 * ym2py2) + (13 / 60.0 * ym1py1) + (11 / 20.0 * y2);
    T c1 = (1 / 24.0 * y2mym2) + (5 / 12.0 * y1mym1);
    T c2 = (1 / 12.0 * ym2py2) + sixthym1py1 - (1 / 2.0 * y2);
    T c3 = (1 / 12.0 * y2mym2) - (1 / 6.0 * y1mym1);
    T c4 = (1 / 24.0 * ym2py2) - sixthym1py1 + (1 / 4.0 * y2);
    T c5 = (1 / 120.0 * (y5 - y0)) + (1 / 24.0 * (y1 - y4)) +
           (1 / 12.0 * (y3 - y2));
    return (((((((((c5 * x) + c4) * x) + c3) * x) + c2) * x) + c1) * x) + c0;
}

/**
 * @brief 4 point, 3rd order Lagrange interpolation
 *
 * This method uses four points and the Lagrange method to interpolate between
 * the middle two points. It guarantees that the output will pass through the
 * provided points, but it does not guarantee continuity of any derivatives.
 *
 * @tparam T Numerical type
 * @param x Fraction between y1 and y2 to interpolate at, from 0 to 1
 * @param y0 Value at x = -1
 * @param y1 Value at x = 0
 * @param y2 Value at x = 1
 * @param y3 Value at x = 2
 * @return T Interpolated value between y1 and y2 at the given fraction
 */
template <typename T>
constexpr T lagrange4(T x, T y0, T y1, T y2, T y3) {
    T c0 = y1;
    T c1 = y2 - (1 / 3.0 * y0) - (1 / 2.0 * y1) - (1 / 6.0 * y3);
    T c2 = (1 / 2.0 * (y0 + y2)) - y1;
    T c3 = (1 / 6.0 * (y3 - y0)) + (1 / 2.0 * (y1 - y2));
    return (((((c3 * x) + c2) * x) + c1) * x) + c0;
}

/**
 * @brief 6 point, 5th order Lagrange interpolation
 *
 * This method uses four points and the Lagrange method to interpolate between
 * the middle two points. It guarantees that the output will pass through the
 * provided points, but it does not guarantee continuity of any derivatives.
 *
 * @tparam T Numerical type
 * @param x Fraction between y1 and y2 to interpolate at, from 0 to 1
 * @param y0 Value at x = -2
 * @param y1 Value at x = -1
 * @param y2 Value at x = 0
 * @param y3 Value at x = 1
 * @param y4 Value at x = 2
 * @param y5 Value at x = 3
 * @return T Interpolated value between y2 and y3 at the given fraction
 */
template <typename T>
constexpr T lagrange6(T x, T y0, T y1, T y2, T y3, T y4, T y5) {

    // 6-point, 5th-order Lagrange (x-form)
    float ym1py1 = y1 + y3;
    float twentyfourthym2py2 = 1 / 24.0 * (y0 + y4);
    float c0 = y2;
    float c1 = (1 / 20.0 * y0) - (1 / 2.0 * y1) - (1 / 3.0 * y2) + y3 -
               (1 / 4.0 * y4) + (1 / 30.0 * y5);
    float c2 = (2 / 3.0 * ym1py1) - (5 / 4.0 * y2) - twentyfourthym2py2;
    float c3 = (5 / 12.0 * y2) - (7 / 12.0 * y3) + (7 / 24.0 * y4) -
               (1 / 24.0 * (y0 + y1 + y5));
    float c4 = (1 / 4.0 * y2) - (1 / 6.0 * ym1py1) + twentyfourthym2py2;
    float c5 = (1 / 120.0 * (y5 - y0)) + (1 / 24.0 * (y1 - y4)) +
               (1 / 12.0 * (y3 - y2));
    return (((((((((c5 * x) + c4) * x) + c3) * x) + c2) * x) + c1) * x) + c0;
}

// NOLINTEND(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)

}  // namespace dsp::interp
