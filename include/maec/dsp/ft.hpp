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

#include <complex>

#include "../audio_buffer.hpp"

/**
 * This section defines functions for calculating
 * the Discreet Fourier Transform, as well as it's inverse.
 * 
 * These functions are very trivial and simple to use,
 * and they have few surprises and nuances.
 * Unfortunately, these functions are also incredibly slow.
 *
 * If you want extra speed, and extra complexity,
 * then you should look below at the FFT algorithms.
 * 
 */

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

    int final_size = length_ift(size);

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

    int output_size = length_ft(size);

    // Iterate over output size:

    for (int k = 0; k < output_size; ++k) {

        // Iterate over total size:

        auto real_iter = real+k;
        auto nonreal_iter = nonreal+k;

        for (int i = 0; i < size; ++i) {

            // Determine value for real part:

            long double val = *(input+i);

            *(real_iter) += val * cos_basis(i, size, k);
            *(nonreal_iter) += -val * sin_basis(i, size, k);
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

/**
 * This section defines functions for calculating
 * the Fast Fourier Transform, as well as it's inverse.
 * 
 * The FFT is a complicated algorithm that has some nuances.
 * For example, it works primarily with non-real numbers.
 * These functions do contain versions that work with real data,
 * but there are some nuances to consider when using these.
 * 
 * These functions have different implementations,
 * some being in-place, out of place, Decimation in Time(DIT),
 * Decimation in Frequency(DIF), and type (radix-2, radix-4, radix-n, ect,)
 * 
 * Please read the function documentation for a breakdown
 * on the features of each implementation!
 * 
 */

/**
 * @brief Preforms a non-real, out of place, radix2 FFT
 * 
 * This function is very naive!
 * Is is implemented via recursion,
 * and is very slow (at least compared to other FFT algorithms...)
 * 
 * @tparam I Input iterator type
 * @tparam O Output iterator type
 * @param input Input iterator for complex data
 * @param size Size of input data
 * @param output Output iterator for complex data
 * @param stride Current stride (step size)
 */
template <typename I, typename O>
void fft_nr_radix2(I input, int size, O output, int stride=1) {

    // Determine if we have a trivial size:

    if (size == 1) {

        // Trivial case, size=1 is just current value:

        *(output) = *input;

        return;
    }

    // Pre-compute some common values:

    const int N = size/2;
    const long double THETA = 2 * M_PI / size;

    // Call function recursively for first half:

    fft_nr_radix2(input, N, output, stride * 2);

    // Call function recursively for second half:

    fft_nr_radix2(input + N, N, output + N, stride * 2);

    // Iterate over components:

    for (int p = 0; p < N; ++p) {

        // Grab current values:

        const std::complex<long double> a = *(input + p);
        const std::complex<long double> b = *(input + p + N);

        // Determine the twiddle factor:

        //std::complex<long double> twiddle = std::polar<long double>(1.0, THETA * p);
        std::complex<long double> twiddle = std::complex<long double>(cos(p*THETA), -sin(p*THETA));

        // Determine new values:

        *(output + p) = a + b;
        *(output + p + N) = (a - b) * twiddle;
    }
}

template <typename I, typename O>
void ifft_nr_radix2(I input, int size, O output, int stride=1) {

    // First, take conjugate of incoming variables:

    for (int i = 0; i < size; ++i) {

        // Take the conjugate:

        *(input+i) = std::conj(*(input+i));
    }

    // Run through FFT function:

    fft_nr_radix2(input, size, output, stride);

    // Finally, normalize:

    for (int i = 0; i < size; ++i) {

        // Take conjugate once more and normalize:

        *(output+i) = std::conj(*(output+i)) / static_cast<double>(size);
    }    
}
