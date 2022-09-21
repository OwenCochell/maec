/**
 * @file module_mixer.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Implementation for module mixers
 * @version 0.1
 * @date 2022-09-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <algorithm>
#include "module_mixer.hpp"


void ModuleMixDown::meta_process() {

    // Iterate over each module in our list:

    for (auto &mod : this->in) {

        // Call the processing module of each:

        mod->process();

        // Add the buffer to our own:

        this->buffs.push_back(std::move(mod->get_buffer()));

    }

}

void ModuleMixDown::bind(AudioModule* mod) {

    // Add the incoming module to our collection:

    this->in.push_back(mod);
}

void ModuleMixDown::set_buffer(AudioBuffer inbuff) {

    // Add the incoming buffer to our vector:

    this->buffs.push_back(std::move(inbuff));
}

void ModuleMixUp::set_forward(AudioModule* mod) {

    // Add the forward module to our vector:

    this->out.push_back(mod);
}

AudioBuffer ModuleMixUp::get_buffer() {

    // Get a buffer to work with:

    AudioBuffer tbuff = this->create_buffer();

    // Copy the contents:

    std::copy(this->buff->begin(), this->buff->end(), tbuff->begin());

    // Finally, return the buffer:

    return std::move(tbuff);
}
