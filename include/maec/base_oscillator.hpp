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

#include "source_module.hpp"
#include "module_param.hpp"

/**
 * @brief A base class for all oscillators
 *
 * This class is the base class for all oscillators.
 * This class offers no real functionality, but is
 * used for identifying oscillators.
 * 'Oscillators' in the context of maec are components that
 * generate a periodic waveform at a given frequency.
 *
 * This class pulls a lot of configuration info
 * from the module info object,
 * so parameters such as channels, buffer size, and sample rate
 * should be altered there.
 * 
 * The frequency must be set by the user in hertz.
 * This can be done via constructor or by setters. 
 * 
 * The phase can also be configured,
 * and this should be in radians.
 */
class BaseOscillator : public SourceModule {

private:
    /// The current phase of the oscillator
    double phase = 0;

    /// Frequency Parameter
    double freq = 0;

public:
    BaseOscillator() = default;

    /**
     * @brief Construct a new Sine Oscillator object
     *
     * @param freq The frequency of the oscillator
     * @param ph The phase of the oscillator
     *
     */
    BaseOscillator(long double freq, double pha) : phase(pha), freq(freq) {}

    /**
     * @brief Construct a new Sine Oscillator object, but only define the
     * frequency
     *
     * @param freq The frequency of the oscillator
     *
     */
    BaseOscillator(long double freq) : freq(freq) {}

    /**
     * @brief Gets the frequency of this oscillator
     * 
     * @return double Frequency of oscillator in hertz
     */
    double get_frequency() const { return this->freq; }

    /**
     * @brief Sets the frequency of this oscillator
     * 
     * @param fre New frequency in hertz
     */
    void set_frequency(double fre) { this->freq = fre; }

    /**
     * @brief Gets the phase of this oscillator
     *
     * @return int Current phase
     */
    double get_phase() const { return this->phase; }

    /**
     * @brief Sets the phase of this oscillator
     *
     * @param phs Phase to set
     */
    void set_phase(double phs) { this->phase = phs; }

    /**
     * @brief Increments the phase by the provided amount
     * 
     * @param inc Value to increment phase by
     */
    void inc_phase(double inc) { this->phase += inc; }
};

class BaseModulatedOscillator : public ParamSource<1> {

    private:

        /// The current phase of the oscillator
        double phase = 0;

        /// Frequency Parameter
        ModuleParam freq;

    public:
        BaseModulatedOscillator() : ParamSource<1>(&freq) {}

        /**
         * @brief Construct a new Sine Oscillator object
         * 
         * @param freq The frequency of the oscillator
         * @param ph The phase of the oscillator
         *
         */
        BaseModulatedOscillator(long double freqv, double pha)
            : ParamSource<1>(&freq), phase(pha), freq(freqv) {}

        /**
         * @brief Construct a new Sine Oscillator object, but only define the frequency
         * 
         * @param freq The frequency of the oscillator
         * 
         */
        BaseModulatedOscillator(long double freqv)
            : ParamSource<1>(&freq), freq(freqv) {}

        /**
         * @brief Gets the frequency parameter
         * 
         * This parameter allows for modules to be attached to this oscillator,
         * allowing for the frequency to be modulated.
         * 
         * @return ModuleParameter* Frequency parameter
         */
        ModuleParam* get_frequency() { return &(this->freq); }

        /**
         * @brief Gets the phase of this oscillator
         * 
         * @return int Current phase
         */
        double get_phase() const { return this->phase; }

        /**
         * @brief Sets the phase of this oscillator
         * 
         * @param phs Phase to set
         */
        void set_phase(double phs) { this->phase = phs; }

        /**
         * @brief Increments the phase by a provided amount
         * 
         * @param inc Value to increment phase by
         */
        void inc_phase(double inc) { this->phase += inc; }
};
