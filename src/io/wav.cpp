/**
 * @file wav.cpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Implementations for wave file components
 * @version 0.1
 * @date 2023-10-30
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <istream>

#include "io/wav.hpp"

void WaveReader::read_chunk_header(ChunkHeader& header) {

    // Read the chunk header:

    this->str.read(header.chunk_id, 4);
    this->str.read(reinterpret_cast<char*>(&header.chunk_size), 4);
}
