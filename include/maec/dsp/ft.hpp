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
#include <iterator>

#include "audio_buffer.hpp"
#include "dsp/util.hpp"

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
 * @brief Determines the twiddle factor for a frequency component
 *
 * We calculate the twiddle factor of the given frequency component of the given size.
 * Users can optionally provide the sign of the twiddle factor,
 * with (-1) for the forward FFT, and (1) for the backward FFT.
 * We utilize the following equation:
 *
 * twiddle = e ^ (j * sign * 2 * k * PI / size)
 *
 * (j being the imaginary component)
 * Which expands out to this:
 *
 * real = cos(sign * 2 * k * PI / size)
 * imaginary = sin(sign * 2 * k * PI / size)
 *
 * Where the real and imaginary parts will be
 * used in the complex type and returned.
 * By default we store results as long doubles.
 * 
 * @tparam T Type of output complex value
 * @param k Frequency component to calculate
 * @param size Size of input data
 * @param sign Sign to use
 * @return std::complex<T> Twiddle factor
 */
template <typename T = long double>
std::complex<T> twiddle(int k, int size, int sign = -1) {

}

template<typename T, typename I>
std::complex<T> normalize_e(I input, int size_2, int k) {

    // Calculate the real part:

    return (*(input + k) + std::conj(*(input + (size_2 - k)))) / static_cast<long double>(2.0);

}

template<typename T, typename I>
std::complex<T> normalize_o(I input, int size_2, int k) {

    // Calculate imaginary part:

    using namespace std::complex_literals;

    return -1il * ((*(input + k) - std::conj(*(input + (size_2 - k)))) / static_cast<long double>(2.0));
}

template<typename T>
void postprocess_fft_c(T complex, int size) {

    const long double THETA = -1 * 2 * M_PI / size;

    // Run through each K value:
    // TODO: try doing this out of place instead?

    for (int k = 0; k < size/2; ++k) {

         // Determine the twiddle factor:

        std::complex<long double> twiddle = std::polar<long double>(1.0, THETA * k);

        std::complex<long double> part_e = normalize_e<long double>(complex, size/2, k);
        std::complex<long double> part_o = normalize_o<long double>(complex, size / 2, k);

        *(complex+k) = part_e + part_o * twiddle;
    }

    // Calculate value at N/2:

    *(complex + size / 2) = normalize_e<long double>(complex, size / 2, size / 2) - normalize_o<long double>(complex, size / 2, size / 2);
}

// void lastrealstage(double *datareal, std::complex<long double> *twiddle, unsigned int framesize)
// {
//     unsigned int n, f = framesize >> 1;
//     long double evenreal, evenim, oddreal, oddim; // temp variables
//     std::complex<long double> *data = (std::complex<long double> *)datareal; // cast the pointer

//     for (n = framesize >> 2; n; n--) // include the 'half-Nyquist' point
//     {
//          evenreal = data[n].real + data[f - n].real; // split the spectra
//          evenim = data[n].im - data[f - n].im;
//          oddreal = data[n].im + data[f - n].im;
//          oddim = data[f - n].real - data[n].real;

//          // twiddle and add

//          data[n].real = evenreal + (oddreal * twiddle[n].real - oddim * twiddle[n].im);
//          data[n].im = evenim + (oddim * twiddle[n].real + oddreal * twiddle[n].im);

//          data[f - n].real = evenreal + (oddreal * twiddle[f - n].real + oddim * twiddle[f - n].im);
//          data[f - n].im = -evenim + (-oddim * twiddle[f - n].real + oddreal * twiddle[f - n].im);
//     }
//     evenreal = data[0].real;
//     data[0].real = (data[0].real + data[0].im) * 2.; // double DC value
//     data[0].im = (evenreal - data[0].im) * 2;        // double Nyquist and store next to DC
// }

// void postprocess_fft_c_new(std::complex<long double> *complex, int size) {

//     int f = size >> 1;
//     const long double THETA = -1 * 2 * M_PI / size;

//     for (int n = size >> 2; n; n--) {

