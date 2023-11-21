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

void WaveReader::read_chunk_header(ChunkHeader& chunk) {

    // Read the chunk header:

    this->stream.read(chunk.chunk_id.begin(), 4);
    this->stream.read(reinterpret_cast<char*>(&chunk.chunk_size), 4);
}

void WaveReader::read_wave_header(WavHeader& chunk) {

    // Read the generic header data:

    ChunkHeader head;

    this->read_chunk_header(head);

    // Read the format data:

    this->stream.read(chunk.format.begin(), 4);

    // Place contents of the chunk header into the wave header:

    chunk.chunk_id = head.chunk_id;
    chunk.chunk_size = head.chunk_size;
}

void WaveReader::read_format_header(WavFormat& chunk) {

    // Read the audio format:
    this->stream.read(reinterpret_cast<char*>(&chunk.format), 2);

    // Read the number of channels
    this->stream.read(reinterpret_cast<char*>(&chunk.channels), 2);

    // Read the sample rate:
    this->stream.read(reinterpret_cast<char*>(&chunk.channels), 4);

    // Read the byte rate:
    this->stream.read(reinterpret_cast<char*>(&chunk.byte_rate), 4);

    // Read the block align:
    this->stream.read(reinterpret_cast<char*>(&chunk.block_align), 2);

    // Read the bits per sample:
    this->stream.read(reinterpret_cast<char*>(&chunk.bits_per_sample), 2);
}
