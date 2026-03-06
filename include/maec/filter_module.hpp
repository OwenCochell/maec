/**
 * @file filter_module.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Various modules for filtering data
 * @version 0.1
 * @date 2023-08-02
 *
 * @copyright Copyright (c) 2023
 *
 * This file contains several modules that can
 * be used to filter audio data in the chain.
 * We offer a few methods for filtering,
 * ranging from FIR filtering via convolution,
 * to recursive filtering.
 * Each implementation has it's own benefits
 * and nuances, so be sure to read up on the docs for each module!
 */

#pragma once

#include <algorithm>
#include <vector>

#include "audio_module.hpp"
#include "dsp/const.hpp"
#include "dsp/conv.hpp"
#include "dsp/kernel.hpp"

/**
 * @brief A base class for filters
 *
 * This class defines some attributes that can be
 * configured by the user.
 * Some common filtering values are provided,
 * such as the filter type and frequency.
 *
 * If the filter type in not band related,
 * then filters will ignore the stop_freq parameter.
 * For example, if you have a low pass filter,
 * then the start_freq parameter will be used for the frequency value.
 * All frequency values should be in hertz!
 *
 * By default, we set the filter type to be of LowPass,
 * and the start/stop frequency values to be 0.
 *
 */
template <typename C = void, maecm B = BaseModule*>
class BaseFilter : public AudioModule<C, B> {

private:
    /// Current filter type
    FilterType filter_type = FilterType::LowPass;

    /// Start frequency of filter
    double start_freq = 0;

    /// Stop frequency of the filter
    double stop_freq = 0;

public:
    BaseFilter() = default;

    /**
     * @brief Creates a new BaseFilter
     *
     * This is a convience method that allows developers
     * to set the basic filter parameters when constructing the object.
     *
     * @param type Filter type
     * @param startf Start frequency of filter
     * @param stopf Stop frequency of filter
     */
    BaseFilter(FilterType type, double startf, double stopf)
        : filter_type(type), start_freq(startf), stop_freq(stopf) {};

    /**
     * @brief Gets the type of this filter.
     *
     * The filter type describes the type of the filter,
     * be it low pass, high pass, band pass, band reject.
     * You can use this method to retrieve this type.
     *
     * @return FilterType Type of this filter
     */
    FilterType get_type() const { return this->filter_type; }

    /**
     * @brief Sets the type of this filter.
     *
     * As stated above, the filter type describes the type of the filter.
     * You can use this method to retrieve this type.
     *
     * @param type Type to set
     */
    void set_type(FilterType type) { this->filter_type = type; }

    /**
     * @brief Gets the start frequency
     *
     * @return double Start frequency
     */
    double get_start_freq() const { return this->start_freq; }

    /**
     * @brief Sets the start frequency
     *
     * @param freq The frequency value to set
     */
    void set_start_freq(double freq) { this->start_freq = freq; }

    /**
     * @brief Gets the stop frequency
     *
     * @return double Stop frequency
     */
    double get_stop_freq() const { return this->stop_freq; }

    /**
     * @brief Sets the stop frequency
     *
     * @param freq The frequency value to set
     */
    void set_stop_freq(double freq) { this->stop_freq = freq; }
};

/**
 * @brief A base class for filters FIR filters
 *
 * This class defines some operations that may be useful
 * for filters that utilize convolution.
 * For one, we define methods for generating filter kernels,
 * and we set the default process method to be a convolution.
 *
 */
template <typename C = void, maecm B = BaseModule*>
class BaseConvFilter : public BaseFilter<C, B> {

private:
    /// Filter kernel to utilize
    BufferPointer kernel = nullptr;

    /// Size of the filter kernel
    int size = 51;

    /// Overlap-add tail buffer: holds the last (kernel_size - 1) samples
    /// from the previous block's convolution output that must be added
    /// to the beginning of the next block's output.
    std::vector<double> overlap_tail;

public:
    /**
     * @brief Gets the size of the filter kernel
     *
     * @return int Size of filter kernel
     */
    int get_size() const { return this->size; }

    /**
     * @brief Sets the size of the filter kernel
     *
     * @param nsize New size of filter kernel.
     */
    void set_size(int nsize) { this->size = nsize; }

    /**
     * @brief Gets a pointer to the kernel
     *
     * @return AudioBuffer* AudioBuffer to contain a kernel
     */
    BufferPointer get_kernel() { return std::move(this->kernel); }

    /**
     * @brief Sets the kernel using the BufferPointer
     *
     * @param nkern New kernel to set
     */
    void set_kernel(BufferPointer nkern) { this->kernel = std::move(nkern); }

