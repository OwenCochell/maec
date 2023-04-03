/**
 * @file module_param.hpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Components for module parameters
 * @version 0.1
 * @date 2023-02-03
 * 
 * @copyright Copyright (c) 2023
 * 
 * This file contains components for module parameters,
 * which allow for inter-module connections.
 */

#pragma once

#include <functional>

#include "audio_module.hpp"

/**
 * @brief A component that represents a parameter
 * 
 * A ModuleParameter is a component that represents a parameter to be set and accessed.
 * The value of this parameter can be used for many things,
 * and often times modules will utilize this component.
 * For example, an oscillator may use this component for the frequency.
 * 
 * The value of this parameter can differ greatly!
 * Here are some values that we support:
 * 
 * - Static values that do not change
 * - Output from other modules
 * - Custom methods defined by users
 * 
 * These parameters offer a modular framework for defining sources for values.
 * Users can define a function that will be called each time we require a value.
 * We also offer some methods for simplifying configuration.
 * Using the proper methods, we can auto-configure this class for getting values
 * from an AudioModule.
 * 
 * One possible use of this component is to offer the ability for parameters to change over time.
 * For example, one could attach a SineOscillator, which would cause the this parameter to represent a sine wave.
 * This allows for components to be modulated.
 */
class ModuleParameter {

    using value_func = std::function<long double(ModuleParameter*)>;

    private:

        /// Function to retrieve values
        value_func func = nullptr;

        /// A constant value to be returned
        long double value = 0;

        /// An AudioBuffer holding the current audio data:
        std::unique_ptr<AudioBuffer> buff = nullptr;

        /// The current AudioModule we are working with:
        AudioModule* mod = nullptr;

        /// The current iterator:
        AudioBuffer::InterIterator<long double> siter;

        /// The current end iterator:
        AudioBuffer::InterIterator<long double> eiter;

        /**
         * @brief A friend function to get constant value
         * 
         * We set this as the value function
         * if we are optimized for returning a constant value.
         * 
         * @param mod ModuleParameter to get value from
         * @return long double Current value
         */
        friend long double get_constant(ModuleParameter* mod);

        /**
         * @brief A friend function to get values via a module
         * 
         * We set this as the module function
         * if we are optimized for returning values from a module.
         * 
         * @param mod ModuleParameter to get value from
         * @return long double 
         */
        friend long double get_module(ModuleParameter* mod);

    public:

        ModuleParameter() =default;

        /**
         * @brief Construct a parameter, and configures for constant values
         * 
         * Creates a ModuleParameter, and configures it for returning 
         * constant values.
         * Useful for defining an initial value to return.
         * 
         * @param val Constant value to set
         */
        ModuleParameter(long double val) { this->set_constant(val); }

        /**
         * @brief Construct a parameter, and configures for constant values
         * 
         * Creates a ModuleParameter, and configures it for returning
         * values sampled from a module.
         * 
         * @param imod Module to track
         */
        ModuleParameter(AudioModule* imod) { this->set_module(imod); }

        /**
         * @brief Gets the current value
         * 
         * Under the hood, we call the value function.
         * We simply return what this gives us
         * 
         * @return long double Current value
         */
        long double get() { return this->func(this); }

        /**
         * @brief Set the value function
         * 
         * This sets the function to be used
         * for getting values.
         * 
         * @param fn Value function to utilize
         */
        void set_function(ModuleParameter::value_func fnc) { this->func = fnc; }

        /**
         * @brief Configures this parameter for constant values.
         * 
         * This sets the underlying constant value to the one provided,
         * and sets the value function to the 'get_constant()' method.
         * 
         * @param val Value to set
         */
        void set_constant(long double val);

        /**
         * @brief Configures this parameter for module sampling
         * 
         * This sets the module we are tracking to the one provided,
         * and sets the value function to the 'get_module()' method.
         * 
         * @param imod Module to set
         */
        void set_module(AudioModule* imod);
};

// Type alias representing a value function
using value_func = std::function<long double(ModuleParameter*)>;
