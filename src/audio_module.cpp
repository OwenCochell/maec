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

void AudioModule::meta_process() {  // NOLINT(misc-no-recursion): No recursion cycles present, chain will eventually end

    // Call the module behind us:

    this->get_backward()->meta_process();

    // Grab the buffer from the module behind us:

    this->set_buffer(this->get_backward()->get_buffer());

    // Call the processing module of our own:

    this->process();
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

    return std::make_unique<AudioBuffer>(this->info.out_buffer, channels);
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
