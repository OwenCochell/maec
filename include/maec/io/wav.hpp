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
 * Pretty much all chunks inherit this struct.
 * When encoding, there are no big surprises,
 * each chunk will configure the header as necessary.
 * 
 * For decoding, there are some things to keep in mind.
 * The values defined here (specifically chunk_size)
 * will be utilized to decode the rest of the chunk.
 * These values MUST be accurate to ensure this operation goes well!
 * 
 * At the end of the day, 
 * use the high level WaveReader classes!
 * They will handle this stuff for you!
 */
struct ChunkHeader {

    /// Chunk ID of the header
    std::string chunk_id = "    ";

    /// Size of the chunk
    uint32_t chunk_size = 0;

    /// Size of chunk data
    static const u_int32_t csize = 4 + 4;

    /**
     * @brief Determines the size of this chunk
     * 
     * @return u_int32_t Size of chunk in bytes
     */
    constexpr static u_int32_t size() { return csize; }

    /**
     * @brief Create this chunk using data from a stream
     * 
     * @param stream Stream to utilize
     */
    void decode(BaseMIStream& stream);

    /**
     * @brief Create this chunk using byte data
     * 
     * @param byts Byte data to utilize
     */
    void decode(char* byts);

    /**
     * @brief Converts this chunk into bytes and sends it to the stream
     * 
     * @param stream Stream to utilize
     */
    void encode(BaseMOStream& stream);

    /**
     * @brief Converts this chunk into bytes
     * 
     * @param byts Byte data to output to
     */
    void encode(char* byts);
};

/**
 * @brief Struct that represents a wave file header chunk
 * 
 * The wave file header chunk describes information
 * about the wave file we are working with.
 * 
 */
struct WavHeader : public ChunkHeader {

    /// Chunk ID of this header, should be RIFF
    std::string chunk_id = "RIFF";

    /// Format, in this case "WAVE"
    std::string format = "WAVE";

    /// Size of this chunk
    static const uint32_t csize = ChunkHeader::size() + 4;

    /**
     * @brief Returns the size of this chunk
     * 
     * @return u_int32_t Size of chunk in bytes
     */
    constexpr static u_int32_t size() { return csize; }

    /**
     * @brief Create this chunk data from a stream.
     * 
     * We ask the ChunkHeader class to pull it's generic data,
     * and from there we pull any necessary values.
     * 
     * @param stream Stream to utilize
     */
    void decode(BaseMIStream& stream);

    /**
     * @brief Converts this chunk into bytes and sends it to the stream
     * 
     * @param stream Stream to utilize
     */
    void encode(BaseMOStream& stream);
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
struct WavFormat : public ChunkHeader {

    /// Chunk ID of format chunks
    std::string chunk_id = "fmt ";

    /// Size of this chunk
    uint32_t chunk_size = size() - ChunkHeader::size();

    /// Format of the wave data, in this case 1
    /// If anything other than 1, compression is utilized
    uint16_t format = 1;

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

    /// Size of this chunk
    static const uint32_t csize = 2 + 2 + 4 + 4 + 2 + 2 + ChunkHeader::size();

    /**
     * @brief Returns the size of this chunk
     * 
     * @return u_int32_t Size of chunk in bytes
     */
    constexpr static uint32_t size() { return csize; }

    /**
     * @brief Creates this Format Chunk for a mstream
     * 
     * @param stream Stream to utilize
     */
    void decode(BaseMIStream& stream);

    /**
     * @brief Encodes this Format Chunk and sends it though the mstream
     * 
     * @param stream Stream to utilize
     */
    void encode(BaseMOStream& stream);
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

    UnknownChunk(uint32_t size) : data(size) { this->chunk_size = size; }

    /// Data of unknown chunk
    std::vector<char> data;

    /**
     * @brief Determines the size of this chunk
     * 
     * @return u_int32_t Size of this chunk in bytes
     */
    uint32_t size() const { return ChunkHeader::size() + data.size(); }

    /**
     * @brief Creates this UnknownChunk for a mstream
     * 
     * @param stream Stream to utilize
     */
    void decode(BaseMIStream& stream);

    /**
     * @brief Encodes this UnknownChunk and sends it through the mstream
     * 
     * @param stream Stream to utilize
     */
    void encode(BaseMOStream& stream);
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
     * If this value is 1, then there is no compression at play
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
    int format = 1;

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
 * Wave data is provided to this component via mstreams.
 * There are many mstreams provided with maec, so check them out!
 * 
 * This reader supports buffered reading,
 * which means we will extract data from the file in chunks,
 * keeping the state of our position as we iterate through the wave data.
 * Most wave files put all the data into one chunk,
 * but if it is split then we will handle this as well.
 * If we reach the end of the file before we reach the buffer size,
 * then the rest of the buffer will be filled with zeros.
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
    BaseMIStream* get_stream() const { return this->stream; }

