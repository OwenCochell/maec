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

#include "module_mixer.hpp"

#include <algorithm>

#include "audio_module.hpp"
#include "base_module.hpp"

void ModuleMixDown::meta_process() {

    // Iterate over each module in our list:

    for (auto& mod : this->in) {

        // Call the processing module of each:

        mod->process();

        // Add the buffer to our own:

        this->buffs.push_back(std::move(mod->get_buffer()));
    }

    // Finally, call our processing method:

    this->process();
}

void ModuleMixDown::meta_start() {

    // Iterate over each module in the collection

    for (auto* mod : this->in) {

        // Call the meta start of each

        mod->meta_start();
    }
}

void ModuleMixDown::meta_stop() {

    // Iterate over each module in the collection

    for (auto* mod : this->in) {

        // Call the meta stop of each

        mod->meta_stop();
    }
}

void ModuleMixDown::meta_finish() {

    // Iterate over each module in the collection

    for (auto* mod : this->in) {

        // Call the meta finish of each

        mod->meta_finish();
    }
}

AudioModule<>::BV* ModuleMixDown::link(AudioModule<>::BT mod) {

    // Add the incoming module to our collection:

    this->in.push_back(mod);

    // Set the forward module

    mod->forward(this);

    // Sync the chain info
    // TODO: See AudioModule for notes on chain info syncing

    mod->set_chain_info(this->get_chain_info());

    return mod;
}

void ModuleMixDown::process() {

    // Create a new buffer:

    std::unique_ptr<AudioBuffer> fbuff = create_buffer();

    // Iterate over each buffer:

    for (auto& b : this->buffs) {

        // Iterate over each value in the buffer:

        for (auto iter = b->sbegin();
             (unsigned int)iter.get_index() < b->size(); ++iter) {

            *(fbuff->sbegin() + iter) += *iter;
        }
    }

    // Set our buffer to the new buffer:

    this->set_buffer(std::move(fbuff));
}

void ModuleMixUp::forward(BaseModule* mod) {

    // Add the forward module to our vector:

    this->out.push_back(mod);
}

std::unique_ptr<AudioBuffer> ModuleMixUp::get_buffer() {

    // Get a buffer to work with:

    std::unique_ptr<AudioBuffer> tbuff = this->create_buffer();

    std::copy(this->buff->sbegin(), this->buff->send(), tbuff->sbegin());

    // Finally, return the buffer:

    return tbuff;
}
