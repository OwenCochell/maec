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

#include <utility>

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