//         long double evenreal = complex[n].real() + complex[f - n].real();
//         long double evenim = complex[n].imag() - complex[f - n].imag();
//         long double oddreal = complex[n].imag() + complex[f - n].imag();
//         long double oddim = complex[f - n].real() - complex[n].real();

//         // Twiddle and add:

//         long double final_n_real = evenreal + (oddreal);
//     }
// }

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
 * @brief Preforms a complex, out of place, radix2 FFT
 * 
 * This function is very naive!
 * Is is implemented via recursion,
 * and is very slow (at least compared to other FFT algorithms...)
 * This is an implementation of the Cooley-Turkey algorithm.
 * 
 * We work with iterators (or array pointers if necessary),
 * allowing this function to work with many different
 * containers and datatypes.
 * Please be aware that we work with long doubles under the hood.
 * 
 * The size of this input data MUST be a power of two!
 * We will happily work with data that does not match this requirement,
 * but the outgoing data will be incorrect!
 * Please be sure that the output array has the correct
 * size reserved! We will encounter issues if not.
 * 
 * @tparam I Input iterator type
 * @tparam O Output iterator type
 * @param input Input iterator for complex data
 * @param size Size of input data
 * @param output Output iterator for complex data
 * @param stride Current stride (step size)
 * @param sign Sign of the exponent, (-1) for forward, (1) for backward
 */
template <typename I, typename O>
void fft_c_radix2(I input, int size, O output, int stride = 1, int sign = -1) {

    // Determine if we have a trivial size:

    if (size == 1) {

        // Trivial case, size=1 is just current value:

        *(output) = *input;

        return;
    }

    // Pre-compute some common values:

    const int N_2 = size / 2;
    const long double THETA = sign * 2 * M_PI / size;

    // Call function recursively for first half:

    fft_c_radix2(input, N_2, output, stride * 2, sign);

    // Call function recursively for second half:

    fft_c_radix2(input + stride, N_2, output + N_2, stride * 2, sign);

    // Iterate over frequency components:

    for (int p = 0; p < N_2; ++p) {

        // Grab current values:

        const std::complex<long double> first = *(output + p);
        const std::complex<long double> second = *(output + p + N_2);

        // Determine the twiddle factor:

        std::complex<long double> twiddle = std::polar<long double>(1.0, THETA * p) * second;

        // Determine new values:

        *(output + p) = first + twiddle;
        *(output + p + N_2) = first - twiddle;
    }
}

/**
 * @brief Preforms a complex, in place, radix2 inverse FFT
 * 
 * This function uses the fft_c_radix2 (out of place) function under the hood!
 * We simply specify that the sign is positive one (1).
 * and pass the data along to fft_c_radix2.
 * This means that this function inherits the speed issues
 * of forward fft_c_radix2 function.
 * 
 * We also preform some normalization on the output data,
 * where we divide each value in the output by the size.
 * 
 * We work with iterators (or array pointers if necessary),
 * allowing this function to work with many different
 * containers and datatypes.
 * Please be aware that we work with long doubles under the hood.
 * 
 * The size of this input data MUST be a power of two!
 * We will happily work with data that does not match this requirement,
 * but the outgoing data will be incorrect!
 * Please be sure that the output array has the correct
 * size reserved! We will encounter issues if not.
 * 
 * @tparam I Input iterator type
 * @tparam O Output iterator type
 * @param input Input iterator of complex data
 * @param size Size of input data
 * @param output Output iterator of complex data
 * @param stride Current stride (step size)
 */
template <typename I, typename O>
void ifft_c_radix2(I input, int size, O output, int stride = 1) {

    // Run through FFT function:

    fft_c_radix2(input, size, output, stride, 1);

    // Finally, normalize:

    for (int i = 0; i < size; ++i) {

        // Normalize the output:

        *(output+i) = *(output+i) / static_cast<long double>(size);
    }
}

