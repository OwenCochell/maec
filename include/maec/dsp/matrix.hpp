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

    int done = 0;

    // Iterate over each value in the provided iterators:

    for (int i = 0; i < num; ++i) {

        // Grab the value from each iterator and compute:

        done += aiter[i] * biter[i];
    }

    return done;
}

/**
 * @brief Preforms a matrix multiplication on the provided buffers.
 * 
 * Given two buffers, the matrix product will be computed and placed
 * in the out buffer.
 * We will automatically size and configure the out buffer for you!
 * 
 * The matrix multiplication can be described as computing the dot product
 * of each row and column for each value in the out array.
 * Consider these matrixes A and B:
 * 
 * A:
 * 1 2 3
 * 4 5 6
 * 7 8 9
 * 
 * B:
 * 10 20 30
 * 40 50 60
 * 70 80 90
 * 
 * To compute the value at value <0,0>
 * (that is, row zero column zero)
 * in the output:
 * 
 * dot(row(A, 0), col(B, 0))
 * 1*10 + 2*40 + 3*70 = 300
 * 
 * Where row() and col() are functions that return
 * a vector of the row or column of the provided matrix.
 * Now, something more random, like <2, 1>:
 * 
 * dot(row(A, 2), col(B, 1))
 * 7*20 + 8*50 + 9*80 = 1260
 * 
 * At the end of the day, to compute the position of the output
 * matrix at any position <r, c> will be:
 * 
 * dot(row(A, r), row(B, c))
 * 
 * We utilize the Buffer methods to retrieve the row and column
 * of the input matrixes.
 * Also, if the provided matrixes have a different number of rows and columns,
 * then this operation will fail!
 * The number of rows in A MUST equal the number of columns in B!
 * 
 * This operation is NOT communicative!
 * You may get very different results if you switch the order of A and B.
 * 
 * @tparam T Type of input matrix A
 * @tparam U Type of input matrix B
 * @tparam V Type of output matrix
 * @param buf1 Input matrix A
 * @param buf2 Output matrix B
 * @param out Matrix to store result in
 */
template <typename T, typename U, typename V>
void matrix_mult(const Buffer<T>& buf1, const Buffer<U>& buf2, Buffer<V>& out) {

    // Configure the out buffer to match our size:

    out.set_channels(buf2.channels());
    out.set_channel_capacity(buf1.channel_capacity());

    // Iterate over the rows:

    for (int row = 0; row < buf1.channel_capacity(); ++row) {

        // Iterate over the cols:

        for (int col = 0; col < buf2.channels(); ++col) {

            // Grab the iterators:

            auto rowv = buf1.scbegin();
            auto colv = buf2.icbegin();

            // Set the iterator position:

            rowv.set_channel(row);
            colv.set_sample(col);

            // Compute the dot product at this position:

            auto done = dot_product(rowv, colv, buf1.channel_capacity());

            // Store the dot product in the final buffer:

            out.at(row, col) = done;
        }
    }
}
