/**
 * @file amp_module.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Implementations for amplitude modules
 * @version 0.1
 * @date 2023-02-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <algorithm>

#include "amp_module.hpp"

void AmplitudeScale::process() {

    // Scale the audio by the given value:

    std::transform(this->buff->ibegin(), this->buff->iend(), this->buff->ibegin(), [this](long double inv) {

        // Scale the value:

        return this->get_value() * inv;
    });
}

void AmplitudeAdd::process() {

    // Add the given value to the audio:

    std::transform(this->buff->ibegin(), this->buff->iend(), this->buff->ibegin(), [this](long double inv) {

        // Add to the value:

        return this->get_value() + inv;
    });
}
