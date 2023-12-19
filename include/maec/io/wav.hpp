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

#include "../sink_module.hpp"
#include "../source_module.hpp"
#include "audio_buffer.hpp"
#include "mstream.hpp"

/**
 * @brief Struct that represents a chunk header
 * 
 * A chunk header is a collection of data that
 * is present at the start of every chunk.
 * This data shares the chunk ID,
 * as well as the size of the chunk.
 * 
 */
struct ChunkHeader {

    /// Chunk ID of the header
    //std::array<char, 4> chunk_id = {};
    std::string chunk_id = "    ";

    /// Size of the chunk
    u_int32_t chunk_size = 0;
};

/**
 * @brief Struct that represents a wave file header chunk
 * 
 * The wave file header chunk describes information
 * about the wave file we are working with.
 * 
 */
struct WavHeader {

    /// Chunk ID of the header, in this case "RIFF"
    //std::array<char, 4> chunk_id = {};
    std::string chunk_id = "    ";

    /// Chunk size, in this case the size of the file minus 8
    uint32_t chunk_size = 0;

    /// Format, in this case "WAVE"
    //std::array<char, 4> format = {};
    std::string format = "    ";
};

/**
 * @brief Struct that represents a wave format chunk
 * 
 * The format chunk contains a lot of information related
 * to the audio data contained within.
 * Much of this data is used elsewhere to intepret the audio data.
 * 
 * TODO: We need to have some support for working with compressed wave data,
 * and hold extra parameters encountered.
 */
struct WavFormat {

    /// Format of the wave data, in this case 1
    /// If anything other than 1, compression is utilized
    uint16_t format = 0;

    /// Number of channels in the wave data
    uint16_t channels = 0;

    /// Sample rate of the wave data
    uint32_t sample_rate = 0;

    /// Byte rate of the wave data
    uint32_t byte_rate = 0;

    /// Block align of the wave data
    uint16_t block_align = 0;

    /// Bits per sample of the wave data
    uint16_t bits_per_sample = 0;
};

/**
 * @brief A chunk that represents a unknown chunk
 * 
 * Sometimes, chunks with unknown structure/purpose are encountered.
 * We have no idea how to work with or utilize these chunks,
 * so we throw all the data into this struct.
 * This can be parsed and utilized by the user as necessary.
 * 
 * This struct contains the chunk ID,
 * size, and data (stored in a vector).
 * 
 */
struct UnknownChunk : public ChunkHeader {

    /// Data of unknown chunk
    std::vector<char> data;
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
     * @brief Sets the byte rate of the wave data
     * 
     * The byte rate determines how many bytes
     * are required to represent one second.
     * 
     * @param byr Byte rate of wave data to set
     */
    void set_byterate(int byr) { this->byte_rate = byr; }

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
     * @brief Sets the block align of the wave data
     * 
     * The block align is the number of bytes
     * per frame, that is,
     * the number of bytes required to represent a sample
     * at a given time in all channels.
     * 
     * @param bal Block align of wave data to set
     */
    void set_blockalign(int bal) { this->block_align = bal; }

    /**
     * @brief Gets the number of bits per sample
     * 
     * The bits per sample determines how many bits
     * are required to represent a single sample.
     * 
     * @return Number of bits per sample
     */
    int get_bits_per_sample() const { return this->bits_per_sample; }

    /**
     * @brief Sets the number of bits per sample
     * 
     * The bits per sample determines how many bits
     * are required to represent a single sample
     * 
     * This method automatically sets the bytes per sample
     * using the provided value
     * 
     * @param bps Number of bits per sample to set
     */
    void set_bits_per_sample(int bps) { this->bits_per_sample = bps; this->bytes_per_sample = bps / 8; }

    /**
     * @brief Gets the number of bytes per sample
     * 
     * The bytes per sample determines how many bytes
     * are required to represent a single sample
     * 
     * @return int Number of bytes per sample
     */
    int get_bytes_per_sample() const { return this->bytes_per_sample; }

    /**
     * @brief Sets the number of bytes per sample
     * 
     * The bytes per sample determines how many bytes
     * are required to represent a single sample.
     * 
     * This method automatically sets the bits per sample
     * using the provided value.
     * 
     * @param bps Number of bytes per sample to set
     */
    void set_bytes_per_sample(int bps) { this->bytes_per_sample = bps; this->bits_per_sample = bps * 8; }

    /**
     * @brief Gets the size of the wave file
     * 
     * This function returns the size of the wave file in bytes.
     * 
     * @return int Size in bytes
     */
    int get_size() const { return this->size; }

    /**
     * @brief Sets the size of the wave file
     * 
     * This function sets the size of the wave file in bytes.
     * 
     * @param size Size of wave file
     */
    void set_size(int size) { this->size = size; }

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

    /// Bytes per sample of the wave file
    int bytes_per_sample = 0;

    /// Size of the wave file
    int size = 0;
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
public:

    WaveReader() = default;

    WaveReader(BaseMIStream* stream) : stream(stream) {}

    /**
     * @brief Starts this wave file for reading
     * 
     * You should call this method before any read operations!
     * This component will open the file and do some preliminary checks,
     * as well as load critical info from the stream so we can correctly
     * handle incoming audio data.
     */
    void start();

    /**
     * @brief Stops this wave reader
     * 
     * You should call this method when reading is to be stopped!
     * This method ensures the mstream is properly closed.
     * 
     */
    void stop();

    /**
     * @brief Sets the input mstream to utilize
     * 
     * This function allows you to set an mstream to read from.
     * This WaveReader wil read all the data from the mstream provided.
     * 
     * @param stream New input mstream
     */
    void set_stream(BaseMIStream* stream) { this->stream = stream; }

    /**
     * @brief Gets the input mstream
     * 
     * This function allows you to get the mstream
     * this WaveReader utilizes.
     * 
     * @return BaseMIStream* input mstream
     */
    BaseMIStream* get_stream() const { return this->stream;}

    /**
     * @brief Reads audio data from the stream
     * 
     * We read chunks from the stream until
     * we encounter a data chunk.
     * From there, we read the data and do any necessary conversions (?)
     * 
     * If we encounter any weird chunks that we don't recognize,
     * then we will discard them
     * TODO: Save these for later?
     * Offer some kind of handling system for these? 
     * 
     */
    AudioBuffer get_data();

private:

    /**
     * @brief Reads the header of the current chunk
     * 
     * The chunk header contains the chunk ID and size,
     * which is used by other methods to process the chunk.
     * 
     * @param header ChunkHeader to place information into
     */
    void read_chunk_header(ChunkHeader& chunk);

    /**
     * @brief Reads the wave header chunk
     *
     * The wave header chunk contains basic information
     * about the wave file. 
     *
     * @param header WaveHeader to place information into
     */
    void read_wave_header(WavHeader& chunk);

    /**
     * @brief Reads the wave format chunk
     * 
     * This wave format chunk contains information about the wave data
     * 
     * @param chunk WavFormat to place information into
     */
    void read_format_chunk(WavFormat& chunk);

    void read_chunk();

    /// Stream we are reading from
    BaseMIStream* stream = nullptr;
};
