/**
 * @file audio_buffer.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Implementations for AudioBuffers
 * @version 0.1
 * @date 2022-11-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "audio_buffer.hpp"

AudioBuffer::AudioBuffer(int size) {

    // Pre-allocate the buffer to a given size:

    this->reserve(size);
}

long double AudioBuffer::SeqIterator::operator *() const {

    // Get the current value:

    return this->buff->buff.at(this->get_channel()).at(get_position());
}

int AudioBuffer::SeqIterator::get_channel() const {

    // Get the current channel:

    return int(this->sample / this->buff->buff[0].size());
}

void AudioBuffer::SeqIterator::set_channel(int channel) {

    // Set the current channel:

    this->sample = channel * this->buff->buff[0].size();
}

int AudioBuffer::SeqIterator::get_position() const {

    // Get the current position:

    return int(this->sample % this->buff->buff[0].size());
}

void AudioBuffer::SeqIterator::set_position(int channel, int position) {

    // Sets the position of this iterator:

    this->sample = channel * this->buff->buff[0].size() + position;
}

long double AudioBuffer::InterIterator::operator *() const {

    // Get the current value:

    return this->buff->buff.at(this->sample % this->buff->buff.size()).at(int(this->sample / 4));
}
