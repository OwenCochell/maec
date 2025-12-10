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

#include "util.hpp"

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
double cos_basis(int phase, int total, double freq);

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
double sin_basis(int phase, int total, double freq);

/**
 * @brief Determines the twiddle factor for a frequency component
 *
 * We calculate the twiddle factor of the given frequency component of the given
 * size. Users can optionally provide the sign of the twiddle factor, with (-1)
 * for the forward FFT, and (1) for the backward FFT. We utilize the following
 * equation:
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
 * By default we store results as doubles.
 *
 * @tparam T Type of output complex value
 * @param k Frequency component to calculate
 * @param size Size of input data
 * @param sign Sign to use, (-1) forward, (1) backward
 * @return std::complex<T> Twiddle factor
 */
template <typename T>
std::complex<T> twiddle(int k, int size, int sign = -1) {

    std::complex<T> res = std::polar<T>(1.0, sign * 2 * M_PI * k / size);

    return res;
}

/**
 * @brief Computes the A coefficient for real FFT processing
 *
 * When processing (in either direction) data related to the real valued FFT,
 * it is necessary to multiply the input data to the A and B coefficients
 * to result in proper output.
 *
 * This function computes the A coefficient for this operation.
 * The A coefficient is computed as follows:
 *
 * A(k, N) = (1 / 2) * (1 - j * W(k, N))
 *
 * Where k is the current frequency component,
 * N is the size of the incoming data
 * (that is, the size of the data before real/odd ordering, which results in
 * data of N/2 size), and W(k, N) is the twiddle factor method.
 *
 * This function will be called automatically where necessary!
 * Most of the time you will never have to worry about the output of this
 * method.
 *
 * @tparam T Type of complex data
 * @param k Frequency component to calculate
 * @param size Size of data to compute
 * @return Complex result of A()
 */
template <typename T>
std::complex<T> compute_a(int k, int size) {

    using namespace std::complex_literals;

    auto res = twiddle<T>(k, size);

    return (static_cast<T>(1.0) - res * 1i) / static_cast<T>(2);
}

/**
 * @brief Computes the B coefficient for real FFT processing
 *
 * When processing (in either direction) data related to the real valued FFT,
 * it is necessary to multiply the input data to the A and B coefficients
 * to result in proper output.
 *
 * This function computes the B coefficient for this operation.
 * The B coefficient is computed as follows:
 *
 * B(k, N) = (1 / 2) * (1 + j * W(k,N))
 *
 * Where k is the current frequency component,
 * N is the size of the incoming data
 * (that is, the size of the data before real/odd ordering, which results in
 * data of N/2 size), and W(k, N) is the twiddle factor method.
 *
 * This function will be called automatically where necessary!
 * Most of the time you wil never have to worry about the output of this method.
 *
 * @tparam T Type of complex data
 * @param k Frequency component to calculate
 * @param size Size of data to compute
 * @return Complex result of B()
 */
template <typename T>
std::complex<T> compute_b(int k, int size) {

    using namespace std::complex_literals;

    auto res = twiddle<T>(k, size);

    return (static_cast<T>(1.0) + res * 1i) / static_cast<T>(2);
}

/**
 * @brief Processes forward and backward FFT data.
 *
 * When processing real data in the FFT
 * (placing even values in real component and odd in imaginary),
 * it is necessary to preform some processing steps to ensure
 * output data is what we expect.
 *
 * The forward processing formula is as follows:
 *
 * O(k) = X[k] * A(k) + X*[N - k] * B(k)
 *
 * The inverse (backwards) processing formula is as follows:
 *
 * O(k) = X[k] * A*(k) + X*[N - k] * B*(k)
 *
 * Where k is the frequency component,
 * N is the size of the of the data
 * (that is, the size of the data after real/odd ordering, which is 2N in size),
 * and A/B are the coefficients, which are documented above.
 *
 * This method should be called when working with frequency information
 * of real data that has been ran through the FFT.
 * After sending the frequency info of complex valued data through the forward
 * FFT, then it should be sent through this method. Before sending the frequency
 * info of the complex valued data through the inverse FFT, then it should be
 * sent through this method.
 *
 * The data to be provided MUST have the size of (N / 2) + 1
 * (again, N is the size of the data before real/odd ordering).
 * This can be calculated by sending the size through the 'ft_size()' function.
 * You also need to provide if you want to do a forward or inverse operation,
 * simply pass false for forward, and true for backward.
 * This method also works in place!
 *
 * @tparam T Type of complex data
 * @param complex Iterator of complex data
 * @param size Size of complex data before real/odd ordering
 * @param invert true for backward operation, false for forward operation
 */
