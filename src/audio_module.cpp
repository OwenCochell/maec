/**
 * @file audio_module.cpp
 * @author your name (you@domain.com)
 * @brief Implementations for audio module components.
 * @version 0.1
 * @date 2022-09-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "audio_module.hpp"

#include <memory>
#include <utility>

#include "audio_buffer.hpp"
#include "base_module.hpp"

void AudioModule::meta_process() {  // NOLINT(misc-no-recursion): No recursion cycles present, valid chains will eventually end

    // Call the module behind us:

    this->get_backward()->meta_process();

    // Grab the buffer from the module behind us:

    this->set_buffer(this->get_backward()->get_buffer());

    // Call the processing module of our own:

    this->process();
}

void AudioModule::meta_start() {  // NOLINT(misc-no-recursion): No recursion cycles present, valid chains will eventually end

    // Ask the previous module to start:

    this->backward->meta_start();

    // Set our state:

    BaseModule::start();

    // Start this current module:

    this->start();
}

void AudioModule::meta_stop() {  // NOLINT(misc-no-recursion): No recursion cycles present, valid chains will eventually end

    // Yeah, just ask for previous module to stop:

    this->backward->meta_stop();

    // Set our state:

    BaseModule::stop();

    // Stop this current module:

    this->stop();
}

void AudioModule::meta_finish() {  // NOLINT(misc-no-recursion): No recursion cycles present, valid chains eventually end

    // Ask backward module to stop:

    this->backward->meta_finish();

    // Set our state:

    BaseModule::finish();

    // Stop this current module:

    this->finish();
}

void AudioModule::info_sync() {

    // By default, copy AudioInfo from front:

    this->info = *(this->forward->get_info());
}

void AudioModule::meta_info_sync() {  // NOLINT(misc-no-recursion): No recursion cycles present, valid chains eventually end

    // Preform the current info sync:

    this->info_sync();

    // Preform backwards meta sync:

    this->backward->meta_info_sync();
}

void AudioModule::done() {

    // Call parent done method:

    BaseModule::done();

    // Report to the chain that we are done:

    ++(this->chain->module_finish);
}

void AudioModule::finish() {

    // Call done immediately:

    this->done();
}

void AudioModule::set_buffer(std::unique_ptr<AudioBuffer> inbuff) {

    // Set our buffer:

    this->buff = std::move(inbuff);
}

std::unique_ptr<AudioBuffer> AudioModule::get_buffer() {

    // Return our buffer:

    return std::move(this->buff);
}

std::unique_ptr<AudioBuffer> AudioModule::create_buffer(int channels) {

    // Allocate the new buffer:

    return std::make_unique<AudioBuffer>(this->info.out_buffer, channels, this->info.sample_rate);
}

std::unique_ptr<AudioBuffer> AudioModule::create_buffer(int size, int channels) {

    // Allocate the new buffer:

    return std::make_unique<AudioBuffer>(size, channels);
}

void AudioModule::set_forward(AudioModule* mod) {

    // Set the forward module:

    this->forward = mod;
}

AudioModule* AudioModule::bind(AudioModule* mod) {

    // Simply attach the pointer to ourselves:

    this->backward = mod;

    // Set the forward module:

    mod->set_forward(this);

    // Set the chain info in the back to ours:
    // TODO: Really need to find a more robust way of doing this!
    // If a chain is added to another, previous ChainInfo pointers will not be updated!

    mod->set_chain_info(this->chain);

    return mod;
}