/**
 * @brief Preforms a complex, in place, radix2 FFT
 * 
 * This function is very naive!
 * Is is implemented via recursion,
 * and is very slow (at least compared to other FFT algorithms...)
 * This is an implementation of the in-place Cooley-Turkey algorithm.
 * 
 * It is important to note that we do NOT
 * do bit reversal sorting on the output data.
 * This means the data will be not be in natural order.
 * For forward/reverse transforms, as well certain processing
 * operations (like fast convolution!), this out-of-order
 * data works fine.
 * If you do want to work with natural order data,
 * then you can look elsewhere in maec for bit reversal sorting functions.
 * 
 * We work with iterators (or array pointers if necessary),
 * allowing this function to work with many different containers and datatypes.
 * Please be aware that we work with long doubles under the hood.
 * 
 * The size of this input data MUST be a power of two!
 * We will happily work with data that does not match this requirement,
 * but the outgoing data will be incorrect!
 * 
 * @tparam I Input iterator type
 * @param input Input iterator of complex data
 * @param size Size of input data
 */
template <typename I>
void fft_c_radix2(I input, int size, int sign = -1) {

    // Determine if we are working with the trivial case:

    if (size == 1) {

        // Trivial case, do nothing:

        return;
    }

    // Pre-compute some common values:

    const int N_2 = size / 2;
    const long double THETA = sign * 2 * M_PI / size;

    // Iterate over frequency components:

    for (int p = 0; p < N_2; ++p) {

        // Grab current values:

        const std::complex<long double> first = *(input + p);
        const std::complex<long double> second = *(input + p + N_2);

        // Determine the twiddle factor:

        std::complex<long double> twiddle = std::polar<long double>(1.0, THETA * p);

        // Determine new values:

        *(input + p) = first + second;
        *(input + p + N_2) = (first - second) * twiddle;
    }

    fft_c_radix2(input, size / 2, sign);
    fft_c_radix2(input + N_2, size / 2, sign);
}

/**
 * @brief Preforms a complex, in place, radix2 inverse FFT
 * 
 * This function uses the fft_c_radix2 (in place) function under the hood!
 * We simply specify that the sign is positive one (1).
 * and pass the data along to fft_c_radix2.
 * This means that this function inherits the speed issues
 * of forward fft_c_radix2 function.
 * 
 * It is important to note that, same as the fft_c_nr (in place) function,
 * we do NOT do bit reversal sorting on the output data.
 * This means the data will be not be in natural order.
 * For forward/reverse transforms, as well certain processing
 * operations (like fast convolution!), this out-of-order data works fine.
 * If you do want to work with natural order data,
 * then you can look elsewhere in maec for bit reversal sorting functions.
 * 
 * We also preform some normalization on the output data,
 * where we divide each value in the output by the size.
 * 
 * We work with iterators (or array pointers if necessary),
 * allowing this function to work with many different containers and datatypes.
 * Please be aware that we work with long doubles under the hood.
 * 
 * The size of this input data MUST be a power of two!
 * We will happily work with data that does not match this requirement,
 * but the outgoing data will be incorrect!
 * 
 * @tparam I Input iterator type
 * @param input Input iterator of complex data
 * @param size Size of input data
 */
template <typename I>
void ifft_c_radix2(I input, int size) {

    // Run through FFT function:

    fft_c_radix2(input, size, 1);

    // Finally, normalize:

    for (int i = 0; i < size; ++i) {

        // Normalize the output:

        *(input+i) = *(input+i) / static_cast<long double>(size);
    }
}

template <typename I, typename O>
void fft_r_radix2(I input, int size, O output) {

    // First, determine iterator type:

    typedef typename std::iterator_traits<I>::value_type iter_type;

    // Determine output size:

    int output_size = length_ft(size);

    // Cast input data into complex array:

    auto *cmp = reinterpret_cast<std::complex<iter_type>*>(&(*input));

    // Send data through FFT function:

    fft_c_radix2(cmp, size / 2, output);

    // Complex output lives in output, do some processing:

    postprocess_fft_c(output, size);

    // This is some testing stuff:

    int k = 0;

    auto part1 = *(output + k);
    auto part2 = *(output + (output_size / 2 - k));
    auto part3 = std::conj(part2);
}
