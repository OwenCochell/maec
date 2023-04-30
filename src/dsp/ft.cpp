/**
 * @file ft.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Implementations of FT algorithms
 * @version 0.1
 * @date 2023-04-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#define _USE_MATH_DEFINES

#include <cmath>

#include "dsp/ft.hpp"

long double cos_basis(int phase, int total, long double freq) {

    // Preform operation and return:

    return cos((2 * M_PI * freq * phase) / total);

}

long double sin_basis(int phase, int total, long double freq) {

    // Preform operation and return:

    return sin((2 * M_PI * freq * phase) / total);
}

int length_ft(std::size_t size) {

    // Simply calculate and return:

    return (size / 2) + 1;
}

int length_ift(std::size_t size) {

    // Simply calculate and return:

    return (size - 1) * 2;
}

BufferPointer inv_dft(BufferPointer real, BufferPointer nonreal) {

    // Create final output buffer:

    BufferPointer buff = std::make_unique<AudioBuffer>((real->size() - 1) * 2);

    // Pass values to other function:

    inv_dft(real->ibegin(), nonreal->ibegin(), static_cast<int>(real->size()), buff->ibegin());

    // Finally, return pointer to output buffer:

    return buff;
}

BufferPointer dft(BufferPointer input) {

    // Create final output buffer:

    BufferPointer buff = std::make_unique<AudioBuffer>((input->size() / 2) + 1, 2);

    // Pass values to other function:

    dft(input->ibegin(), static_cast<int>(input->size()), buff->chbegin()->begin(), (buff->chbegin()+1)->begin());

    // Finally, return pointer to output buffer:

    return buff;
}
