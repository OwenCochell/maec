/**
 * @file meta_audio.hpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief This file contains various meta audio modules
 * @version 0.1
 * @date 2022-09-16
 * 
 * @copyright Copyright (c) 2022
 * 
 * This file contains various 'meta modules'
 * that can be useful, particularly for debugging,
 * but are not strictly necessary for generating audio.
 * 
 */

#pragma once

#include "source_module.hpp"


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

/**
 * @brief Repeats a buffer for output
 * 
 * This module will save a buffer, and repeat it forever.
 * We do this by copying an internal buffer to a new one when processed.
 * Please keep this performance issue in mind when using this module.
 * 
 * We also set the size of the chain to match the size of this buffer.
 * TODO: Is this a good idea?
 */
class BufferModule : public SourceModule {

    private:

        /// The 'good' copy to repeat
        AudioBuffer* gbuff = nullptr;

    public:

        BufferModule() =default;

        BufferModule(AudioBuffer* ibuff) { this->set_rbuffer(ibuff); }

        /**
         * @brief Sets the buffer to repeat
         * 
         * @param ibuff Buffer to repeat
         */
        void set_rbuffer(AudioBuffer* ibuff) { this->gbuff = ibuff; this->get_info()->buff_size = ibuff->size(); }

        /**
         * @brief Gets the buffer being repeated
         * 
         * @return AudioBuffer* Buffer being repeated
         */
        AudioBuffer* get_rbuffer() const { return this->gbuff; }

        /**
         * @brief Copys the initial buffer into a new one
         * 
         * We create a new buffer to be returned, 
         * and copy the old contents into the new one.
         * 
         */
        void process();
};