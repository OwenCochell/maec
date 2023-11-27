/**
 * @file fund_oscillator.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Implementations of the fundamental oscillators
 * @version 0.1
 * @date 2022-09-30
 *
 * @copyright Copyright (c) 2022
 *
 */

#define _USE_MATH_DEFINES  // NOLINT(bugprone-reserved-identifier): Required to get access to M_PI

#include "fund_oscillator.hpp"

#include <cmath>
#include <vector>

const long double TWO_PI = 2.0 * M_PI;

void SineOscillator::process() {

    // Create a new buffer:

    this->set_buffer(this->create_buffer());

    // Fill the buffer with the sine wave:

    for (auto iter = this->buff->ibegin();
         static_cast<unsigned int>(iter.get_index()) < this->buff->size(); ++iter) {

        // Calculate the sine wave:

        *iter = sin(static_cast<double>(TWO_PI * this->frequency * this->phase / this->sample_rate));

        // Increment the phase:

        this->phase++;
    }
}

void SquareOscillator::process() {

    // Create a new buffer:

    this->set_buffer(this->create_buffer());

    // Create a placeholder:

    double placeholder = 0.0;

    // Fill the buffer with the square wave:

    for (auto iter = this->buff->ibegin();
         (unsigned int)iter.get_index() < this->buff->size(); ++iter) {

        // Calculate the square wave:

        *iter = (modf(static_cast<double>(this->frequency * this->phase / this->sample_rate),
                      &placeholder) < 0.5)
                    ? 1.0
                    : -1.0;

        // Increment the phase:

        this->phase++;
    }
}

void SawtoothOscillator::process() {

    // Create a new buffer:

    this->set_buffer(this->create_buffer());

    // Create a placeholder:

    double placeholder = 0.0;

    // Fill the buffer with the sawtooth wave:

    for (auto iter = this->buff->ibegin();
         static_cast<unsigned int>(iter.get_index()) < this->buff->size(); ++iter) {

        // Calculate the sawtooth wave:

        *iter = (2.0 *
                 modf(static_cast<double>(this->frequency * this->phase / this->sample_rate + 0.5),
                      &placeholder)) -1.0;

        // Increment the phase:

        this->phase++;
    }
}

void TriangleOscillator::process() {

    // Create a new buffer:

    this->set_buffer(this->create_buffer());

    // Create a placeholder:

    double placeholder = 0.0;

    // Fill the buffer with the triangle wave:

    for (auto iter = this->buff->ibegin();
         static_cast<unsigned int>(iter.get_index()) < this->buff->size(); ++iter) {

        // Calculate the triangle wave:

        long double temp =
            modf(static_cast<double>(this->frequency * this->phase / this->sample_rate),
                 &placeholder);

        if (temp < 0.25) {
            temp *= 4.0;
        }

        else if (temp > 0.75) {
            temp = (temp - 1.0) * 4.0;
        }

        else {
            temp = (0.5 - temp) * 4.0;
        }

        *iter = temp;

        // Increment the phase:

        this->phase++;
    }
}
