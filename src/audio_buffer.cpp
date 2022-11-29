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

int AudioBuffer::SeqIterator::get_channel() const {

    // Get the current channel:

    return int(this->sample / this->buff->buff[0].size());
}

void AudioBuffer::SeqIterator::set_channel(int channel) {

    // Set the current channel:

    this->set_index(channel * this->buff->buff[0].size());
}

int AudioBuffer::SeqIterator::get_position() const {

    // Get the current position:

    return int(this->sample % this->buff->buff[0].size());
}

void AudioBuffer::SeqIterator::set_position(int channel, int position) {

    // Sets the position of this iterator:

    this->set_index(channel * this->buff->buff[0].size() + position);
}

void AudioBuffer::SeqIterator::set_index(int size) {

    // First, set the index:

    this->sample = size;

    // Now, calculate the pointer:

    this->point = (this->buff->buff.at(this->get_channel()).begin() + get_position()).base();
};

void AudioBuffer::SeqIterator::set_index(int size) {

    // First, set the index:

    this->sample = size;

    // Now, calculate the pointer:

    this->point = (this->buff->buff.at(this->sample % this->buff->buff.size()).begin() + int(this->sample / 4)).base();
}
