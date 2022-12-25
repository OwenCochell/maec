/**
 * @file sink_module.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Implementations for sink modules
 * @version 0.1
 * @date 2022-12-23
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "sink_module.hpp"

void SinkModule::meta_process() {

    // First, process the backwards modules:

    this->get_backward()->meta_process();

    // Next, grab the buffer:

    std::unique_ptr<AudioBuffer> temp = std::move(this->get_backward()->get_buffer());

    // Create a buffer for storing data, and ensure the size is correct:

    this->buff = std::make_unique<AudioBuffer>(temp->get_channel_count() * temp->size() * this->get_period(), 1);

    // Iterate over each period:

    for(int i = 1; i < this->periods - 1; ++i) {

        // Call the module behind us:

        this->get_backward()->meta_process();

        // Grab the buffer from the module behind us:

        temp = std::move(this->get_backward()->get_buffer());

        // Copy this value to the buffer:

        std::copy(temp->ibegin(), temp->iend(), buff->ibegin() + (int)(i * temp->size() * temp->get_channel_count()));

    }

    // Call the processing module of our own:

    this->process();

}
