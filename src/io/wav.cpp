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
#include <cstring>

#include "io/wav.hpp"

void WaveReader::start() {

    // First, start the stream:

    stream.start();

    // Next, read file header:

    WavHeader head;

    this->read_wave_header(head);

    // Ensure we are in a format we expect:

    if (strcmp(head.chunk_id.data(), "RIFF") != 0) {

        // Invalid header!
        // We do not support this file
        // TODO: Do something here
    }

    // Ensure the format is something we expect:

    if (strcmp(head.chunk_id.data(), "WAVE") != 0) {

        // Invalid format type!
        // We do not support this file
        // TODO: Again, do something here
    }

    // Save the size of the file:

    this->set_size(head.chunk_size);

    // Next, read the format chunk
    // TODO: Is it always guaranteed that format will always be second chunk?

    ChunkHeader chead;

    this->read_chunk_header(chead);

    // Determine this chunk is a format chunk:

    if (strcmp(head.chunk_id.data(), "fmt") != 0) {

        // This chunk is not format chunk
        // TODO: Gotta do something here
    }

    // Read the wave format chunk

    WavFormat form;

    this->read_format_chunk(form);

    // Set the values from the format chunk:

    this->set_format(form.format);
    this->set_channels(form.channels);
    this->set_samplerate(form.sample_rate);
    this->set_byterate(form.byte_rate);
    this->set_blockalign(form.block_align);
    this->set_bitspersample(form.bits_per_sample);
}

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

    // TODO: Is this line inefficient?
    chunk.chunk_id = head.chunk_id;
    chunk.chunk_size = head.chunk_size;
}

void WaveReader::read_format_chunk(WavFormat& chunk) {

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
