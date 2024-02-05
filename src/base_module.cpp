/**
 * @file base_module.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Implementations for base modules
 * @version 0.1
 * @date 2022-09-04
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "base_module.hpp"

void BaseModule::start() {
    // Set the state to started:
    this->state_type = State::Started;
}

void BaseModule::stop() {
    // Set the state to stopped:
    this->state_type = State::Stopped;
}

void BaseModule::finish() {
    // Set the state to finishing:
    this->state_type = State::Finishing;

    // Call the finish method:

    this->done();
}

void BaseModule::done() {
    // Set the state to done:
    this->state_type = State::Finished;
}
