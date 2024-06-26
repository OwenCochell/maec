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
 * We contain sine, square, saw, triangle, and sawtooth oscillators.
 */

#pragma once

#include "base_oscillator.hpp"

/**
 * @brief Sine wave oscillator
 * 
 * This class implements a sine wave.
 * All buffers returned will be valid sine waves.
 * 
 */
class SineOscillator : public BaseOscillator {

    using BaseOscillator::BaseOscillator;

    public:

        /**
         * @brief Process the oscillator
         * 
         * This method will process the oscillator.
         * It will create a new buffer and fill it with
         * the sine wave.
         */
        void process() override;
};

/**
 * @brief Square wave oscillator
 * 
 * This class implements a square wave.
 * All buffers returned will be valid square waves.
 */
class SquareOscillator : public BaseOscillator {

    using BaseOscillator::BaseOscillator;

    public:

        /**
         * @brief Process the oscillator
         * 
         * This method will process the oscillator.
         * It will create a new buffer and fill it with
         * the square wave.
         */
        void process() override;
};

/**
 * @brief Sawtooth oscillator
 * 
 * This class implements a sawtooth wave.
 * All buffers returned will be valid sawtooth waves.
 */
class SawtoothOscillator : public BaseOscillator {

    using BaseOscillator::BaseOscillator;

    public:

        /**
         * @brief Process the oscillator
         * 
         * This method will process the oscillator.
         * It will create a new buffer and fill it with
         * the sawtooth wave.
         */
        void process() override;
};

/**
 * @brief Triangle oscillator
 * 
 * This class implements a triangle wave.
 * All buffers returned will be valid triangle waves.
 */
class TriangleOscillator : public BaseOscillator {

    using BaseOscillator::BaseOscillator;

    public:

        /**
         * @brief Process the oscillator
         * 
         * This method will process the oscillator.
         * It will create a new buffer and fill it with
         * the triangle wave.
         */
        void process() override;
};

/**
 * @brief Modulated sine wave oscillator
 *
 * This class implements a sine wave.
 * All buffers returned will be valid sine waves.
 *
 * This version can have it's frequency modulated.
 *
 */
class ModSineOscillator : public BaseModulatedOscillator {

    using BaseModulatedOscillator::BaseModulatedOscillator;

public:
    /**
     * @brief Process the oscillator
     *
     * This method will process the oscillator.
     * It will create a new buffer and fill it with
     * the sine wave.
     */
    void process() override;
};

/**
 * @brief Modulated square wave oscillator
 *
 * This class implements a square wave.
 * All buffers returned will be valid square waves.
 * 
 * This version can have it's frequency modulated.
 */
class ModSquareOscillator : public BaseModulatedOscillator {

    using BaseModulatedOscillator::BaseModulatedOscillator;

public:
    /**
     * @brief Process the oscillator
     *
     * This method will process the oscillator.
     * It will create a new buffer and fill it with
     * the square wave.
     */
    void process() override;
};

/**
 * @brief Modulated sawtooth oscillator
 *
 * This class implements a sawtooth wave.
 * All buffers returned will be valid sawtooth waves.
 * 
 * This version can have it's frequency modulated.
 */
class ModSawtoothOscillator : public BaseModulatedOscillator {

    using BaseModulatedOscillator::BaseModulatedOscillator;

public:
    /**
     * @brief Process the oscillator
     *
     * This method will process the oscillator.
     * It will create a new buffer and fill it with
     * the sawtooth wave.
     */
    void process() override;
};

/**
 * @brief Module triangle oscillator
 *
 * This class implements a triangle wave.
 * All buffers returned will be valid triangle waves.
 *
 * This version can have it's frequency modulated.
 */
class ModTriangleOscillator : public BaseModulatedOscillator {

    using BaseModulatedOscillator::BaseModulatedOscillator;

public:
    /**
     * @brief Process the oscillator
     *
     * This method will process the oscillator.
     * It will create a new buffer and fill it with
     * the triangle wave.
     */
    void process() override;
};