template <typename T>
void fft_process_real(T complex, int size, bool invert) {

    // Set last input value to 0th value, if we are forward:

    if (!invert) {

        complex[size / 2] = complex[0];
    }

    // Run through each K value:

    int total = size >> 2;

    for (int k = 0; k <= total; ++k) {

        // Determine left A and B values:

        std::complex<double> a1 = compute_a<double>(k, size);
        std::complex<double> b1 = compute_b<double>(k, size);

        // Determine right A and B values:

        std::complex<double> a2 = compute_a<double>(size / 2 - k, size);
        std::complex<double> b2 = compute_b<double>(size / 2 - k, size);

        // Determine if we are doing a backwards operation:

        if (invert) {

            // Conjugate each value:

            a1 = std::conj(a1);
            b1 = std::conj(b1);
            a2 = std::conj(a2);
            b2 = std::conj(b2);
        }

        // Grab each value:

        std::complex<double> left = *(complex + k);  // Grab left value
        std::complex<double> right =
            *(complex + (size / 2 - k));  // Grab right value

        // Determine value in first half:
        // X[k] * A(K) + X*[N - k] * B(k)
        // left * A(k) + *right * B(k)

        *(complex + k) = left * a1 + std::conj(right) * b1;

        // Determine value in second half:
        // X[N - k] * A(N - k) + X*[k] * B(N - K)
        // right * A(N - k) + *left * B(N - k)

        *(complex + (size / 2 - k)) = right * a2 + std::conj(left) * b2;
    }
}

/**
 * @brief Determines the length of real and non-real results from a DFT
 * operation
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
 * @brief Determines the length of the output signal result from an inverse DFT
 * operation
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
template <typename R, typename U, typename O>
void inv_dft(R real, U nonreal, int size, O output) {

    // Determine final output size:

    int final_size = length_ift(size);

    // Determine division value:

    const double div_value = final_size / 2.0;

    // Grab initial values for future reference:

    auto real0 = *(real);
    auto reale = *(real + (size - 1));

    // Preform edge case normalization operation, divide by final_size:

    *(real) /= 2;
    *(real + (size - 1)) /= 2;

    // Iterate over each sample in components:

    for (int k = 0; k < size; ++k) {

        // Grab samples for this operation and normalize:

        double real_part = *(real + k) / (div_value);
        double nonreal_part = *(nonreal + k) / (-div_value);

        // Iterate over output:

        for (int i = 0; i < final_size; ++i) {

            // Preform operation on real part:

            *(output + i) += (real_part * cos_basis(i, final_size, k)) +
                             (nonreal_part * sin_basis(i, final_size, k));
        }
    }

    // Finally, undo edge case normalization:

    *(real) = real0;
    *(real + (size - 1)) = reale;
}

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
template <typename I, typename R, typename U>
void dft(I input, int size, R real, U nonreal) {

    // Determine size of output buffers:

    int output_size = length_ft(size);

    // Iterate over output size:

    for (int k = 0; k < output_size; ++k) {

        // Iterate over total size:

        auto real_iter = real + k;
        auto nonreal_iter = nonreal + k;

        for (int i = 0; i < size; ++i) {

            // Determine value for real part:

            double val = *(input + i);

            *(real_iter) += val * cos_basis(i, size, k);
            *(nonreal_iter) += -val * sin_basis(i, size, k);
        }
    }
}

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
 * Please be aware that we work with doubles under the hood.
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
    const double THETA = sign * 2 * M_PI / size;

    // Call function recursively for first half:

    fft_c_radix2(input, N_2, output, stride * 2, sign);

    // Call function recursively for second half:

    fft_c_radix2(input + stride, N_2, output + N_2, stride * 2, sign);

    // Iterate over frequency components:

    for (int p = 0; p < N_2; ++p) {

        // Grab current values:

        const std::complex<double> first = *(output + p);
        const std::complex<double> second = *(output + p + N_2);

        // Determine the twiddle factor:

        std::complex<double> twiddle =
            std::polar<double>(1.0, THETA * p) * second;

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
 * Please be aware that we work with doubles under the hood.
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

        *(output + i) = *(output + i) / static_cast<double>(size);
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
 * Please be aware that we work with doubles under the hood.
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
    const double THETA = sign * 2 * M_PI / size;

    // Iterate over frequency components:

    for (int p = 0; p < N_2; ++p) {

        // Grab current values:

        const std::complex<double> first = *(input + p);
        const std::complex<double> second = *(input + p + N_2);

        // Determine the twiddle factor:

        std::complex<double> twiddle = std::polar<double>(1.0, THETA * p);

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
 * Please be aware that we work with doubles under the hood.
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

        *(input + i) = *(input + i) / static_cast<double>(size);
    }
}

template <typename I, typename O>
void fft_r_radix2(I input, int size, O output) {

    // First, determine iterator type:

    typedef typename std::iterator_traits<I>::value_type iter_type;

    // Cast input data into complex array:

    auto* cmp = reinterpret_cast<std::complex<iter_type>*>(&(*input));

    // Send data through FFT function:

    fft_c_radix2(cmp, size / 2, output);

    // Complex output lives in output, do some processing:

    fft_process_real(output, size, false);
}

template <typename I, typename O>
void ifft_r_radix2(I input, int size, O output) {

    // First, determine iterator type:

    typedef typename std::iterator_traits<O>::value_type iter_type;

    // Determine output size:

    int osize = length_ift(size);

    // Run data through FFT process method:

    fft_process_real(input, osize, true);

    // Cast the iterator as a complex array:

    auto* cmp = reinterpret_cast<std::complex<iter_type>*>(&(*output));

    // Send data through iFFT function (excluding last value):

    ifft_c_radix2(input, size - 1, cmp);
}
