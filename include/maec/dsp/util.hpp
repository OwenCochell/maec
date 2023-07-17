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
#include <utility>
#include <iterator>

template <typename I>
void bit_reverse(int size, I iter) {

    // Iterate over all components:

    for (int i = 0, j = 1; j < size-1; j++) {

        for (int k = size >> 1; k > (i ^= k); k >>= 1) {}

        if (i < j) {

            // Swap iter[i] <> iter[j]

            std::swap(iter[i], iter[j]);
        }
    }
}

template<typename I1, typename O>
void multiply_signals(int size, I1 input1, I1 input2, O output) {

    // Determine the type:

    typedef typename std::iterator_traits<I1>::value_type iter_type;

    // Multiply the two signals together:

    std::transform(input1, input1+size, input2, output, [](iter_type val1, iter_type val2) {

        // Multiply the two signals:

        return val1 * val2;
    });
}

template<typename I, typename O>
void real_complex_naive(I input, int size, O output) {

    // Determine type:

    typedef typename std::iterator_traits<I>::value_type iter_type;

    // Transform real data into complex data:

    std::transform(input, input+size, output, [](iter_type val) {

        // Return new complex value:

        return std::complex<iter_type>(val, 0);
    });
}

template <typename I, typename O>
void real_eop_complex(I input, int size, O output) {

    // Iterate over real data:

    for (int i = 0; i < size / 2; ++i) {

        // Add data to output:

        *(output+i) = std::complex<long double>(*(input + (2 * i)), *(input + (2 * i + 1)));
    }
}
