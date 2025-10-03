/**
 * @file base_module.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Implementations for base modules
 * @version 0.1
 * @date 2022-09-04
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "base_module.hpp"

std::unique_ptr<AudioBuffer> BaseModule::create_buffer(int channels) {

    // Allocate the new buffer:

    return std::make_unique<AudioBuffer>(this->info.out_buffer, channels,
                                         this->info.sample_rate);
}

std::unique_ptr<AudioBuffer> BaseModule::create_buffer(int size, int channels) {

    // Allocate the new buffer:

    return std::make_unique<AudioBuffer>(size, channels);
}

std::unique_ptr<AudioBuffer> BaseModule::get_buffer() {

    // Return our buffer:

    return std::move(this->buff);
}

void BaseModule::set_buffer(std::unique_ptr<AudioBuffer> inbuff) {

    // Set our buffer:

    this->buff = std::move(inbuff);
}

void BaseModule::start() {
    // Set the state to started:
    this->state_type = State::Started;
}

void BaseModule::stop() {
    // Set the state to stopped:
    this->state_type = State::Stopped;
}

void BaseModule::finish() {
    // Set the state to finishing:
    this->state_type = State::Finishing;
}

void BaseModule::done() {
    // Set the state to done:
    this->state_type = State::Finished;

    // Report to the chain that we are done:

    ++(this->chain->module_finish);
}
