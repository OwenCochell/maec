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


void AudioModule::meta_process() {

    // Call the module behind us:

    this->get_backward()->meta_process();

    // Grab the buffer from the module behind us:

    this->set_buffer(std::move(this->get_backward()->get_buffer()));

    // Call the processing module of our own:

    this->process();
}

void AudioModule::set_buffer(AudioBuffer inbuff) {

    // Set our buffer:

    this->buff = std::move(inbuff);
}

AudioBuffer AudioModule::get_buffer() {

    // Return our buffer:

    return std::move(this->buff);
}

AudioBuffer AudioModule::create_buffer() {

    // Allocate the new buffer:

    return std::make_unique<std::vector<long double>>(this->info->buff_size);
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

    return mod;
}
