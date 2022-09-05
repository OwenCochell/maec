/**
 * @file audio_module.hpp
 * @author your name (you@domain.com)
 * @brief This file contains module components for working with audio data.
 * @version 0.1
 * @date 2022-09-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include "base_module.hpp"

/**
 * @brief Structure for holding audio info
 * 
 * We contain various info to be shared among modules
 * that are linked together.
 */
struct audio_info {

    double freq;  /// The frequency of the audio data, if applicable
    double sample_rate;  /// The sample rate of the audio data, if applicable

    int done;  /// Number of modules that are finished
    int num;  /// Number of modules present in the chain
    int velocity;  /// Velocity of the audio data, if applicable
    int buff_size;  /// The size of the audio buffer

    bool running;  // Value determining if we are running

};


/**
 * @brief Module for working with audio data
 * 
 * These modules are designed to work with audio data.
 * Audio modules can be attached to each other to form a chain,
 * and audio data will flow them as they are linked.
 * 
 * Audio modules are share a pointer to an 'audio_info' struct,
 * which will be used to share information between modules.
 */
class AudioModule : public BaseModule {

    private:

        /// Index of this module:

        int index = 0;

        /// Pointer to the audio info:

        audio_info* info;

    public:

        /**
         * @brief Function called when processing is necessary.
         * 
         * This is where the magic happens!
         * Any time audio processing is required,
         * this method will be called.
         * 
         * This can do anything from generating audio data,
         * to taking audio data from previous modules and working with it.
         */
        virtual void process();

        /**
         * @brief Get the buffer object
         * 
         * This method samples the attached modules for an audio buffer.
         * We return a pointer to the array of audio data.
         * This array contains a set number of samples
         * in the form of a long double.
         * 
         * @return long double* 
         */
        (long double)* get_buffer();
};
