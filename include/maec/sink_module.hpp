/**
 * @file base_output.hpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Components for outputting audio
 * @version 0.1
 * @date 2022-10-08
 * 
 * @copyright Copyright (c) 2022
 * 
 * This file contains base components for outputting audio,
 * which we call sink modules.
 * Audio sinks can be many things, such as a file, sound card, network stream, etc.
 * 
 * We aim to simplify this experience by providing frameworks for different types of output systems.
 */

#pragma once

#include <functional>
#include "audio_module.hpp"

/**
 * @brief Sink module, base class for outputting audio data
 * 
 * A sink is a component that consumes audio data,
 * allowing audio data to leave the chain.
 * How it goes about consuming audio data varies greatly,
 * but can include sending audio to a sound card, file,
 * network stream, ect.
 * These components are usually the last modules in the chain, 
 * also know as right most or forward most modules.
 * 
 * Sink modules usually have the functionality to manage the entire
 * module chain, sampling each module and outputting it
 * for you, so the user does not have to. 
 * 
 * We offer some helper methods for outputting audio data to
 * various backends.
 * 
 * The most common functionality is the concept of periods.
 * A 'period' is a chunk of audio.
 * Some backends utilize periods to split up their output buffers,
 * so instead of generating one large chunk of audio,
 * we generate many smaller chunks of audio that are then outputted.
 * We offer methods to change the period,
 * and we automatically handle it for you.
 * 
 * We should also figure out how to handle the issue of specifying
 * differing sample rates, channel numbers, ect.
 */
class SinkModule : public AudioModule {

    private:

        /// The number of periods
        int periods = 1;
 
    public:

        /**
         * @brief Construct a new Sink Module object
         * 
         * Default constructor.
         * We simply set the default squishier to squish_null
         */
        SinkModule() =default;

        /**
         * @brief Gets the periods for this sink module
         * 
         * The period is the number of times the backward modules
         * will be sampled before outputting the audio data.
         * 
         * For example, if we have a period of 3,
         * then we will sample the back modules 3 times
         * and merge them into one buffer for output.
         * To build on this example, if the module chain
         * buffer size is 440, then the final output buffer
         * will have the size of:
         * 
         * 440 * 3 = 1320
         * 
         * @return int Number of periods
         */
        int get_period() const { return periods; }

        /**
         * @brief Set the number of periods for this sink module
         * 
         * Again, the period is the number of times the backward
         * modules will be sampled before outputting the audio data.
         * 
         * @param period Number of periods
         */
        void set_period(int period) { this->periods = period; }

        /**
         * @brief Meta processing for sinks
         * 
         * We are similar to the default AudioModule meta-process() method,
         * except that we also take into account the period number,
         * and will do the necessary operations to create the full buffer.
         * 
         * By default, we represent this buffer in an interleaved format
         * 
         * TODO: Create methods to allow squished format to be choosen
         */
        void meta_process() override;
};
