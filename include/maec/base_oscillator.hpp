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
 *
 * By default, we pull the initial frequencies from the
 * module info object, if provided.
 * However, the frequency (among other things)
 * can be defined by the user.
 * TODO: ^ FIX THIS ABOVE STATEMENT! IT"S BAD!
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
class BaseOscillator : public SourceModule {

protected:
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
    void set_phase(int phs) { this->phase = phs; }
};

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
 * TODO: ^ FIX THIS ABOVE STATEMENT! IT"S BAD!
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
class BaseModulatedOscillator : public ParamModule<1> {

    protected:

        /// The current phase of the oscillator
        double phase = 0;

        /// Frequency Parameter
        ModuleParam freq;

    public:
        BaseModulatedOscillator() : ParamModule<1>(&freq) {}

        /**
         * @brief Construct a new Sine Oscillator object
         * 
         * @param freq The frequency of the oscillator
         * @param ph The phase of the oscillator
         *
         */
        BaseModulatedOscillator(long double freq, double pha)
            : phase(pha), freq(freq) {}

        /**
         * @brief Construct a new Sine Oscillator object, but only define the frequency
         * 
         * @param freq The frequency of the oscillator
         * 
         */
        BaseModulatedOscillator(long double freq)
            : freq(freq) {}

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
        void set_phase(int phs) { this->phase = phs; }
};