    /**
     * @brief Creates a filter kernel
     *
     * Each filter modules should use this method
     * to create the necessary filter kernel.
     * This method will be called automatically where necessary.
     *
     */
    virtual void generate_kernel() {}

    /**
     * @brief Starts this module
     *
     * We call the 'generate_kernel()' method at startup,
     * allowing for proper filtering to take place
     * once audio data is provided.
     * We also clear the overlap tail so previous state does
     * not bleed into the new session.
     *
     */
    void start() override {
        this->generate_kernel();
        this->overlap_tail.clear();
    }

    /**
     * @brief Processes incoming audio data
     *
     * We convolve the incoming audio data with the generated filter kernel
     * using the overlap-add method so block boundaries are seamless.
     *
     * Each linear convolution of an N-sample block with an M-tap kernel
     * produces N + M - 1 output samples.  The final M - 1 samples (the
     * "tail") represent the impulse-response energy that extends past the
     * end of the current block and should be summed into the beginning of
     * the next block's output.  Without this accumulation the tail is
     * silently discarded every block, creating a periodic click at the
     * block rate (~100 Hz at the default 440-sample block size) which is
     * audible as an oscillating buzz under the wanted signal.
     *
     */
    void process() override {

        // Grab the input block:

        auto ibuff = this->get_buffer();
        const std::size_t input_size = ibuff.size();
        const std::size_t kernel_size = this->kernel->size();

        // Allocate a full convolution output buffer (N + M - 1 samples)
        // zero-initialised so the overlap-add accumulation is correct:

        std::vector<double> conv_out(length_conv(input_size, kernel_size), 0.0);

        // Run the convolution:

        input_conv(ibuff.ibegin(), input_size, this->kernel->ibegin(),
                   kernel_size, conv_out.begin());

        // Overlap-add: fold the saved tail from the previous block
        // The tail holds the last (M - 1) samples from the previous full
        // convolution.  They overlap with the first (M - 1) samples of the
        // current convolution output, so we add them in place:

        const std::size_t prev_tail = this->overlap_tail.size();
        for (std::size_t i = 0; i < prev_tail; ++i) {
            conv_out[i] += this->overlap_tail[i];
        }

        // Save new tail for the next block

        this->overlap_tail.assign(conv_out.begin() + input_size,
                                  conv_out.end());

        // Trim output to exactly N samples and set on the buffer
        // Only the first input_size samples are forwarded downstream;
        // the tail will be contributed to the next block via overlap_tail.

        auto nbuff = AudioBuffer(input_size, 1);
        std::copy(conv_out.begin(), conv_out.begin() + input_size,
                  nbuff.ibegin());

        this->set_buffer(std::move(nbuff));
    }
};

template <maecm B = BaseModule*>
class SincFilter : public BaseConvFilter<SincFilter<B>, B> {

public:
    SincFilter() = default;

    /**
     * @brief Generates a sinc filter kernel
     *
     * We generate a sinc filter kernel based upon
     * the filter parameters in this class.
     *
     */
    void generate_kernel() override {
        // Determine how to generate the kernel:

        const auto type = this->get_type();
        const int final_size = this->get_size();

        const auto start_ratio = static_cast<double>(
            this->get_start_freq() / this->get_info()->sample_rate);
        const auto stop_ratio = static_cast<double>(
            this->get_stop_freq() / this->get_info()->sample_rate);

        // First, create a buffer for use:

        BufferPointer kern = std::make_unique<AudioBuffer>(this->get_size(), 1);

        // First off, just create the sinc kernel:

        sinc_kernel(start_ratio, final_size, kern->ibegin());

        // Determine if we are making a high pass filter:

        if (type == FilterType::HighPass) {

            // Do a spectral inversion to create high pass:

            spectral_inversion(kern->ibegin(), final_size);
        }

        else if (type == FilterType::BandPass ||
                 type == FilterType::BandReject) {

            // We need to create another kernel:

            AudioBuffer hkern(final_size);

            // Create low pass filter:

            sinc_kernel(stop_ratio, final_size, hkern.ibegin());

            // Create high pass filter from this kernel:

            spectral_inversion(hkern.ibegin(), final_size);

            // Add kernels together to create band-reject:

            for (int i = 0; i < final_size; ++i) {

                // Add values together:

                *(kern->ibegin() + i) =
                    *(kern->ibegin() + i) + *(hkern.ibegin() + i);
            }

            // Determine if we should invert:

            if (type == FilterType::BandPass) {

                // Invert the filter:

                spectral_inversion(kern->ibegin(), final_size);
            }
        }

        // Finally, set the kernel:

        this->set_kernel(std::move(kern));
    }
};
