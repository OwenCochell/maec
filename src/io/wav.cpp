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

#include <cstring>
#include <vector>

#include "io/wav.hpp"
#include "audio_buffer.hpp"

void WaveReader::start() {

    // First, start the stream:

    stream->start();

    // Next, read file header:

    WavHeader head;

    this->read_wave_header(head);

    // Ensure we are in a format we expect:

    //if (strcmp(head.chunk_id.data(), "RIFF") != 0) {
    if (head.chunk_id != "RIFF") {

        // Invalid header!
        // We do not support this file
        // TODO: Do something here
        int dhd = 0;
    }

    // Ensure the format is something we expect:

    //if (strcmp(head.chunk_id.data(), "WAVE") != 0) {
    if (head.format != "WAVE") {

        // Invalid format type!
        // We do not support this file
        // TODO: Again, do something here
        int dhd = 0;
    }

    // Save the size of the file:

    this->set_size(head.chunk_size);

    // Next, read the format chunk
    // TODO: Is it always guaranteed that format will always be second chunk?

    ChunkHeader chead;

    this->read_chunk_header(chead);

    // Determine this chunk is a format chunk:

    //if (strcmp(head.chunk_id.data(), "fmt ") != 0) {
    if (chead.chunk_id != "fmt ") {

        // This chunk is not format chunk
        // TODO: Gotta do something here
        int dhd = 0;
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
    this->set_bits_per_sample(form.bits_per_sample);
}

void WaveReader::stop() {

    // Simply stop the stream:

    this->stream->stop();
}

void WaveReader::read_chunk_header(ChunkHeader& chunk) {

    // Read the chunk header:

    //this->stream->read(chunk.chunk_id.begin(), 4);
    this->stream->read(chunk.chunk_id.data(), 4);
    this->stream->read(reinterpret_cast<char*>(&chunk.chunk_size), 4);
}

void WaveReader::read_wave_header(WavHeader& chunk) {

    // Read the generic header data:

    ChunkHeader head;

    this->read_chunk_header(head);

    // Read the format data:

    //this->stream->read(chunk.format.begin(), 4);
    this->stream->read(chunk.format.data(), 4);

    // Place contents of the chunk header into the wave header:

    // TODO: Is this line inefficient?
    chunk.chunk_id = head.chunk_id;
    chunk.chunk_size = head.chunk_size;
}

void WaveReader::read_format_chunk(WavFormat& chunk) {

    // Read the audio format:
    this->stream->read(reinterpret_cast<char*>(&chunk.format), 2);

    // Read the number of channels
    this->stream->read(reinterpret_cast<char*>(&chunk.channels), 2);

    // Read the sample rate:
    this->stream->read(reinterpret_cast<char*>(&chunk.sample_rate), 4);

    // Read the byte rate:
    this->stream->read(reinterpret_cast<char*>(&chunk.byte_rate), 4);

    // Read the block align:
    this->stream->read(reinterpret_cast<char*>(&chunk.block_align), 2);

    // Read the bits per sample:
    this->stream->read(reinterpret_cast<char*>(&chunk.bits_per_sample), 2);
}

BufferPointer WaveReader::get_data() {

    while (true) {

        // Read wave file header:

        ChunkHeader head;

        this->read_chunk_header(head);

        // Determine if this is a data chunk:

        //if (strcmp(head.chunk_id.data(), "data") != 0) {
        if (head.chunk_id != "data") {

            // Not a data chunk, continue
            continue;
        }

        // Determine which processing pathway to utilize:
        // TODO: Really don't like this.
        // While I think this is the fastest solution,
        // It has a few problems:
        // 1. Not modular - This is all hardcoded
        // 2. Endianess problems, what we we have differing endianess?
        // 3. Breaks the DRY principal a lot

        if (this->get_bits_per_sample() == 16) {

            // Need to do 16 bit processing:

            std::vector<int16_t> tdata(head.chunk_size / this->get_bytes_per_sample());

            // Read data into vector:

            this->stream->read(reinterpret_cast<char*>(tdata.data()), head.chunk_size);

            // Create AudioBuffer:

            auto buff = std::make_unique<AudioBuffer>(head.chunk_size / this->get_bytes_per_sample(), this->get_channels());

            // Read data into buffer:

            for (int i = 0; i < tdata.size(); ++i) {

                buff->at(i) = static_cast<long double>(tdata[i]) / 32768.;
            }

            // Finally, return data:

            return buff;
        }
    }
}
