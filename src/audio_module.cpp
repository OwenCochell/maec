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

    this->forward->set_buffer(this->buff);
}

void AudioModule::set_buffer(long double* inbuff) {

    // Set our buffer:

    this->buff = inbuff;
}

void AudioModule::bind(AudioModule* mod) {

    // Simply attach the pointer to ourselves:

    this->backward = mod;

    // Set the forward module:

    mod->set_forward(this);
}

long double* AudioModule::get_buffer() {

    // Return the pointer to our audio buffer:

    return this->buff;
}
