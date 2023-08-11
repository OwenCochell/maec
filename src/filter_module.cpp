/**
 * @file filter_module.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Implementations for filter modules
 * @version 0.1
 * @date 2023-08-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "filter_module.hpp"

#include "dsp/kernel.hpp"
#include "dsp/conv.hpp"

FilterType BaseFilter::get_type() const {

    // Just return the type:

    return this->filter_type;
}

void BaseFilter::set_type(FilterType type) {

    // Just set the type:

    this->filter_type = type;
}

double BaseFilter::get_start_freq() const {

    // Just return start frequency:

    return this->start_freq;
}

void BaseFilter::set_start_freq(double freq) {

    // Just set the start frequency:

    this->start_freq = freq;
}

double BaseFilter::get_stop_freq() const {

    // Just return stop frequency:

    return this->stop_freq;
}

void BaseFilter::set_stop_freq(double freq) {

    // Just set the stop frequency:

    this->stop_freq = freq;
}

int BaseConvFilter::get_size() const {

    // Just return the size:

    return this->size;
}

void BaseConvFilter::set_size(int nsize) {

    // Just set the size:

    this->size = nsize;
}

BufferPointer BaseConvFilter::get_kernel() {

    // Return a pointer to the kernel:

    return std::move(this->kernel);
}

void BaseConvFilter::set_kernel(BufferPointer nkern) {

    // Just set the filter kernel:

    this->kernel = std::move(nkern);
}

void BaseConvFilter::start() {

    // Call the kernel generation method:

    this->generate_kernel();
}

void BaseConvFilter::process() {

    // Do convolution operation:

    // THIS LINE CAUSES SEG FAULT!
    // We must find a better way to share the kernel with the DSP functions.
    // After the first process, the kernel is destroyed,
    // as it was owned by the convolution function.
    // I'm thinking maybe we finally hash out the AudioInfo issues?

    // Grab the buffer:

    auto ibuff = this->get_buffer();

    // Allocate buffer for holding output:

    auto nbuff = this->create_buffer(length_conv(ibuff->size(), this->kernel->size()), 1);

    // Run though convolution function:

    input_conv(ibuff->ibegin(), ibuff->size(), this->kernel->ibegin(), this->kernel->size(), nbuff->ibegin());

    // Finally, set the buffer:

    this->set_buffer(std::move(nbuff));
}

void SincFilter::generate_kernel() {

    // Determine how to generate the kernel:

    auto type = this->get_type();
    int final_size = this->get_size() + 1;

    auto start_ratio = static_cast<double>(this->get_start_freq() / this->get_info()->sample_rate);
    auto stop_ratio = static_cast<double>(this->get_stop_freq() / this->get_info()->sample_rate);

    // First, create a buffer for use:

    BufferPointer kern =
        std::make_unique<AudioBuffer>(this->get_size(), 1);

    // First off, just create the sinc kernel:

    sinc_kernel(start_ratio, this->get_size(), kern->ibegin());

    // Determine if we are done:

    if (type == FilterType::HighPass) {

        // Do a spectral inversion to create high pass:

        spectral_inversion(kern->ibegin(), final_size);
    }

    else if (type == FilterType::BandPass || type == FilterType::BandReject) {

        // We need to create another kernel:

        AudioBuffer hkern(final_size);
        hkern.reserve(final_size);

        // Create low pass filter:

        sinc_kernel(stop_ratio, this->get_size(), hkern.ibegin());

        // Create high pass filter from this kernel:

        spectral_inversion(kern->ibegin(), final_size);

        // Add kernels together to create band-reject:

        for (int i = 0; i < final_size; ++i) {

            // Add values together:

            *(kern->ibegin() + i) = *(kern->ibegin() + i) + *(hkern.ibegin() + 1);
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
