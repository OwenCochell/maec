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

#include <cmath>
#include <deque>
#include <numbers>
#include <vector>

#include "dsp/const.hpp"

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
T iir_recursive_single(T input, C& input_container, C& output_container, D aco,
                       D bco, int apoles, int bpoles) {

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
    int asize = 0;

    /// Number of B coefficients
    int bsize = 0;

public:
    IIRFilter() = default;

    IIRFilter(int asize, int bsize) : asize(asize), bsize(bsize) {
        this->reserve();
    }

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
     * @brief Sets the number of A coefficients
     *
     * @param size New number of A coefficients
     */
    void set_asize(int size) { this->asize = size; }

    /**
     * @brief Gets the number of B coefficients
     *
     * @return Number of B coefficients
     */
    int get_bsize() const { return this->bsize; }

    /**
     * @brief Sets the number of B coefficients
     *
     * @param size New number of B coefficients
     */
    void set_bsize(int size) { this->bsize = size; }

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
     * @return Start iterator for B coefficient vector
     */
    auto bbegin() { return this->bcoes.begin(); }

    /**
     * @brief Retrieves an end iterator for the B coefficient vector
     *
     * You can use this iterator to determine when you have traversed
     * the entire A coefficient vector.
     *
     * @return auto End iterator for B coefficient vector
     */
    auto bend() { return this->bcoes.end(); }

    /**
     * @brief Sets the A coefficient at the given index
     *
     * @param index A coefficient index
     * @param value Value to set
     */
    void set_a(int index, T value) { this->acoes[index] = value; }

    /**
     * @brief Gets the A coefficient at the given index
     *
     * @param index A coefficient index
     * @return T Coefficient at the given index
     */
    T get_a(int index) const { return this->acoes[index]; }

    /**
     * @brief Sets the B coefficient at the given index
     *
     * @param index B coefficient index
     * @param value Value to set
     */
    void set_b(int index, T value) { this->bcoes[index] = value; }

    /**
     * @brief gets the B coefficient at the given index
     *
     * @param index B coefficient index
     * @return T Coefficient at the given index
     */
    T get_b(int index) const { return this->bcoes[index]; }

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
    template <typename I>
    void process(I input, int size) {

        // Loop over each value in the signal:

        for (int i = 0; i < size; ++i) {

            // Run the signal through the IIR filter:
            *(input + i) = iir_recursive_single(
                *(input + i), this->input, this->output, this->abegin(),
                this->bbegin(), this->asize, this->bsize);
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
    template <typename I, typename O>
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

/**
 * This section contains classes representing the various IIR filters.
 *
 * Each class inherits from the IIRFilter class and a base class that defines
 * some attributes all filters must implement and utilize.
 * Some filters may have some special configuration parameters,
 * so refer to the documentation for each filter for more information.
 *
 */

/**
 * @brief Class that represents an IIR filter implementation
 *
 * This class offers some base functionality that all IIR filters must
 * implement. We describe the frequency cutoffs, sample rate, and number of
 * coefficients. We also describe some methods for generating the coefficients.
 *
 * This class aims to standardize and simplify the implementation of IIR
 * filters!
 *
 * @tparam T Type of value to work with
 */
template <typename T>
class BaseIIRImplementation : public IIRFilter<T> {

private:
    /// Sample rate
    int sample_rate;

    /// Lower frequency cutoff
    double freq_high = 0.;

    /// Upper frequency cutoff
    double freq_low = 0.;

    /// Filter type
    FilterType type;

public:
    /**
     * @brief Gets the high fraction cutoff
     *
     * This determines the stop fraction for the filter,
     * and it should be represented as a fraction of the sample rate.
     *
     * @return double High fraction cutoff
     */
    double get_frac_high() const { return this->freq_high; }

    /**
     * @brief Sets the high fraction cutoff
     *
     * This determines the stop frequency for the filter,
     * and it should be represented as a fraction of the sample rate.
     *
     * @param freq New high fraction cutoff
     */
    void set_frac_high(double freq) { this->freq_high = freq; }

    /**
     * @brief Gets the low fraction cutoff
     *
     * This determines the start fraction for the filter,
     * and it should be represented as a fraction of the sample rate.
     *
     * @return double Low fraction cutoff
     */
    double get_frac_low() const { return this->freq_low; }

    /**
     * @brief Gets the low fraction cutoff
     *
     * This determines the start fraction for the filter,
     * and it should be represented as a fraction of the sample rate.
     *
     * @param freq New low fraction cutoff
     */
    void set_frac_low(double freq) { this->freq_low = freq; }

    /**
     * @brief Gets the high frequency in hertz
     *
     * The high frequency determines the stop frequency for the filter.
     *
     * @return double High frequency in hertz
     */
    double get_freq_high() const { return this->freq_high * this->sample_rate; }

    /**
     * @brief Sets the high frequency in hertz
     *
     * The high frequency determines the stop frequency for the filter.
     *
     * @param freq High frequency in hertz
     */
    void set_freq_high(double freq) {
        this->freq_high = freq / this->sample_rate;
    }

    /**
     * @brief Gets the low frequency in hertz
     *
     * The low frequency determines the start frequency for the filter.
     *
     * @return double Low frequency in hertz
     */
    double get_freq_low() const { return this->freq_low * this->sample_rate; }

    /**
     * @brief Sets the low frequency in hertz
     *
     * The low frequency determines the start frequency for the filter.
     *
     * @param freq Low frequency in hertz
     */
    void set_freq_low(double freq) {
        this->freq_low = freq / this->sample_rate;
    }

    /**
     * @brief Gets the sample rate
     *
     * @return int Sample rate
     */
    int get_sample_rate() const { return this->sample_rate; }

    /**
     * @brief Sets the sample rate
     *
     * @param rate New sample rate
     */
    void set_sample_rate(int rate) { this->sample_rate = rate; }

    /**
     * @brief Gets the filter type of this filter
     *
     * This value determines the type of filter we are using,
     * be in low pass, high pass, band pass, or band stop.
     *
     * @return FilterType Type of filter we are implementing
     */
    FilterType get_type() const { return this->type; }

    /**
     * @brief Sets the filter type of this filter
     *
     * This value determines the type of filter we are using,
     * be in low pass, high pass, band pass, or band stop.
     *
     * @param type New filter type
     */
    void set_type(FilterType type) { this->type = type; }

    /**
     * @brief Function that generates the coefficients for the filter
     *
     * This function should be used to configure and generate the necessary
     * coefficients. How these coefficients are generated depends on the type of
     * filter we are using, and will vary from filter to filter!
     *
     * Child classes should implement and call the parent function,
     * as we automatically configure and reserve the IIRFilter class.
     */
    virtual void generate_coefficients() {

        // Reserve the IIRFilter class:

        this->reserve();
    }
};

/**
 * @brief Implementation of a single pole filter
 *
 * Single pole filters are very similar to analog single resistor capacitor
 * circuits. They are pretty good at DC removal, high frequency noise
 * suppression, wave shaping and smoothing, ect. They preform pretty well in the
 * time domain, but do not preform well in the frequency domain.
 *
 * @tparam T Type to work with
 */
template <typename T>
class SinglePole : public BaseIIRImplementation<T> {

public:
    /**
     * @brief Converts a frequency fraction to a value
     *
     * This function converts a frequency fraction to a value,
     * that being the X value used to calculate the coefficients.
     * This function is used internally by the generate_coefficients() function,
     * so it is unecessary to touch this function.
     *
     * @param freq Frequency fraction to convert
     * @return double New X value
     */
    double frac_to_x(double freq) {
        return std::exp(-2 * std::numbers::pi_v<double> * freq);
    }

    /**
     * @brief Generates the coefficients for this filter
     *
     */
    void generate_coefficients() {

        // Determine if we are a low pass filter:

        if (this->get_type() == FilterType::LowPass) {

            // Reserve the sizes:

            this->set_asize(1);
            this->set_bsize(1);

            this->reserve();

            // Determine the X value:

            double xval = this->frac_to_x(this->get_freq_high());

            // Generate A & B coefficients:

            this->set_a(0, 1.0 - xval);
            this->set_b(0, xval);
        }

        // Determine if we are a high pass filter:

        if (this->get_type() == FilterType::HighPass) {

            // Reserve the sizes:

            this->set_asize(2);
            this->set_bsize(1);

            this->reserve();

            // Determine the x value:

            double xval = this->frac_to_x(this->get_freq_low());

            // Generate A & B coefficients:

            this->set_a(0, (1 + xval) / 2);
            this->set_a(1, -(1 + xval) / 2);

            this->set_b(0, this->get_freq_low());
        }
    }
};
