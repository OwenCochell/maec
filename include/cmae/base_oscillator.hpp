/**
 * @file base_oscillator.hpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief This file contains base oscillator classes and tools
 * @version 0.1
 * @date 2022-09-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include "meta_audio.hpp"

 
/**
 * @brief A base class for all oscillators
 * 
 * This class is the base class for all oscillators.
 * This class offers no real functionality, but is
 * used for identifying oscillators.
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
 * The phase can also be set, which determines the offset of the frequency.
 * So, for example, you can set the index to 10 instead of starting at 0.
 * We determine the index at a given time by dividing by the sampling rate.
 */
class BaseOscillator : public BackStop {

    protected:

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
        BaseOscillator() {

            // Get the frequency from the module info object:

            this->frequency = this->get_info()->freq;

            // Get the sample rate from the module info object:

            this->sample_rate = this->get_info()->sample_rate;

            // Set the phase:

            this->phase = 0;
        }

        /**
         * @brief Construct a new Sine Oscillator object
         * 
         * @param freq The frequency of the oscillator
         * @param sr The sample rate of the oscillator
         * @param ph The phase of the oscillator
         *
         */
        BaseOscillator(long double freq, long double sr, long double ph) {

            // Set the frequency:

            this->frequency = freq;

            // Set the sampling rate:

            this->sample_rate = sr;

            // Set the phase:

            this->phase = ph;
        }

        /**
         * @brief Construct a new Sine Oscillator object, but only define the frequency
         * 
         * @param freq The frequency of the oscillator
         * 
         */
        BaseOscillator(long double freq) {

            // Get the sample rate from the module info object:

            this->sample_rate = this->get_info()->sample_rate;

            // Set the frequency:

            this->frequency = freq;

            // Set the phase:

            this->phase = 0;

        }

        /**
         * @brief Gets the frequency of this oscillator
         * 
         * @return long double Frequency in Hertz
         */
        long double get_frequency() { return this->frequency; }

};


/**
 * @brief Simply returns the same double on each iteration
 * 
 * We will always create a buffer containing one number
 * for processing.
 * You can set this number with the set_value() method,
 * or by passing a double to the class constructor.
 * Otherwise, the default value is 0.0.
 * 
 */
class ConstantOscillator : public BaseOscillator {

    private:

        /// The value to return
        long double value;

    public:

        /**
         * @brief Construct a new Constant Oscillator object
         * 
         * By default, we set the value to 0.0
         * 
         */
        ConstantOscillator() : value(0.0) {}

        /**
         * @brief Construct a new Constant Oscillator object
         * 
         * @param invalue Value to use in buffer creation
         */
        ConstantOscillator(long double invalue) : value(invalue) {}

        /**
         * @brief Set the value to use in buffer creation
         * 
         * @param invalue Value to use in buffer creation
         */
        void set_value(long double invalue) { this->value = invalue; }

        /**
         * @brief Get the value object
         * 
         * We return the value that will be used in buffer creation.
         * 
         * @return long double 
         */
        long double get_value() { return this->value; }

        /**
         * @brief Override process method
         * 
         * We will fill a buffer filled with the given value.
         * 
         */
        void process() override;

};
