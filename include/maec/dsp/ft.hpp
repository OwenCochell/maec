/**
 * @file ft.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Various Fourier Transform tools and components
 * @version 0.1
 * @date 2023-04-03
 *
 * @copyright Copyright (c) 2023
 *
 * This file contains various components for working
 * with Fourier Transform operations.
 * Each implementation has it's own benefits and nuances.
 *
 * A "Fourier Transform" is an operation that decomposes
 * a single signal into multiple, simpler signals.
 * This can be useful for MANY different reasons:
 *
 * https://www.dspguide.com/ch8.htm
 */

#pragma once

#include "../audio_buffer.hpp"

/**
 * @brief Cosine basis function
 *
 * This function will be utilized to represent 
 * the decomposed cosine components in the results of a FT operation.
 *
 * @param phase Phase to compute
 * @param total Total number of points in FT operation
 * @param freq Frequency, i.e number of cycles to cover N points
 * @return Value at current phase
 */
long double cos_basis(int phase, int total, long double freq);

/**
 * @brief Sine basis function
 *
 * This function will be utilized to represent
 * the decomposed sine components in the results of a FT operation. 
 *
 * @param phase Phase to compute
 * @param total Total number of points in the FT operation
 * @param freq Frequency, i.e number of cycles to cover N points
 * @return Value at current phase
 */
long double sin_basis(int phase, int total, long double freq);

/**
 * @brief Determines the length of real and non-real results from a DFT operation
 * 
 * When applying a FT operation on an input signal, we will 
 * have two output parts of a set size, defined by this equation:
 * 
 * output = (size / 2) + 1
 * 
 * @param size Size of input signal
 * @return int Size of the real and non-real parts
 */
int length_ft(std::size_t size);

/**
 * @brief Determines the length of the output signal result from an inverse DFT operation
 * 
 * When preforming an inverse FT operation on a real and non-real part,
 * we will have one output signal of a set size, defined by this equation:
 * 
 * output = (size - 1) * 2
 * 
 * @param size Size of real and non-real parts
 * @return int Size of output signal
 */
int length_ift(std::size_t size);

/**
 * @brief Preforms an Inverse DFT  
 * 
 * We compute the inverse DFT using the given real and non-real parts.
 * We expect iterators for the real, unreal, and output buffers,
 * as well as the size of the real and unreal buffers.
 * 
 * The incoming real and non-real parts MUST be the same size!
 * 
 * TODO:
 * 
 * Maybe elaborate more?
 * Include equation?
 * Maybe offer custom basis functions? That could be cool...
 * 
 * @tparam R Real iterator type
 * @tparam U Non-real iterator type
 * @tparam O Output iterator type
 * @param real Start iterator for real data
 * @param nonreal Start iterator for unreal data
 * @param size Size of real/non-real data
 * @param output Start iterator for output buffer
 */
template<typename R, typename U, typename O>
void inv_dft(R real, U nonreal, int size, O output) {

    // Determine final output size:

    int final_size = (size - 1) * 2;

    // Determine division value:

    const long double div_value = final_size / 2.0;

    // Grab initial values for future reference:

    auto real0 = *(real);
    auto reale = *(real + (size - 1));

    // Preform edge case normalization operation, divide by final_size:

    *(real) /= 2;
    *(real + (size - 1)) /= 2;

    // Iterate over each sample in components:

    for (int k = 0; k < size; ++k) {

        // Grab samples for this operation and normalize:

        long double real_part = *(real+k) / (div_value);
        long double nonreal_part = *(nonreal+k) / (-div_value);

        // Iterate over output:

        for (int i = 0; i < final_size; ++i) {

            // Preform operation on real part:

            *(output+i) += (real_part * cos_basis(i, final_size, k)) + (nonreal_part * sin_basis(i, final_size, k));
        }
    }

    // Finally, undo edge case normalization:

    *(real) = real0;
    *(real + (size - 1)) = reale;
}

/**
 * @brief Preforms an Inverse DFT
 * 
 * This function is identical to the normal inv_dft() function,
 * with the exception that we take a unique pointer to an AudioBuffer
 * containing the real and non-real parts.
 * We also return a unique pointer to an AudioBuffer containing results.
 * 
 * @param real Pointer to Real part
 * @param nonreal Pointer Non-real part
 * @return Pointer to buffer containing result
 */
BufferPointer inv_dft(BufferPointer real, BufferPointer nonreal);

/**
 * @brief Preforms a Discreet Fourier Transform
 * 
 * We compute the DFT using the given input signal.
 * We store the real and non-real results in the given iterators.
 *
 * @tparam R Real iterator type
 * @tparam U Non-real iterator type
 * @tparam I Input iterator type
 * @param input Start iterator to input data
 * @param size Size of input data
 * @param real Output iterator to real data
 * @param nonreal Output iterator to non-real data
 */
template<typename I, typename R, typename U>
void dft(I input, int size, R real, U nonreal) {

    // Determine size of output buffers:

    int output_size = (size / 2) + 1;

    // Iterate over output size:

    for (int k = 0; k < output_size; ++k) {

        // Iterate over total size:

        for (int i = 0; i < size; ++i) {

            // Determine value for real part:

            long double val = *(input+i);

            *(real+k) += val * cos_basis(i, size, k);
            *(nonreal+k) += -val * sin_basis(i, size, k);
        }
    }
}

/**
 * @brief Preforms a Discreet Fourier Transform
 * 
 * This function is identical to the dft() function,
 * with the exception that we take a BufferPointer containing the input signal,
 * and return the result as a BufferPointer,
 * with the real part existing in channel 0 and the non-real part existing in channel 1.
 * 
 * @param input Pointer to input signal
 * @return Pointer to AudioBuffer containing real and non-real parts
 */
BufferPointer dft(BufferPointer input);
