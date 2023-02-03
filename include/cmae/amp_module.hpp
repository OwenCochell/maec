/**
 * @file amp_module.hpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Components for altering amplitude 
 * @version 0.1
 * @date 2023-02-02
 * 
 * @copyright Copyright (c) 2023
 * 
 * This file contains components for altering the amplitude of audio data. 
 * 
 */

#pragma once

#include "audio_module.hpp"

/**
 * @brief Base class for amplitude modules
 * 
 * This class defines some components for amplitude modules.
 * We define a value that can be used for scaling,
 * as well as methods for getting and setting this value. 
 * 
 */
class BaseAmplitude : public AudioModule {

    private:

        /// Value to scale the data:
        double value = 1;

    public:

        BaseAmplitude() =default;

        /**
         * @brief Construct a new Amplitude Scale object
         * 
         * We offer the ability to set the scale value.
         * This can be useful if you know the amplitude at creation time.
         * 
         * @param val Value to scale the input by
         */
        BaseAmplitude(double val) { this->value = val; }

        /**
         * @brief Gets the current value
         * 
         * Gets the value that is used for scaling.
         * 
         * @return double Current value
         */
        double get_value() const { return this->value; }

        /**
         * @brief Set the scaling value
         * 
         * Sets the value that will be used for scaling. 
         * 
         * @param val Value to be used
         */
        void set_value(double val) { this->value = val; }

};

/**
 * @brief Scales the amplitude by a given value.
 * 
 * We multiply the incoming audio data by the given value.
 * 
 * For example, if the value provided is '0.5',
 * then amplitude of the outgoing data will be cut in half.
 * If we are working with sample of magnitude '1',
 * then after processing will be '0.5'.
 * 
 */
class AmplitudeScale : public BaseAmplitude {

    public:

        using BaseAmplitude::BaseAmplitude;

        /**
         * @brief Scale the incoming audio data
         * 
         * We multiply the incoming buffer by the given value.
         * 
         */
        void process() override;
};

/**
 * @brief Adds the amplitude to the audio data
 * 
 * We add the amplitude to the incoming audio data.
 * 
 * For example, if the value provided is '0.25',
 * then the amplitude of the outgoing audio data will be increased by '0.25'.
 * If we are working with sample of magnitude '1',
 * then after processing will '1.25'.
 * 
 */
class AmplitudeAdd : public BaseAmplitude {

    public:

        using BaseAmplitude::BaseAmplitude;

        /**
         * @brief Add the value to the incoming audio data
         * 
         * We add our value to the incoming audio data.
         * 
         */
        void process() override;
};
