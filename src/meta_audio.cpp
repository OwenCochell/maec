/**
 * @file meta_audio.hpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Implementations of meta audio modules
 * @version 0.1
 * @date 2022-09-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "meta_audio.hpp"

void Counter::process() {

    // Increment the processed value:

    this->m_processed++;

    // Increment the number of samples encountered:

    this->m_samples += this->buff->size() * this->buff->get_channel_count();
}

void BufferModule::process() {

    // First, create a buffer to use:

    this->set_buffer(this->create_buffer());

    // Next, fill it using the contents of our old buffer:

    std::copy(this->gbuff->ibegin(), this->gbuff->iend(), this->buff->ibegin());
}
