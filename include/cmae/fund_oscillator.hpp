/**
 * @file fund_oscillator.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Oscillators for the fundamental waveforms
 * @version 0.1
 * @date 2022-09-25
 * 
 * @copyright Copyright (c) 2022
 * 
 * Various fundamental oscillators are defined here.
 * These oscillators can be used to create more complex waveforms.
 */

#pragma once

#include "base_oscillator.hpp"


/**
 * @brief Returns a sine wave
 * 
 * This class implements a sine wave.
 * All buffers returned will be valid sine waves.
 * 
 * By default, we pull the initial frequencies from the
 * module info object, if provided.
 * However, the frequency (among other things)
 * can be defined by the user.
 * 
 * Optionally, the frequency can be defined by the user in hertz
 * (see TODO: WRITE THIS for help getting this value).
 * The sample rate can also be defined by the user,
 * as well as the phase, which determines the start index of the sine wave.
 * 
 * The phase 
 * 
 */
class SineOscillator : public BaseOscillator {

    private:

        /// The current phase of the oscillator
        long double phase;

        /// The frequency of the oscillator
        long double frequency;

        /// The sample rate of the oscillator
        long double sample_rate;

    public:

        /**
         * @brief Construct a new Sine Oscillator object
         * 
         * If no parameters are defined, then we 
         * pull them from the AudioInfo object.
         */
        SineOscillator() : phase(0.0), frequency(0.0), sample_rate(0.0) {

            // Get the frequency from the module info object:

            this->frequency = this->get_info()->freq;

            // Get the sample rate from the module info object:

            this->sample_rate = this->get_info()->sample_rate;
        }

        /**
         * @brief Construct a new Sine Oscillator object
         * 
         * @param freq The frequency of the oscillator
         * @param sr The sample rate of the oscillator
         * @param phase The phase of the oscillator
         *
         */
        SineOscillator(long double freq, long double sr, long double phase) : frequency(freq), phase(phase), sample_rate(sr) {}

        /**
         * @brief Construct a new Sine Oscillator object, but only define the frequency
         * 
         * @param freq The frequency of the oscillator
         * 
         */
        SineOscillator(long double freq) : frequency(freq), phase(0.0), sample_rate(0.0) {

            // Get the sample rate from the module info object:

            this->sample_rate = this->get_info()->sample_rate;
        }

        /**
         * @brief Process the oscillator
         * 
         * This method will process the oscillator.
         * It will create a new buffer and fill it with
         * the sine wave.
         */
        void process() override;
};
