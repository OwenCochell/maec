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

#include <algorithm>
#include <iterator>

void CharIStream::read(char* byts, int num) {

    // Copy the contents over:

    std::copy_n(arr.begin() + this->index, num, byts);

    // Increment index:

    this->index += num;
}

void CharOStream::write(char* byts, int num) {

    // Offset to use when determining back insertion:

    std::size_t offset = 0;

    // Determine if we need to overwrite instead of push_back:

    if (this->index < this->arr.size()) {

        // Determine the max value to copy to:

        const std::size_t max = std::min(this->index + num, this->arr.size());

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

void FIStream::read(char* byts, int num) {

    // Read the data:
    get_stream()->read(byts, num);

    // Determine if we are in a bad state:

    if (!BaseFStream::good()) {

        // Invalid, just close:

        this->stop();
    }
}

void FIStream::start() {

    // Call parent start method:

    BaseMIStream::start();

    // Open the fstream:

    open();

    // Determine if we are in a bad state:

    if (!BaseFStream::good()) {

        // We are invalid, set error state:

        this->set_state(BaseMStream::mstate::err);
    }
}

void FIStream::stop() {

    // Call parent stop method:

    BaseMIStream::stop();

    // Close the fstream:

    close();
}

void FOStream::write(char* byts, int num) {

    // Write the bytes:
    this->get_stream()->write(byts, num);

    // Determine if we are in a bad state:

    if (!BaseFStream::good()) {

        // Invalid, just close:

        this->stop();
    }
}

void FOStream::start() {

    // Call parent start method:

    BaseMOStream::start();

    // Open the fstream:

    open();

    // Determine if we are in a bad state:

    if (!BaseFStream::good()) {

        // We are invalid, set error state:

        this->set_state(BaseMStream::mstate::err);
    }
}

void FOStream::stop() {

    // Call parent stop method:

    BaseMOStream::stop();

    // Close the fstream:

    close();
}