    /**
     * @brief Returns the size of the output buffer
     * 
     * The buffer size is the number FRAMES this reader will output.
     * For example, if there are 3 channels, and you wish to have 3 frames,
     * then this reader will output 9 samples.
     * 
     * @return int Size of output buffer
     */
    int get_buffer_size() const { return this->buffer_size; }

    /**
     * @brief Sets the size of the output buffer
     * 
     * @param bsize New size of output buffer
     */
    void set_buffer_size(int bsize) { this->buffer_size = bsize; }

    /**
     * @brief Determines if this WaveReader is done
     * 
     * A wave reader can be 'done' for a few reasons.
     * First, if we hit the end of the file,
     * then there is nothing left to do, and we are done.
     * 
     * Second, a reader can be done if the given mstream goes into an invalid state.
     * 
     * @return true Reader is done
     * @return false Reader is not done
     */
    bool done() const {
        return this->total_read >= static_cast<u_int32_t>(this->get_size()) || this->stream->bad(); }

    /**
     * @brief Reads audio data from the stream
     * 
     * We buffer the audio data as necessary,
     * reading chunks and maintaining our state as we go.
     * If we need more data than is left in the file,
     * the empty space in the buffer will simply be zeros.
     * 
     * If we encounter any weird chunks that we don't recognize,
     * then we will discard them
     * TODO: Save these for later?
     * Offer some kind of handling system for these? 
     * 
     * @return BufferPointer AudioBuffer containing audio data, ready for processing
     */
    BufferPointer get_data();

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

    /// Size of buffer to output
    int buffer_size = 0;

    /// Number of bytes read from chunk
    int chunk_read = 0;

    /// Total number of bytes read
    uint32_t total_read = 0;

    /// Stream we are reading from
    BaseMIStream* stream = nullptr;

    /// Current chunk header we are reading
    ChunkHeader head;

    /// Boolean determining if we require a new chunk
    bool needs_chunk = true;
};

/**
 * @brief Component for writing wave data.
 * 
 * This component simplifies the process for writing wave data.
 * We will take the configuration of this writer and configure the wave data accordingly.
 * This means you must configure the wave data configuration
 * (such as number of channels, sample rate, bits per sample)
 * before this write is started. If these values are changed after start,
 * then any audio data written at a later date may be invalid!
 * 
 * Like to WaveReader, we utilize mstreams to output audio data.
 * 
 * This writer will output wave data in a standard way,
 * meaning that all all audio data will be in one large data chunk.
 * We currently do not support writing of any meta chunks,
 * we do the bare minimum to output audio data.
 * 
 */
class WaveWriter : public BaseWave {
public:

    WaveWriter() = default;

    WaveWriter(BaseMOStream* stream) : stream(stream) {}

    /**
     * @brief Starts this wave reader for writing
     * 
     * You should call this method before doing any writing!
     * We will start the mstream,
     * and write all the necessary pre-data formatting.
     * 
     */
    void start();

    /**
     * @brief Writes data to the wave file
     * 
     * We encode the given data and write it to our mstream.
     * We automatically handle the encoding process,
     * just provide the data and this component will do the rest!
     * 
     * @param data Data to write
     */
    void write_data(BufferPointer data);

private:
    void write_format_chunk();

    /// Stream we are reading from
    BaseMOStream* stream = nullptr;

    /// Total number of byte written
    uint32_t total_written = 0;
};

/**
 * @brief A source for wave data
 * 
 * This module reads audio data from a wave source,
 * and allows this audio data to be integrated into a chain.
 * 
 * The required wave reading operations are done at start time,
 * so you may see some latency when the chain is started.
 * 
 * TODO: Need to find a process for stopping the chain
 * if we reach the end of audio data.
 * 
 */
class WaveSource : public SourceModule, public WaveReader {

    WaveSource() = default;

    WaveSource(BaseMIStream* stream) : WaveReader(stream) {}

    /**
     * @brief Starts this wave source
     * 
     * We simply tell the WaveReader to start read operations.
     * This operation can take some time!
     * 
     */
    void start() override;

    /**
     * @brief Stops this wave source
     * 
     * We simply tell the WaveReader to stop read operations,
     * and will close the underlying mstream.
     * 
     */
    void stop() override;

    /**
     * @brief Processes this module
     * 
     * We simply ask the WaveReader to extract audio data from the wave source.
     * 
     */
    void process() override;
};
