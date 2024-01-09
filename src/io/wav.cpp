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

#include <vector>

#include "io/wav.hpp"
#include "audio_buffer.hpp"

void ChunkHeader::decode(BaseMIStream& stream) {

    // Create array to hold data:
    std::array<char, size()> data = {};

    // Copy data into array:

    stream.read(data.begin(), size());

    // Load byte data:

    this->decode(data.begin());
}

void ChunkHeader::decode(char* byts) {

    // Extract data from array:

    std::copy_n(byts, 4, this->chunk_id.data());
    this->chunk_size = char_int32(byts + 4);
}

void ChunkHeader::encode(BaseMOStream& stream) {

    // Create array to hold data:
    std::array<char, size()> data = {};

    // Convert into bytes:

    encode(data.begin());

    // Write contents to stream:

    stream.write(data.data(), size());
}

void ChunkHeader::encode(char* byts) {

    // Encode string:

    std::copy_n(this->chunk_id.data(), 4, byts);

    // Encode chunk size:

    int32_char(this->chunk_size, byts + 4);
}

void WavHeader::decode(BaseMIStream& stream) {

    // Create array to hold data:

    std::array<char, size()> data = {};

    // Copy data into array:

    stream.read(data.begin(), size());

    // Load header data:

    ChunkHeader::decode(data.begin());

    // Load string from chunk header:

    chunk_id = ChunkHeader::chunk_id;
    chunk_size = ChunkHeader::chunk_size;

    // Load format data:

    std::copy_n(data.begin() + ChunkHeader::size(),
                WavHeader::size() - ChunkHeader::size(), format.data());
}

void WavHeader::encode(BaseMOStream& stream) {

    // Create array to hold data:

    std::array<char, size()> data = {};

    // First, ask the header to encode into bytes:

    ChunkHeader::encode(data.begin());

    // Next, encode format data:

    std::copy_n(format.data(), WavHeader::size() - ChunkHeader::size(), data.begin() + ChunkHeader::size());

    // Write data to the stream:

    stream.write(data.begin(), data.size());
}

void WavFormat::decode(BaseMIStream& stream) {

    // Create array to hold data:

    std::array<char, size() - ChunkHeader::size()> data = {};

    // Copy data into array:

    stream.read(data.begin(), data.size());

    // Load format data:
 
    this->format = char_int16(data.begin());
    this->channels = char_int16(data.begin() + 2);
    this->sample_rate = char_uint32(data.begin() + 4);
    this->byte_rate = char_uint32(data.begin() + 8);
    this->block_align = char_int16(data.begin() + 12);
    this->bits_per_sample = char_int16(data.begin() + 14);
}

void WavFormat::encode(BaseMOStream& stream) {

    // Create array of data:

    std::array<char, size()> data = {};

    // Encode chunk data:

    ChunkHeader::encode(data.begin());

    // Encode each value:

    auto* pbegin = data.begin() + ChunkHeader::size();

    int16_char(this->format, pbegin);
    int16_char(this->channels, pbegin + 2);
    int32_char(this->sample_rate, pbegin + 4);
    int32_char(this->byte_rate, pbegin + 8);
    int16_char(this->block_align, pbegin + 12);
    int16_char(this->bits_per_sample, pbegin + 14);

    // Write data to stream:

    stream.write(data.begin(), data.size());
}

void UnknownChunk::decode(BaseMIStream& stream) {

    // Read data into vector:

    stream.read(data.data(), data.size());
}

void UnknownChunk::encode(BaseMOStream& stream) {

    // Encode ChunkHeader:

    ChunkHeader::encode(stream);

    //  Send vector contents to stream:

    stream.write(data.data(), data.size());
}

void WaveReader::start() {

    // First, start the stream:

    stream->start();

    // Next, read file header:

    WavHeader head;

    this->read_wave_header(head);

    // Ensure we are in a format we expect:

    if (head.chunk_id != "RIFF") {

        // Invalid header!
        // We do not support this file
        // TODO: Do something here
        int dhd = 0;
    }

    // Ensure the format is something we expect:

    if (head.format != "WAVE") {

        // Invalid format type!
        // We do not support this file
        // TODO: Again, do something here
        int dhd = 0;
    }

    // Save the size of the file:

    this->set_size(head.chunk_size + 8);

    // Next, read the format chunk
    // TODO: Is it always guaranteed that format will always be second chunk?

    ChunkHeader chead;

    this->read_chunk_header(chead);

    // Determine this chunk is a format chunk:

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

    chunk.decode(*stream);

    this->total_read += 8;
}

