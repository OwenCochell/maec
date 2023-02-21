/**
 * @file chrono.cpp
 * @author Owen Cochell (owen@gmail.com)
 * @brief Implementations for time components
 * @version 0.1
 * @date 2023-02-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "chrono.hpp"

int64_t get_time() {

    // Get the current value:

    return std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now()).time_since_epoch().count();
}

void ChainTimer::reset() {

    // Reset variables:

    this->channels = 1;
    this->sample = 0;
    this->nano_frame = 0;
}
