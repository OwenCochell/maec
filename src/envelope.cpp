/**
 * @file envelope.cpp
 * @author Owen Cochell (owencochell@gmail.com)
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

void DurationEnvelope::start() {

    // First, set the start time to value from timer:

    this->env->set_start_time(this->env->get_time());

    // Next, set the stop time:

    this->env->set_stop_time(this->env->get_time() + this->duration);

    // Next, start the enclosed envelope:

    this->env->start();

}

void ConstantEnvelope::process() {

    // Create a buffer for use:

    this->set_buffer(this->create_buffer());

    // Fill the buffer:

    std::fill(this->buff->sbegin(), this->buff->send(), this->get_start_value());

    // Set the time:

    this->get_timer()->add_sample(static_cast<int>(this->buff->size()));

}

void SetValue::process() {

    // Create a buffer for use:

    this->set_buffer(this->create_buffer());

    // Determine the number of values to be initial:

    auto size = this->buff->size();

    int64_t initial = std::min(this->remaining_samples(), static_cast<int64_t>(size));

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

void ChainEnvelope::add_envelope(BaseEnvelope* env) {

    // Determine if we are optimized
    // (Value exists at the end):

    if (!this->inter.empty()) {

        // Remove last value:

        this->envs.pop_back();
        this->inter.pop_back();
    }

    // Add the envelope to the collection:

    this->envs.push_back(env);

    // Optimize?

    this->optimize();

}

void ChainEnvelope::optimize() {

    // Iterate until all modules are optimized:

    int size = static_cast<int>(this->envs.size()) - 1;

    while (this->can_optimize && this->optimized < size) {

        // Determine if we need to fill in some blanks:

        auto* env = this->envs[this->optimized];

        if (env->get_stop_time() >= this->envs[this->optimized+1]->get_start_time()) {

            // Valid chain! No inserts necessary ...

            this->optimized++;

            continue;
        }

        // Otherwise, create internal envelope:

        this->create_internal(++(this->optimized));

    }

    // Finally, add envelope to the back:

    this->create_internal(static_cast<int>(this->envs.size()));

}

void ChainEnvelope::create_internal(int index) {

    // Create the InternalEnvelope:

    std::unique_ptr<InternalEnvelope> interp = std::make_unique<InternalEnvelope>();

    // Determine start value and time:

    if (index == 0) {

        // Simply use ChainTimer start value:

        interp->set_start_value(this->get_start_value());
        interp->set_start_time(0);
    }
 
    else {

        // Otherwise, use previous stop value:

        interp->set_start_value(this->envs[index-1]->get_stop_value());
        interp->set_start_time(this->envs[index-1]->get_stop_time());
    }

    // Determine stop time:

    if (index >= static_cast<int>(this->envs.size())) {

        // No next envelope, set as -1:

        interp->set_stop_time(-1);
    }

    else {

        // Just use next envelope stop time:

        interp->set_stop_time(this->envs[index]->get_start_time());
    }

    // Insert the envelope into the collection:

    this->envs.insert(this->envs.begin()+index, interp.get());

    // Add this envelope to the internal list:

    this->inter.push_back(std::move(interp));

}

void ChainEnvelope::next_envelope() {

    // Grab the next envelope:

    this->current = this->envs[++(this->env_index)];

    // Set their chain timer to ours:

    *(this->current->get_timer()) = *(this->get_timer());

}

void ChainEnvelope::process() {

    // Create a final buffer:

    auto tbuff = this->create_buffer();

    int processed = 0;

    // Iterate until buffer is full:

    while (processed < this->get_info()->out_buffer) {

        int num = this->get_info()->out_buffer - processed;

        // First, determine the number of samples current envelope will output:

        if (this->current->get_stop_time() >= 0) {

            num = std::min(num, static_cast<int>(this->current->remaining_samples()));

        }

        // Set size of current envelope to num value:

        this->current->get_info()->out_buffer = num;
 
        // Grab buffer from current envelope:

        this->current->meta_process();

        auto cbuff = this->current->get_buffer();

        // First, fill it with contents of first envelope:

        std::copy_n(cbuff->ibegin(), num, tbuff->ibegin()+processed);

        // Update the number of samples processed:

        processed += num;

        // Update the timer:

        this->get_timer()->add_sample(num);

        if (this->current->get_stop_time() >= 0 && this->get_time() >= this->current->get_stop_time()) {

            // Get a new envelope:

            this->next_envelope();
        }

    }

    // Finally, set the buffer:

    this->set_buffer(std::move(tbuff));
}

void ADSREnvelope::start() {

    // Add linear ramp for attack:

    std::unique_ptr<BaseEnvelope> att = std::make_unique<LinearRamp>();

    att->set_start_value(0);
    att->set_stop_value(1);
    att->set_start_time(0);
    att->set_stop_time(this->attack);

    this->add_envelope(att.get());
    this->envs.add_object(att);

    // Add linear ramp for decay:

    std::unique_ptr<BaseEnvelope> dec = std::make_unique<LinearRamp>();

    dec->set_start_value(1);
    dec->set_stop_value(this->sustain);
    dec->set_start_time(this->attack);
    dec->set_stop_time(this->decay);

    this->add_envelope(dec.get());
    this->envs.add_object(dec);

    // Add constant ramp for sustain:

    std::unique_ptr<BaseEnvelope> sus = std::make_unique<ConstantEnvelope>();

    sus->set_start_value(this->sustain);
    sus->set_stop_value(this->sustain);
    sus->set_start_time(this->decay);
    sus->set_stop_time(-1);

    this->add_envelope(sus.get());
    this->envs.add_object(sus);

    // 

}

void ADSREnvelope::finish() {

    // Ca
}