void WaveReader::read_wave_header(WavHeader& chunk) {

    // Read the format data:

    chunk.decode(*stream);

    this->total_read += 4+8;
}

void WaveReader::read_format_chunk(WavFormat& chunk) {

    chunk.decode(*stream);

    this->total_read += 16;
}

BufferPointer WaveReader::get_data() {

    // Define the BufferPointer to return:

    BufferPointer bpoint = std::make_unique<AudioBuffer>(
        this->buffer_size,
        this->get_channels());

    // Also set the sample rate:

    bpoint->set_samplerate(this->get_samplerate());

    // Define some variables:

    int read = 0;

    // Loop as long as our mstream is valid
    while (!this->done() && read < buffer_size * this->get_channels()) {

        // Read wave file header:

        if (this->needs_chunk) {

            this->read_chunk_header(this->head);

            // Determine if this is a data chunk:

            if (this->head.chunk_id != "data") {

                // Create UnknownChunk:

                UnknownChunk chunk(this->head.chunk_size);

                // Not a data chunk, read data into an unknown chunk:

                this->stream->read(chunk.data.data(), this->head.chunk_size);

                // Also copy over other pieces of info:

                chunk.chunk_id = head.chunk_id;
                chunk.chunk_size = head.chunk_size;

                // We are past this weird chunk! Continue
                // TODO: Should we save these chunks somewhere?

                this->total_read += this->head.chunk_size;

                continue;
            }

            // Set our reading chunk status:

            this->needs_chunk = false;

            // Set the number of bytes read from this chunk:

            this->chunk_read = 0;
        }

        // Determine which processing pathway to utilize:
        // TODO: Really don't like this.
        // While I think this is the fastest solution,
        // It has a few problems:
        // 1. Not modular - This is all hardcoded
        // 2. Endianess problems, what we we have differing endianess?
        // 3. Breaks the DRY principal a lot

        // Determine the number of bytes to read
        // We either read the entire buffer size,
        // or the number of bytes left in this chunk, whichever is smallest

        const int buffer_bytes =
            (this->buffer_size * this->get_channels() - read) * this->get_bytes_per_sample();

        const int chunk_bytes = static_cast<int>(head.chunk_size - this->chunk_read);

        int to_read = 0;

        if (chunk_bytes <= buffer_bytes) {

            // This chunk is NOT big enough to fill a buffer
            // We read all we can and then read a new chunk

            this->needs_chunk = true;

            to_read = chunk_bytes;
        }

        else {

            // Chunk is big enough, read size of vector:

            to_read = buffer_bytes;
        }

        // Read data into vector:

        std::vector<char> tdata(to_read);

        this->stream->read(tdata.data(), to_read);

        this->total_read += to_read;

        // Add to number of bytes read:

        this->chunk_read += to_read;

        // Determine which type to read in:

        if (this->get_bits_per_sample() == 16) {

            // We are reading in 16 bit ints:

            for (int i = 0; i < static_cast<int>(tdata.size() / 2); ++i) {

                // Read this int16:

                auto val = char_int16(tdata.begin() +
                                      static_cast<int64_t>(i) * 2);

                // Convert to mf:

                auto mf_val = int16_mf(val);

                // Add value to final vector:

                bpoint->at(read++) = mf_val;
            }
        }

        else if (this->get_bits_per_sample() == 8) {

            // We are reading in unsigned chars

            for (char i : tdata) {

                // Convert into mf:

                const long double val = uchar_mf(i);

                // Add to buffer:

                bpoint->at(read++) = val;
            }
        }
    }

    // Determine if we need to close the file (reached end):

    if (this->total_read >= static_cast<u_int32_t>(this->get_size())) {

        // Stop this WaveReader

        this->stop();
    }

    // Finally, return buffer pointer:

    return bpoint;
}

void WaveWriter::start() {

    // First, create the wave header:

    WavHeader whead;

    // Write the data:

    whead.encode(*stream);
}

void WaveSource::start() {

    // Start the wave reader:

    WaveReader::start();

    // Populate our AudioInfo data from wave file info:

    auto* info = this->get_info();

    info->channels = this->get_channels();
    info->in_buffer = 0;
    info->sample_rate = this->get_samplerate();

    // Use buffer size from AUdioInfo:

    WaveReader::set_buffer_size(info->out_buffer);
}

void WaveSource::stop() {

    // Stop the wave reader:

    WaveReader::stop();
}

void WaveSource::process() {

    // Ask the WaveReader to generate AudioBuffer:

    auto buff = WaveReader::get_data();

    // Set our buffer:

    this->set_buffer(std::move(buff));
}
