/**
 * @file iir.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Tools for generating and manipulating IIR filters
 * @version 0.1
 * @date 2023-08-23
 * 
 * @copyright Copyright (c) 2023
 * 
 * This file contains tools for generating and manipulating
 * IIR filter parameters,
 * as well as implementations for recursive filters
 * for an arbitrary amount of poles.
 */

#pragma once

#include <deque>

/**
 * @brief Preforms a recursive IIR filter on a single input
 * 
 * This function computes the output of a recursive IIR filter
 * for a single value!
 * For use of filtering arrays on info, you probably want to look elsewhere.
 *
 * This function uses special containers for holding the previous
 * input signals, as well as the previously computed output signals.
 * We also expect the A and B coefficients to be passed in as containers.
 * You can use whichever container you like for this,
 * but we recommend using one of container creation methods,
 * as they will create containers that are optimized for the task,
 * specifically to push/pop values from the front and back of the container.
 * 
 * @tparam T Type of input value
 * @tparam C Type of container for input and output
 * @tparam D Type of container for A and B coefficients
 * @param input Input value to compute
 * @param input_container Container holding previously encountered input values
 * @param output_container Container holding previously computed output values
 * @param aco All A coefficients
 * @param bco All B coefficients
 * @param apoles Number of A poles in the filter
 * @param bpoles Number of B poles in the filter
 * @return T Output value
 */
template <typename T, typename C, typename D>
T iir_recursive_single(T input, C& input_container, C& output_container, D aco, D bco, int apoles, int bpoles) {

    // First, we push the new input value to the front of the input container:

    input_container.push_front(input);

    // Iterate over all A poles:

    T final_value = 0;

    for (int i = 0; i < apoles; ++i) {

        // We then multiply the input value by the A coefficient,
        // and add it to the final value:

        final_value += input_container[i] * aco[i];
    }

    // Iterate over all B poles:

    for (int i = 0; i < bpoles; ++i) {

        // We then multiply the output value by the B coefficient,
        // and add it to the final value:

        final_value += output_container[i] * bco[i];
    }

    // Add final value to container:

    output_container.push_front(final_value);

    // Pop the backs of the containers:

    input_container.pop_back();
    output_container.pop_back();

    // Finally, return final value:

    return final_value;
}
