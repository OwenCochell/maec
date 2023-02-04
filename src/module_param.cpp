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

}

void ModuleParameter::set_constant(long double val) {

    // Set the underlying value:

    this->value = val;

    // Set the value function:

    this->func = &get_constant;
}
