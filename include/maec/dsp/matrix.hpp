/**
 * @file matrix.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Various matrix operations
 * @version 0.1
 * @date 2023-09-07
 * 
 * @copyright Copyright (c) 2023
 * 
 * This header contains matrix and common matrix operations
 * that may be useful in DSP contexts.
 * These implementations are very basic!
 * These are no replacements for mature linear algebra libraries.
 * The speed of these operations are also probably not ideal,
 * so it is not recommended to use these components in realtime scenarios.
 * 
 * We utilize the generic multi-channel AudioBuffer as a matrix
 * in these operations, as it can correctly emulate the necessary functionality.
 */

#pragma once

#include "dsp/buffer.hpp"

/**
 * @brief Computes the dot product from two vectors.
 * 
 * The dot product can be calculated like so:
 * 
 * d = x0*y0 + x1*y1 + ... + xn*yn
 * 
 * For all values x and y in each provided vector,
 * and d is the scalar result.
 * 
 * This fuction will auto-infer the type of value to return.
 * It inspects the first iterator provided and utilizes the value type,
 * that being the type of info being iterated over.
 * If the first iterator is iterating over doubles,
 * then we will use doubles as the accumulator,
 * and will return doubles from this function.
 * 
 * If any of the provided sequences is smaller than the provided n value,
 * then we will encounter trouble!
 * Make sure the sequences have enough data present.
 * 
 * @tparam A Type of first iterator
 * @tparam B Type of second iterator
 * @tparam T Type of data we are iterating over, inferred from first iterator
 * @param aiter First iterator
 * @param biter Second iterator
 * @param num Number of values to compute
 * @return Scalar result of dot product
 */
template <typename A, typename B, typename T = typename std::iterator_traits<A>::value_type>
T dot_product(A aiter, B biter, int num) {

    T done = 0;

    // Iterate over each value in the provided iterators:

    for (int i = 0; i < num; ++i) {

        // Grab the value from each iterator and compute:

        done += aiter[i] * biter[i];
    }

    return done;
}

template <typename T, typename U, typename V>
void matrix_mult(const Buffer<T>& buf1, const Buffer<U>& buf2, Buffer<V>& out) {

    // Iterate over the rows:

    for (int row = 0; row < buf1.channel_capacity(); ++row) {

        // Iterate over the cols:

        for (int col = 0; col < buf2.channels(); ++buf2) {

            // Grab the vectors:

            auto rowv = buf1.sbegin();
            auto colv = buf2.ibegin();

            // Set the vector position:

            rowv.set_channel(row);
            colv.set_sample(col);

            // Compute the dot product at this position:

            auto done = dot_product(rowv, colv, buf1.channel_capacity());

            // Store the dot product in the final buffer:

            out.at(row, col) = done;
        }
    }
}
