/**
 * @file wav.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Wave file support
 * @version 0.1
 * @date 2023-10-26
 * 
 * @copyright Copyright (c) 2023
 * 
 * Here we define various classes and modules that work with WAV files.
 * Wave file sinks and sources are provided,
 * so modules can read and write to wave files.
 */

#include <fstream>

#include "../sink_module.hpp"
#include "../source_module.hpp"

/**
 * @brief Struct that represents a wave file header
 * 
 */
struct WavHeader {

    /// Chunk ID of the header, in this case "RIFF"
    char chunk_id[4];

    /// Chunk size, in this case the size of the file minus 8
    int chunk_size;

    /// Format, in this case "WAVE"
    char format[4];
};


/**
 * @brief Base class for wave data manipulation
 * 
 * This class defines some common attributes that must be
 * kept in mind when working with wave files.
 * These values will be utilized by other components
 * to determine how to work with wave data.
 * 
 * This probably is not the class you want,
 * WaveReader or WaveWriter is probably more useful. 
 * 
 */
class BaseWave {

public:

    /**
     * @brief Gets the format of this wave data
     * 
     * The format of wave data determines how the data is stored.
     * If this value is 0, then there is no compression at play
     * and samples can be interpreted as they are stored.
     * Anything else is a compression format,
     * which we do not support at this time.
     * 
     * @return Format of the wave data
     */
    int get_format() const { return format; }

    /**
     * @brief Sets the format of this wave data
     * 
     * @param form New format to set
     */
    void set_format(int form) { this->format = form; } 

    /**
     * @brief Gets the channel count of this wave data
     * 
     * The channel count determines how many channels
     * are present in the wave data.
     * 
     * @return Number of channels in wave data
     */
    int get_channels() const { return channels; }

    /**
     * @brief Sets the channel count of this wave data
     * 
     * @param chan New channel count to set
     */
    void set_channels(int chan) { this->channels = chan;}

    /**
     * @brief Gets the samplerate of the wave data
     * 
     * The samplerate determines how many samples
     * are present in the wave data per second.
     * 
     * @return Samplerate of the wave data
     */
    int get_samplerate() const { return this->sample_rate; }

    /**
     * @brief Sets the samplerate of the wave data 
     *
     * @param samp New samplerate to set
     */
    void set_samplerate(int samp) { this->sample_rate = samp; }

    /**
     * @brief Gets the byte rate of the wave data
     * 
     * The byte rate determines how many bytes
     * are required to represent one second.
     * 
     * @return Byte rate of the wave data
     */
    int get_byterate() const { return this->byte_rate; }

    /**
     * @brief Gets the block align of the wave data
     * 
     * The block align is the number of bytes
     * per frame, that is,
     * the number of bytes required to represent a sample
     * at a given time in all channels.
     * 
     * @return Block align of the wave data
     */
    int get_blockalign() const { return this->block_align; }

    /**
     * @brief Gets the number of bits per sample
     * 
     * The bits per sample determines how many bits
     * are required to represent a single sample.
     * 
     * @return Number of bits per sample
     */
    int get_bitspersample() const { return this->bits_per_sample; }

private:

    /// Format of the audio
    int format = 0;

    /// Number of channels present in the wave file
    int channels = 1;

    /// Sample rate of the wave file
    int sample_rate = 44100;

    /// Byte rate of the wave file
    int byte_rate = 0;

    /// Block align of the wave file (size of a frame)
    int block_align = 0;

    /// Bits per sample of the wave file
    int bits_per_sample = 0;
};

/**
 * @brief Component for reading wave data.
 * 
 * This component simplifies the process of reading wave data.
 * We automatically extract the important metadata,
 * traverse the data in a meaningful way,
 * and extract audio data for use.
 * 
 * Wave data is generic, and can be from anywhere.
 * So, this component can read data from a file,
 * socket, or anything that supports standard C++
 * IO streams.
 * 
 */
class WaveReader : public BaseWave {

}