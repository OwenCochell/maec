/**
 * @file module_param.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Implementations for module parameters
 * @version 0.1
 * @date 2023-02-03
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "module_param.hpp"

#include <memory>

#include "audio_buffer.hpp"
#include "base_module.hpp"
#include "meta_audio.hpp"

AudioBuffer&& ModuleParam::get() {

    // First, meta process:

    this->meta_process();

    // Next, return buffer:

    return this->get_buffer();
}

void ModuleParam::set_constant(double val) {

    // Set the underlying value:

    this->value = val;

    // Set the constant module:

    this->const_mod = std::make_unique<ConstModule>(val);

    this->link(this->const_mod.get());
}

void ModuleParam::conf_mod(BaseModule* mod) {

    // Set module as forward:

    this->forward(mod);

    // Grab configuration info and set as chain info:

    auto* chain_info = this->get_chain_info();
    auto* conf_info = mod->get_info();

    chain_info->buffer_size = conf_info->in_buffer;
    chain_info->channels = conf_info->channels;
    chain_info->sample_rate = conf_info->sample_rate;

    // Preform chain info sync:

    this->meta_info_sync();
}
