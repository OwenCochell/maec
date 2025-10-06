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

#include "audio_buffer.hpp"
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

    // Call the parent start method

    AudioModule<>::start();

    // Call user defined start method

    this->start();
}

void ModuleMixDown::meta_stop() {

    // Iterate over each module in the collection

    for (auto* mod : this->in) {

        // Call the meta stop of each

        mod->meta_stop();
    }

    // Call the parent start method

    AudioModule<>::stop();

    // Call the user defined stop method

    this->stop();
}

void ModuleMixDown::meta_finish() {

    // Iterate over each module in the collection

    for (auto* mod : this->in) {

        // Call the meta finish of each

        mod->meta_finish();
    }

    // Call the user defined finish method

    this->finish();
}

void ModuleMixDown::meta_info_sync() {

    // Preform the current info sync

    this->info_sync();

    // Iterate over each module in the collection

    for (auto* mod : this->in) {

        // Call the info sync of each

        mod->meta_info_sync();
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

    auto fbuff =
        AudioBuffer(this->get_info()->out_buffer, this->get_info()->channels,
                    this->get_info()->sample_rate);

    // Iterate over each buffer:

    for (auto& b : this->buffs) {

        // Iterate over each value in the buffer:

        for (auto iter = b.sbegin();
             static_cast<unsigned int>(iter.get_index()) < b.size(); ++iter) {

            *(fbuff.sbegin() + iter) += *iter;
        }
    }

    // Set our buffer to the new buffer:

    this->set_buffer(std::move(fbuff));
}

void ModuleMixUp::forward(BaseModule* mod) {

    // Add the forward module to our vector:

    this->out.push_back(mod);
}

AudioBuffer&& ModuleMixUp::get_buffer() {

    // Configure the temporary buffer

    tbuff.set_channels(this->get_info()->channels);
    tbuff.set_samplerate(this->get_info()->sample_rate);

    tbuff.resize(this->get_info()->out_buffer);

    std::copy(this->buff.sbegin(), this->buff.send(), tbuff.sbegin());

    // Finally, return the buffer:

    return std::move(tbuff);
}
