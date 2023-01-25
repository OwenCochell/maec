/**
 * @file meta_audio.hpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief This file contains various meta audio modules
 * @version 0.1
 * @date 2022-09-16
 * 
 * @copyright Copyright (c) 2022
 * 
 * These modules are included for testing purposes only!
 * They are not needed to generate proper audio.
 * 
 */

#pragma once

#include "audio_module.hpp"


/**
 * @brief This module does not sample any backwards modules!
 * 
 * This module will NEVER sample any backwards modules.
 * A good use for this is a module that is at the start of a chain
 * and does not sample any other modules.
 *
 * For example, all oscillators inherit this class,
 * as they produce audio data, not alter it.
 * 
 * TODO: REPLACE THIS WITH SOURCE MODULE!
 * 
 */
class BackStop : public AudioModule {

    public:

        /**
         * @brief BackStop meta process method
         * 
         * We do the same thing as the AudioModule class,
         * except that we don't call the meta_process() 
         * method on backwards modules!
         * 
         */
        void meta_process() override;
};

/**
 * @brief Counts the number of samples this module encounters, and how many times it has been processed
 * 
 * This module counts the size of the buffer from back modules,
 * and saves it for future use.
 * We also count how many times we have been processed.
 * From there, the audio buffer can be grabbed by any forward modules.
 * 
 * This module is for testing only!
 * We don't recommend using it in production...
 * 
 */
class Counter : public AudioModule {

    private:

        /// Number of times we have been processed
        int m_processed = 0;

        /// Number of samples we have encountered
        int m_samples = 0;

    public:

        /**
         * @brief Resets this module
         * 
         * We simply set the number of processed and samples 
         * encountered to zero.
         * 
         */
        void reset() { this->m_processed = 0; this->m_samples = 0;}

        /**
         * @brief Gets the number of times we have been processed
         * 
         * @return int Number of times we have been processed
         */
        int processed() const { return this->m_processed; }

        /**
         * @brief Gets the number of samples we have encountered
         * 
         * @return int Number of samples encountered
         */
        int samples() const { return this->m_samples; }

        /**
         * @brief Counts how many samples we encounter, and how many times we have been called
         * 
         * We keep track of the info specified above.
         * 
         */
        void process() override;
};
