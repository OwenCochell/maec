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


AudioBuffer::AudioBuffer(int size, int channels) {
     
    // Create the underlying vector:

    this->buff.reserve(channels);

    // Next, create each channel and append it:

    for(int i = 0; i < channels; i++) {

        // Create an empty vector:

        AudioChannel vect;

        // Reserve the size:

        vect.reserve(size);

        // Finally, append the vector:

        this->buff.push_back(vect);
    }
}

AudioBuffer::AudioBuffer(AudioChannel vect) {

    // First, reserve the underlying buffer with a size of 1:

    this->buff.reserve(1);

    // Next, append the given vector to the buffer:

    this->buff.push_back(vect);
}

AudioBuffer::AudioBuffer(std::vector<AudioChannel> vect) {

    // Just set the underlying vector to the given one

    this->buff = vect;
}

int AudioBuffer::SeqIterator::get_channel() const {

    // Get the current channel:

    return int(this->get_index() / this->buff->buff[0].size());
}

int AudioBuffer::InterIterator::get_channel() const {

    // Get the current channel:

    return this->get_index() % this->buff->buff.size();

}

void AudioBuffer::SeqIterator::set_channel(int channel) {

    // Set the current channel:

    this->set_index(channel * this->buff->buff[0].size());
}

void AudioBuffer::InterIterator::set_sample(int sample) {

    // Set the current sample:

    this->set_index(this->buff->get_channel_count() * sample);
}

int AudioBuffer::SeqIterator::get_position() const {

    // Get the current position:

    return int(this->get_index() % this->buff->buff[0].size());

}

int AudioBuffer::InterIterator::get_sample() const {

    // Get the current position:

    return int(this->get_index() / this->buff->buff.size());

}

void AudioBuffer::SeqIterator::set_position(int channel, int position) {

    // Sets the position of this iterator:

    this->set_index(channel * this->buff->buff[0].size() + position);
}

void AudioBuffer::SeqIterator::resolve_pointer() {

    // Now, calculate the pointer:

    this->set_pointer((((this->buff->buff.begin() + this->get_channel())->begin()) + this->get_position()).base());

}

void AudioBuffer::InterIterator::resolve_pointer() {

    // Now, calculate the pointer:

    this->set_pointer((this->buff->buff.at(this->get_channel()).begin() + this->get_sample()).base());
}
