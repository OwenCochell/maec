/**
 * @file envelope.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Implementations of envelopes
 * @version 0.1
 * @date 2023-02-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <cmath>

#include "envelope.hpp"

int64_t BaseEnvelope::get_time_inc() {

    // First, get the time:

    int64_t time = this->timer.get_time();

    // Next, increment the sample:

    this->timer.inc_sample();

    // Return the current time:

    return time;

}

void ConstantEnvelope::process() {

    // Create a buffer for use:

    this->set_buffer(this->create_buffer());

    // Fill the buffer:

    std::fill(this->buff->sbegin(), this->buff->send(), this->get_start_value());

}

void SetValue::process() {

    // Create a buffer for use:

    this->set_buffer(this->create_buffer());

    // Determine the number of values to be initial:

    auto size = this->buff->size();

    int64_t initial = std::min((this->get_stop_time() - this->get_timer()->get_time()) / this->get_timer()->get_npf(), static_cast<int64_t>(size));

    // Determine the number of values to be final:

    int64_t after = size - initial;

    // Fill in the buffer:

    std::fill_n(this->buff->ibegin(), initial, this->get_start_value());

    std::fill_n(this->buff->ibegin() + initial, after, this->get_stop_value());

    // Set the time:

    this->get_timer()->add_sample(static_cast<int>(size));

}

void ExponentialRamp::process() {

    // Create a buffer for use:

    this->set_buffer(this->create_buffer());

    // Iterate over values in buffer:

    for (auto iter = this->buff->ibegin(); static_cast<unsigned int>(iter.get_index()) < this->buff->size(); ++iter) {

        // Determine value at current time:

        *iter = this->get_start_value() * std::pow<long double, long double>(this->val_divide(), static_cast<long double>(this->get_time_inc() - this->get_start_time()) / static_cast<long double>(this->time_diff()));
    }

}

void LinearRamp::process() {

    // Create a buffer for use:

    this->set_buffer(this->create_buffer());

    // Iterate over values in buffer:

    for (auto iter = this->buff->ibegin(); static_cast<unsigned int>(iter.get_index()) < this->buff->size(); ++iter) {

        // Determine value at current time:

        *iter = this->get_start_value() + this->val_diff() * (static_cast<long double>(this->get_time_inc() - this->get_start_time()) / static_cast<long double>(this->time_diff()));
    }

}
