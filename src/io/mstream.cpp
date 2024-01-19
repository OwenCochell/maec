/**
 * @file mstream.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Implementations for mstreams
 * @version 0.1
 * @date 2023-11-02
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "io/mstream.hpp"

#include <iterator>

void CharOStream::write(char* byts, int num) {

    // Offset to use when determining back insertion:

    int offset = 0;

    // Determine if we need to overwrite instead of push_back:

    if (this->index < this->arr.size()) {

        // Determine the max value to copy to:

        const int max = std::min(static_cast<std::size_t>(this->index + num), this->arr.size());

        // Determine offset value:

        offset = max - this->index;

        // Copy values over:

        std::copy_n(byts, max - this->index, this->arr.begin() + this->index);

        // Increment index to max value:

        this->index = max;
    }

    // Do some back insertions:

    std::copy_n(byts + offset, num - offset, std::back_inserter(this->arr));

    // Configure index:

    this->index += num - offset;
}
