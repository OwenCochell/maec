/**
 * @file meta_audio.hpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Implementations of meta audio modules
 * @version 0.1
 * @date 2022-09-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <cmath>

#include "meta_audio.hpp"

void Counter::process() {

    // Increment the processed value:

    this->m_processed++;

    // Increment the number of samples encountered:

    this->m_samples += static_cast<int>(this->buff->size()) * this->buff->get_channel_count();
}

void LatencyModule::reset() {

    // Clear values:

    this->operation_time = 0;
    this->operation_latency = 0;
    this->total_operation_latency = 0;
    this->total_operation_time = 0;

    // Reset chain timer:

    this->timer.reset();

    // Set the time to now:

    this->start_time = get_time();
}

void LatencyModule::meta_process() {

    // Get initial time:

    int64_t start = get_time();

    // Call the module behind us:

    this->get_backward()->meta_process();

    // Get stop time:

    int64_t stop = get_time();

    // Grab the buffer from the module behind us:

    this->set_buffer(this->get_backward()->get_buffer());

    // Update the counter statistics:

    Counter::process();

    // Update chain timer:

    this->timer.set_samplerate(static_cast<int>(this->buff->get_samplerate()));
    this->timer.set_channels(this->buff->get_channel_count());
    this->timer.add_sample(static_cast<int>(this->buff->size() * this->buff->get_channel_count()));

    // Save the time:

    this->operation_time = stop - start;

    // Add to the total:

    this->total_operation_time += this->operation_time;

    // Determine the latency:

    this->operation_latency = this->operation_time - this->timer.get_time();

    // Add to total latency:

    this->total_operation_latency += this->operation_latency;

    // Call the processing module of our own:

    this->process();

}

void BufferModule::process() {

    // First, create a buffer to use:

    this->set_buffer(this->create_buffer());

    // Next, fill it using the contents of our old buffer:

    std::copy(this->gbuff->ibegin(), this->gbuff->iend(), this->buff->ibegin());

}

void UniformBuffer::process() {

    // Create new empty buffer:

    this->set_buffer(this->create_buffer());

    while (this->index < this->get_info()->out_buffer) {

        // Determine if in buffer is out of values:

        if (this->ibuff == nullptr || this->iindex >= static_cast<int>(this->ibuff->size())) {

            // The current in buffer is done, grab a new one:

            // Meta process back module:

            this->get_backward()->meta_process();

            // Save the buffer:

            this->ibuff = this->get_backward()->get_buffer();

            // Update the in index:

            this->iindex = 0;

        }

        // Determine the number of samples yet to fill:

        int remaining = std::min(this->get_info()->out_buffer - this->index, static_cast<int>(this->ibuff->size()) - this->iindex);

        // Fill the current buffer with this value:

        std::copy_n(this->ibuff->ibegin(), remaining, this->buff->ibegin()+this->index);

        // Update values and move on:

        this->iindex += remaining;
        this->index += remaining;

    }

    // Finally, set our index back to zero:

    this->index = 0;

}
