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
#include <vector>

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

/**
 * @brief A class that represents an Infinite Impulse Response (IIR) filter
 * 
 * This class abstracts away some of the details involved in filtering.
 * For example, we will automatically create containers for holding
 * the previous input and output values,
 * and will manage them for you.
 * 
 * Simply provide the A and B coefficients,
 * and we will take it from there!
 * We offer methods for processing signals in place and out of place.
 */
template <typename T>
class IIRFilter {

    private:

        /// Previous input data
        std::deque<T> input;

        /// Previous output data
        std::deque<T> output;

        /// Vector of A coefficients
        std::vector<T> acoes;

        /// Vector of B coefficients
        std::vector<T> bcoes;

        /// Number of A coefficients
        int asize;

        /// Number of B coefficients
        int bsize;

    public:

        IIRFilter() =delete;

        IIRFilter(int asize, int bsize) : asize(asize), bsize(bsize) { this->reserve(); }

        /**
         * @brief Reserves components for operation.
         * 
         * We reserve the A and B coefficient vectors,
         * along with the input and output deques.
         * This function will be called automatically where necessary.
         * 
         */
        void reserve() {

            // First, reserve A and B vectors:

            this->acoes.reserve(this->asize);
            this->bcoes.reserve(this->bcoes);

            // Next, fill input and output deques:

            for (int i = 0; i < this->asize; ++i) {

                // Fill input with 0:

                this->input.push_front(0);
            }

            for (int i = 0; i < this->bsize; ++i) {

                // Fill output with 0:

                this->input.push_front(0);
            }
        }

        /**
         * @brief Gets the number of A coefficients
         *
         * @return Number of A coefficients
         */
        int get_asize() const { return this->asize; }

        /**
         * @brief Gets the number of B coefficients
         * 
         * @return Number of B coefficients
         */
        int get_bsize() const { return this->bsize; }

        /**
         * @brief Retrieves a start iterator for the A coefficient vector 
         *
         * You can use this iterator to populate the A coefficient
         * vector with your values.
         *
         * @return Start iterator for A coefficient vector
         */
        auto abegin() { return this->acoes.begin(); }

        /**
         * @brief Retrieves a end iterator for the A coefficient vector
         * 
         * You can use this iterator to determine when you have
         * traversed the entire A coefficient vector.
         * 
         * @return auto End iterator for A coefficient vector
         */
        auto aend() { return this->acoes.end(); }

        /**
         * @brief Retrieves an iterator for the B coefficient vector
         * 
         * You can use this iterator to populate the B coefficient
         * vector with your values.
         * 
         * @return Iterator for B coefficient vector
         */
        auto bbegin() { return this->bcoes.begin(); }

        auto bend() { return this->bcoes.end(); }

        /**
         * @brief Filters the given signal in place.
         * 
         * We run the signal through the IIR filter in place.
         * We utilize the the coefficients and previous
         * input/output values for this operation.
         * 
         * @tparam I Input iterator type
         * @param input Signal to process
         * @param size Size of signal
         */
        template<typename I>
        void process(I input, int size) {

            // Loop over each value in the signal:

            for (int i = 0; i < size; ++i) {

                // Run the signal through the IIR filter:
                *(input + i) = iir_recursive_single(*(input + i), this->input, this->output,
                                     this->abegin(), this->bbegin(),
                                     this->asize, this->bsize);
            }
        }

        /**
         * @brief Filters the given signal out of place.
         *
         * We run the signal through the IIR filter out of place.
         * We utilize the coefficients and the previous 
         * input/output values for this operation. 
         *
         * @tparam I Input iterator type
         * @tparam O Output iterator type
         * @param input Input iterator
         * @param size Size of signal
         * @param output Output iterator
         */
        template<typename I, typename O>
        void process(I input, int size, O output) {

            // Loop over each value in the signal:

            for (int i = 0; i < size; ++i) {

                // Run signal through IIR filter:

                *(output + i) = iir_recursive_single(
                    *(input + i), this->input, this->output, this->abegin(),
                    this->bbegin(), this->asize, this->bsize);
            }
        }
};
