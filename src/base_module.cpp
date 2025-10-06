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

#include "audio_buffer.hpp"

// std::unique_ptr<AudioBuffer> BaseModule::create_buffer(int channels) {

//     // Allocate the new buffer:

//     return std::make_unique<AudioBuffer>(this->info.out_buffer, channels,
//                                          this->info.sample_rate);
// }

void BaseModule::reserve() {

    // Set the buffer parameters
    // TODO: Grab channels from module info?

    buff.set_channels(info.channels);
    buff.set_samplerate(info.sample_rate);

    // Resize the buffer we are working with

    buff.resize(this->info.out_buffer * info.channels);
}

// std::unique_ptr<AudioBuffer> BaseModule::create_buffer(int size, int
// channels) {

//     // Allocate the new buffer:

//     return std::make_unique<AudioBuffer>(size, channels);
// }

void BaseModule::reserve(int size, int channels) {

    // Set the buffer parameters

    buff.set_channels(channels);

    // Resize the buffer we are working with

    buff.resize(size * channels);
}

AudioBuffer&& BaseModule::get_buffer() {

    // Return our buffer:

    return std::move(buff);
}

void BaseModule::set_buffer(AudioBuffer&& inbuff) {

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
