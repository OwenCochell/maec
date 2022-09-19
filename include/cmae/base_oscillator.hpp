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

#include "meta_audio.hpp"


/**
 * @brief A base class for all oscillators
 * 
 * This class is the base class for all oscillators.
 * This class offers no real functionality, but is
 * used for identifying oscillators.
 *
 */
class BaseOscillator : public BackStop {};


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
