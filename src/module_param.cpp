/**
 * @file module_param.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Implementations for module parameters
 * @version 0.1
 * @date 2023-02-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "module_param.hpp"

long double get_constant(ModuleParameter* mod) { return mod->value; }

long double get_module(ModuleParameter* mod) {

    // First, determine if we need to sample:

    if ( mod->siter == mod->eiter ) {

        // Sample the back module:

        mod->mod->meta_process();

        // Grab the buffer:

        mod->buff = mod->mod->get_buffer();

        // Create a start iterator:

        mod->siter = mod->buff->ibegin();
        mod->eiter = mod->buff->iend();
    }

    // Finally, just return the current value:

    long double val = *(mod->siter);

    // Increment the iterator

    ++(mod->siter);

    // Finally, return val:

    return val;

}

void ModuleParameter::set_constant(long double val) {

    // Set the underlying value:

    this->value = val;

    // Set the value function:

    this->func = &get_constant;
}

void ModuleParameter::set_module(AudioModule* imod) {

    // Set the underlying module:

    this->mod = imod;

    // Reset the iterators:

    this->siter = AudioBuffer::InterIterator<long double>();
    this->eiter = AudioBuffer::InterIterator<long double>();

    this->buff = nullptr;

    // Finally, the function:

    this->func = &get_module;
}
