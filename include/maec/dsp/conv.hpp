/**
 * @file conv.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-01
 * 
 * @copyright Copyright (c) 2023
 * 
 * This file contains various tools and components used
 * for calculating Convolution operations.
 * We provide a few algorithms for doing so,
 * each with their own benefits.
 * 
 * Convolution is a very important operation,
 * it describes how an input signal is affected by another,
 * usually called the kernel.
 * It is mainly used to apply an impulse response to a signal,
 * allowing the operations of a linear system to be applied to a signal
 * in one operation!
 * 
 * For more information, check here:
 * 
 * https://www.dspguide.com/ch6.htm
 */

#pragma once

#include <memory>

#include "../audio_buffer.hpp"

/**
 * @brief Determines the length of the Convolution output given input sizes
 * 
 * Convolution will always have a set output side given the 
 * size of the two inputs, defined by this equation:
 * 
 * output = size1 + size2 - 1
 * 
 * @param size1 Size of first input signal
 * @param size2 Size of second input signal
 * @return int Size of output
 */
int length_conv(int size1, int size2);

/**
 * @brief Determines the length of the Convolution output given input sizes
 * 
 * Determines the final length using the size type.
 *
 * @param size1 Size of first input signal
 * @param size2 Size of second input signal
 * @return Size of output
 */
int length_conv(std::size_t size1, std::size_t size2);

/**
 * @brief Convolves the input signal with the kernel by input side using start iterators and signal sizes
 * 
 * Does a Convolution operation using the input side algorithm:
 * 
 * https://www.dspguide.com/ch6/3.htm
 * 
 * This algorithm is useful for understanding the operation
 * from the context of the inputs,
 * meaning that we determine how samples in the input signal
 * affect the samples in the output signal.
 * This algorithm does the following operations:
 * 
 * - Decompose the input signal
 * - Pass the components through the system (kernel)
 * - Synthesize the output
 * TODO: Figure out which is faster!
 * 
 * Convolution is associative!
 * It does not matter the order you pass the buffers,
 * but for the sake of style it is recommended to pass the input signal
 * and kernel in order to make your operation more clear.
 *
 * This function takes start input iterators for the input signal
 * and kernal, as well as the size of each value.
 * We also expect an output iterator to the buffer the result will be stored in.
 *  
 * @tparam I Input signal iterator type
 * @tparam K Kernel iterator type
 * @tparam O Output buffer iterator type
 * @param binput Start iterator to input signal
 * @param input_size Size of input signal
 * @param bkernel Start iterator of kernel
 * @param kernel_size Size of kernel
 * @param output Start iterator to output buffer
 */
template <typename I, typename K, typename O>
void input_conv(I input, unsigned int input_size, K kernel, unsigned int kernel_size, O output) {

    // Iterate over each point in input:

    for (unsigned int i = 0; i < input_size; ++i) {

        // Iterate over each point in kernel:

        for (unsigned int j = 0; j < kernel_size; ++j) {

            // Determine the value at this position:
            // output[i+j] += input[i] * kernel[j]

            *(output+i+j) += (*(input+i) * *(kernel+j));
        }
    }
}

/**
 * @brief Convolves the input signal with the kernel by input side using start and end iterators
 * 
 * This function is identical to the input_conv() function that takes
 * input iterators and sizes, with the exception that we take
 * start and stop iterators to determine the size of each signal.
 * 
 * @tparam I Input signal iterator type
 * @tparam K Kernel iterator type
 * @tparam O Output buffer iterator type 
 * @param binput Start iterator to input signal
 * @param einput End iterator to input signal
 * @param bkernel Start iterator to kernel
 * @param ekernel End iterator to kernel
 * @param output Iterator to output buffer
 */
template <typename I, typename K, typename O>
void input_conv(I binput, I einput, K bkernel, K ekernel, O output) {

    // Call the other method with sizes determined:

    input_conv(binput, std::distance(binput, einput), bkernel, std::distance(bkernel, ekernel), output);

}

/**
 * @brief Convolves the input signal with the kernel by input side using buffer pointers
 * 
 * This function is identical to the other input_conv() methods,
 * with the exception that we take a unique pointer to an AudioBuffer
 * for the input signal, as well as the kernel.
 * 
 * This function creates an AudioBuffer and returns the results.
 * 
 * @param input Input signal to be convolved
 * @param kernel Kernal to apply to the input signal
 * @return BufferPointer Pointer to buffer containing result
 */
BufferPointer input_conv(BufferPointer input, BufferPointer kernel);

/**
 * @brief Convolves the input signal with the by output side using start iterators and signal sizes
 * 
 * Does a Convolution operation using the output side algorithm:
 * 
 * https://www.dspguide.com/ch6/4.htm
 * 
 * This algorithm is useful for understanding the operation
 * in the context of the output.
 * We instead look at each sample in the output signal,
 * and find the contributing points from the input.
 * 
 * This issue with this operation is that is is not "fully immersed"
 * in the input signal, meaning that at some point we will consider values
 * that are past the bounds of the input signal.
 * We simply used 0 in these cases, but this means that the start
 * and end values will not be very useful.
 * 
 * Again, convolution is associative!
 * It foes not matter the order you pass the buffers,
 * but for the sake of style it is recommended to pass the input signal
 * and kernel in order to make your operation more clear.
 * 
 * @tparam I Input signal iterator type
 * @tparam K Kernel iterator type
 * @tparam O Output buffer iterator type
 * @param input Input signal start iterator
 * @param input_size Size of input signal
 * @param kernel Kernel start iterator
 * @param kernel_size Size of kernel
 * @param output Start iterator to output buffer
 */
template<typename I, typename K, typename O>
void output_conv(I input, int input_size, K kernel, int kernel_size, O output) {

    // Determine final size:

    int size = length_conv(input_size, kernel_size);

    // Iterate over each sample in output:

    for (int i = 0; i < size; ++i) {

        // Iterate over each point in kernel:

        for (int j = 0; j < kernel_size; ++j) {

            // Ensure we are within bounds

            if (i - j < 0 || i - j > input_size - 1) {

                // Simply do nothing

                continue;

            }

            // Do operation:
            // output[i] += kernel[j] * input[i - j]

            *output += *(kernel + j) * *(input + (i - j));
        }

        // Increment the output iterator:

        ++output;
    }
}

/**
 * @brief Convolves the input signal with the kernel by output side using start and end iterators
 * 
 * This function is identical to the other output_conv() functions,
 * with the exception that we take start and stop iterators
 * for the input signal and kernel.
 * 
 * @tparam I Input signal iterator type
 * @tparam K Kernel iterator type
 * @tparam O Output buffer iterator type
 * @param binput Input signal start iterator
 * @param eipnut Input signal stop iterator
 * @param bkernel Kernel start iterator
 * @param ekernel Kernel stop iterator
 * @param output Start iterator to output buffer
 */
template<typename I, typename K, typename O>
void output_conv(I binput, I einput, K bkernel, K ekernel, O output) {

    // Call other method with sizes determined:

    output_conv(binput, std::distance(binput, einput), bkernel, std::distance(bkernel, ekernel), output);
}

/**
 * @brief Convolves the input signal with the kernel by output side using buffer pointers
 * 
 * This function is identical to the other output_conv() functions,
 * with the exception that we take a unique pointer for the input signal
 * and kernel.
 * 
 * The function create an AudioBuffer, stores the result in there,
 * and return the pointer.
 * 
 * @param input Input signal to be convolved
 * @param kernel Kernal to apply to the input signal
 * @return BufferPointer Pointer to buffer containing result.
 */
BufferPointer output_conv(BufferPointer input, BufferPointer kernel);
