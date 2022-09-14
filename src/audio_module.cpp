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

    this->backward->meta_process();

    // Call the processing module of our own:

    this->process();

    // Finally, send our buffer to the module we are attached to:

    this->forward->set_buffer(std::move(this->buff));
}

void AudioModule::set_buffer(std::unique_ptr<long double> inbuff) {

    // Set our buffer:

    this->buff = std::move(inbuff);
}

void AudioModule::bind(AudioModule* mod) {

    // Simply attach the pointer to ourselves:

    this->backward = mod;

    // Set the forward module:

    mod->set_forward(this);
}

std::unique_ptr<long double> AudioModule::create_buffer() {

    // Allocate the new buffer with our buffsize:

    return std::unique_ptr<long double> (new long double[this->info->buff_size]);
}
