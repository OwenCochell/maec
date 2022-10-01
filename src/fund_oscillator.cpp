/**
 * @file fund_oscillator.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief IMplementations of the fundamental oscillators
 * @version 0.1
 * @date 2022-09-30
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <cmath>
#include "fund_oscillator.hpp"


void SineOscillator::process() {

    // Create a new buffer:

    this->set_buffer(this->create_buffer());

    // Fill the buffer with the sine wave:

    for (auto i = 0; i < this->buff->size(); i++) {

        // Calculate the sine wave:

        this->buff->at(i) = sin(2 * M_PI * this->frequency * this->phase / this->sample_rate);

        // Increment the phase:

        this->phase++;

    }

}