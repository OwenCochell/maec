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
#include <complex>

/**
 * @brief A component that chooses between types based upon a flag
 * 
 * Simply provide a bool, and this component
 * will select a type based upon it.
 * If true is provided, then the type provided to True will be used.
 * If false is provided, then the type provided to False will be used.
 * 
 * You can access the type under the 'type' attribute.
 * 
 * @tparam flag Boolean to decide which type to use
 * @tparam True Type to select if True
 * @tparam False Type to select if False
 */
template <bool flag, typename True, typename False>
struct ChooseType;

template<typename True, typename False>
struct ChooseType<true, True, False> {
    
    using type = True;
};

template<typename True, typename False>
struct ChooseType<false, True, False> {

    using type = False;
};

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
long double sinc(long double x);

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
