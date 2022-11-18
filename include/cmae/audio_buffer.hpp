/**
 * @file audio_buffer.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Components for AudioBuffers
 * @version 0.1
 * @date 2022-11-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <vector>

/**
 * @brief Class for holding audio data
 * 
 * This class allows for arbitrary audio data to be stored,
 * worked with, and recalled for later use.
 * 
 * We will support some common buffer operations,
 * and will provide a standardized method for accessing
 * and working with audio data.
 * TODO: Add more once implemented
 * 
 * Under the hood, we keep a vector of audio data.
 * We offer some methods for accessing this data via iterators
 * 
 * Finally, this class contains some info about the audio data itself,
 * such as sample rate, format (TODO: Figure this out),
 * number of samples, ect.
 * 
 */
class AudioBuffer {

    private:

        /// The underlying vector of audio data
        std::vector<long double> buff;

        /// Sample rate in Hertz
        double sample_rate = 4400;

    public:

        /**
         * @brief Construct a new Audio Buffer object
         * 
         */
        AudioBuffer() {}

        /**
         * @brief Construct a new Audio Buffer object and preallocates it
         * 
         * @param size Size of the buffer
         */
        AudioBuffer(int size);

        /**
         * @brief Set the Sample Rate of this buffer
         * 
         * Sets the sample rate for this AudioBuffer.
         * 
         * Please note, we will not be doing checks of any kind
         * to ensure this is accurate!
         * Any component using this buffer will most likely 
         * use this sample rate at face value.
         * With tha t being said,
         * please only set the sample rate to accurate values.
         * If your component has nothing to do with
         * resampling, then please do not change this value.
         * 
         * @param rate New sample rate to set
         */
        void set_samplerate(double rate) { this->sample_rate = rate; }

        /**
         * @brief Get the samplerate of this buffer
         * 
         * Returns the reported sample rate of this buffer.
         * It can be assumed that the reported sample rate 
         * is accurate to the audio buffer.
         * 
         * @return double The current samplerate
         */
        double get_samplerate() { return this->sample_rate; }

        /**
         * @brief Pre-allocates the buffer to a certain size
         * 
         * This tells the underlying vector to allocate memory
         * before hand, which leads to faster performance
         * when writing (and reading, but to a lesser degree...),
         * so it is recommended to allocate the vector before any operations.
         * 
         * See: https://cplusplus.com/reference/vector/vector/reserve/
         * For more info.
         * 
         * @param size Size to pre-allocate
         */
        void reserve(int size) { this->buff.reserve(size); }

        /**
         * @brief Shrinks the vector to it's current size
         * 
         * This tells the underlying vector to un-allocate 
         * 
         */
        void shrink() { this->buff.shrink_to_fit(); }
};