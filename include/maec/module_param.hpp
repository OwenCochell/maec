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
#include <utility>

#include "audio_module.hpp"
#include "sink_module.hpp"
#include "meta_audio.hpp"

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
 * 
 * These parameters offer a modular framework for defining sources for values.
 * Under the hood, this component is a sink,
 * meaning users can attach modules and chains to this parameter.
 * When we are asked to get parameter data, we process the backward modules
 * and return the AudioBuffer for use.
 * Developers can preform this workflow themselves,
 * or they can simply call the get() method which does all this in one call.
 * 
 * It is important to note that this component MUST be included in all state operations!
 * Otherwise, this component may preform oddly or break.
 * An easy way to do this would be to call the necessary state functions in your module.
 * 
 * We also offer some methods for simplifying configuration.
 * Using the proper methods, we can auto-configure this class for getting values
 * from an AudioModule, or this component can configured to return a constant value.
 * Since this component is a sink, one can configure it as any maec module.
 * 
 * One possible use of this component is to offer the ability for parameters to change over time.
 * For example, one could attach a SineOscillator, which would cause the this parameter to represent a sine wave.
 * This allows for components to be modulated.
 * 
 */
class ModuleParameter : public SinkModule {

    private:

        /// A constant value to be returned
        long double value = 0;

        /// Pointer to ConstModule
        std::unique_ptr<ConstModule> const_mod = nullptr;

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
        ModuleParameter(AudioModule* imod) { this->bind(imod); }

        /**
         * @brief Gets the current buffer
         * 
         * We meta process and backward modules,
         * and then return the AudioBuffer.
         * 
         * @return AudioBuffer Buffer of values to work with
         */
        BufferPointer get();

        /**
         * @brief Configures this parameter for constant values.
         * 
         * This sets the underlying constant value to the one provided,
         * and sets the value function to the 'get_constant()' method.
         * 
         * @param val Value to set
         */
        void set_constant(long double val);
};
