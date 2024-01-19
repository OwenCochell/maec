/**
 * @file mstream.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief MAEC stream components
 * @version 0.1
 * @date 2023-10-31
 * 
 * @copyright Copyright (c) 2023
 * 
 * This file defines the interface/components for MAEC streams.
 * MAEC streams are custom objects that support reading and writing
 * from various sources.
 * These mstreams are designed to abstract away the details of streaming data.
 * 
 * Please note, mstreams are not designed to intepret/format/decode the data they are retrieving!
 * They are designed to simply retrieve the data from a source.
 * You should use higher level components for using this data in a meaningful way.
 */

#pragma once

#include <string>
#include <fstream>
#include <algorithm>
#include <vector>

/**
 * @brief Base class for mstreams
 * 
 * This class defines the basic components that should
 * be present in ALL mstreams.
 * We contain methods for state management, error checking,
 * and configuration.
 * 
 * mstreams have the following states:
 * 
 * - init - Inital state, no operations have been done
 * - started - mstream has been started, and can be utilized
 * - stopped - mstream is in stop state, no longer can be utilized
 * - err - mstream is in a bad state, no longer can be utilized
 * 
 * mstreams are also seekable!
 * You can ask the mstream to seek to a particular position,
 * and it will do so for you. From that point,
 * any operations will be done at the position the stream is at.
 * 
 * TODO: NEEDS to add flush() capabilities,
 * for flushing any internal values and writing/reading them.
 * We must make it clear that closing will ALWAYS flush, unless an error has occurred.
 * 
 * We should also include mechanisms for keeping track of how much has been read/written.
 * Not sure if these components will do so, maybe create wrapper class for counting?
 * 
 */
template<bool Input = false, bool Output = false>
class BaseMStream {
public:
    /// State definition
    enum mstate { init, started, stopped, err };

    /**
     * @brief Asks the mstream to seek to a position
     * 
     * This function will change where the mstream outputs data.
     * You should specify this position in characters,
     * so to seek to character 5, you should pass 5 to this method.
     * 
     * Seeking depends on the mstream that is being utilized,
     * and some don't support seeking!
     * However, the mstream will respect this request in the best way it can.
     * 
     */
    virtual void seek(int pos) =0;

    /**
     * @brief Start method for mstreams
     *
     * This method is called when the user is
     * ready to utilize the mstream.
     * This method only sets the run state.
     * Child mstreams should put any start code into this method.
     *
     * Before a mstream is used, it SHOULD be started.
     */
    virtual void start() {

        // Set state:

        this->state = started;
    }

    /**
     * @brief Stop method for mstreams
     * 
     * This method is called when the user is
     * ready to stop the mstream.
     * We will only configure the state.
     * 
     * After a stream has been stopped, it SHOULD NOT
     * be used again.
     * If an mstream is in the stop state,
     * then the state should be considered useless.
     */
    virtual void stop() {

        // Set stop state:

        this->state = stopped;
    }

    /**
     * @brief Gets the current state of the mstream
     * 
     * The state of the mstream can be utilized
     * to understand the current status of the mstream.
     * 
     * @return Current state of mstream
     */
    mstate get_state() const { return this->state; }

    /**
     * @brief Sets the state of the mstream
     * 
     * @param nstate New state of mstream
     */
    void set_state(mstate nstate) { this->state = nstate; }

    /**
     * @brief Determines if this mstream is in a good state
     * 
     * An mstream is 'good' if it is capable of reading/writing.
     * If it is in a good state, then you can use them as expected.
     * 
     * @return true Stream is in good state
     * @return false Stream is in bad state
     */
    bool good() { return this->state == init || this->state == started; }

    /**
     * @brief Determines if this mstream is in a bad state
     * 
     * An mstream is 'bad' if it is NOT capable of reading/writing.
     * If it is in a bad state, then the stream should not be used going forward.
     * 
     * @return true Stream is in bad state
     * @return false Stream is in good state
     */
    bool bad() { return !this->good(); }

    /**
     * @brief Determines if we are an output mstream
     * 
     * @return true We are output stream
     * @return false We are not an output stream
     */
    static bool is_output() { return Output; }

    /**
     * @brief Determines if we are an input stream
     * 
     * @return true We are an input stream
     * @return false We are not an input stream
     */
    static bool is_input() { return Input; }

private:
    /// Current state of mstream
    mstate state = init;
};

/**
 * @brief Base class for input mstreams
 * 
 * This class provides methods and functionality
 * for input mstreams.
 * We provide methods for reading data.
 * 
 * ALL mistreams MUST implement the methods and functionality defined here.
 */
class BaseMIStream : public BaseMStream<true, false> {
public:

    /**
     * @brief Reads a number of bytes from the stream
     * 
     * This function handles the process of reading bytes from the stream.
     * How this function operates is specific to the stream class,
     * but the general idea is you provide a character array and a number 
     * of bytes to read, and the component will read and place those
     * bytes into the character array.
     * 
     * @param byts Charater array to place data into
     * @param num Number of bytes to read
     */
    virtual void read(char* byts, int num) =0;
};

/**
 * @brief Base class for output mstreams
 * 
 * This class provides methods and functionality for output streams.
 * We provide methods for writing data.
 * 
 * ALL mostreams MUST implement the methods and functionally defined here.
 */
class BaseMOStream : public BaseMStream<false, true> {
public:

    /**
     * @brief Writes a number of bytes to the stream
     * 
     * This function handles the process of writing events to a stream.
     * How this function operations is specific to the stream class,
     * but the general idea is you provide a character array and a number
     * of bytes to read, and the component will read and place those
     * bytes into the character array.
     * 
     * @param byts Character array to write to stream
     * @param num Number of bytes to write
     */
    virtual void write(char* byts, int num) =0;
};

/**
 * @brief mstream for reading byte arrays
 * 
 * This mstream can read a provided byte array.
 * 
 * We iterate over this array as we are asked to extract info from it.
 * This array can be provided to us in many ways,
 * but at the end of the day, we store the characters in a vector.
 * 
 */
class CharIStream : public BaseMIStream {
private:

    /// Vector to store character array in
    std::vector<unsigned char> arr;

    /// Current index of character array
    std::size_t index = 0;

public:

    CharIStream() = default;

    CharIStream(int size) : arr(size) {}

    CharIStream(std::initializer_list<unsigned char> lst) : arr(lst) {}

    /**
     * @brief Seeks the index to the given position.
     * 
     * @param pos Position to seek to
     */
    void seek(int pos) final { this->index = pos; }

    /**
     * @brief Reads chars from the array
     * 
     * We copy chars from the array into the output.
     * 
     * @param byts Char array to store results into
     * @param num Number of bytes to read
     */
    void read(char* byts, int num) final {

        // Copy the contents over:

        std::copy_n(arr.begin() + index, num, byts);

        // Increment index:

        this->index += num;
    }

    /**
     * @brief Gets the array utilized by this mstream
     * 
     * We return a reference to the array utilized by this mstream.
     * Users can configure this array as they see fit.
     * 
     * @return std::vector<char>& Current array in use
     */
    std::vector<unsigned char>& get_array() { return this->arr; }
};

/**
 * @brief mstream for writing byte arrays
 * 
 * This mstream can write to a byte array.
 * 
 * We iterate add values to an array as we are asked to write to it.
 * The size of this array can be provided,
 * and we will reserve the initial size.
 * If we are asked to add values beyond the capacity of the array,
 * we will change the size to fit our values.
 * 
 */
class CharOStream : public BaseMOStream {
private:

    /// Vector to store character data in
    std::vector<unsigned char> arr;

    /// Current index of character array
    std::size_t index = 0;

public:

    CharOStream() = default;

    CharOStream(int size) : arr(size) {}

    CharOStream(std::initializer_list<unsigned char> lst) : arr(lst) {}

    /**
     * @brief Seeks the index to the given position
     * 
     * @param pos Index to seek to
     */
    void seek(int pos) final { this->index = pos; }

    /**
     * @brief Copys characters to the array
     * 
     * @param byts Characters to copy
     * @param num Number of characters to copy
     */
    void write(char* byts, int num) final;

    /**
     * @brief Gets the array utilized by this mstream
     *
     * We return a reference to the array utilized by this mstream.
     * Users can configure this array as they see fit.
     *
     * @return std::vector<char>& Current array in use
     */
    std::vector<unsigned char>& get_array() { return this->arr; }
};

/**
 * @brief Base class all file mstreams must implement
 * 
 * We automatically handle the process of
 * configuring the fstream.
 * 
 * @tparam F fstream type to utilize
 * @tparam mode Mode to provide to fstream
 */
template <typename F, std::ios_base::openmode mode>
class BaseFStream {
private:

    /// fstream to utilize
    F fstream;

    /// path to file we are working with
    std::string filepath;

protected:

    /**
     * @brief Gets a pointer to the underlying stream
     * 
     * @return F* Pointer to stream in use
     */
    F* get_stream() { return &fstream; }

public:

    /**
     * @brief Default constructor
     * 
     */
    BaseFStream() =default;

    /**
     * @brief Allows a path to be specified
     * 
     * @param file Path to file to be operated on
     */
    BaseFStream(std::string path) : filepath(std::move(path)) {}

    /**
     * @brief Gets the path to the file we are working with
     * 
     * @return std::string Path to file
     */
    std::string get_path() const { return this->filepath; }

    /**
     * @brief Sets the path to the file we are working with
     * 
     * @param path New path to file
     */
    void set_path(const std::string& path) { this->filepath = path; }

    /**
     * @brief Opens the fstream
     * 
     * This method opens the fstream and prepares it for operation.
     * We pass the open mode to the fstream.
     * 
     */
    void open() { this->fstream.open(filepath, mode); }

    /**
     * @brief Closes the fstream
     * 
     * This method closes the fstream and prepares it for closure.
     * After the fstream is closed, it must NOT be used again. 
     * 
     */
    void close() { fstream.close(); }

    /**
     * @brief Determines if we have reached the end of the file
     * 
     * If we have reached the end of the file,
     * then there is nothing else to be read.
     * It is recommended to stop this mstream and stop using it.
     * 
     * @return true We have reached the end of the file
     * @return false We not reached the end of the file
     */
    bool eof() const { return this->fstream.eof(); }

    /**
     * @brief Determines if this stream is in a bad state
     * 
     * If we are in a bad state, then this stream is beyond repair
     * and should not be used.
     * This can happen for many different reasons!
     * 
     * @return true If stream is valid
     * @return false If stream is invalid
     */
    bool good() const { return this->fstream.good(); }
};

/**
 * @brief mstream for reading file contents
 * 
 * This mstream reads info from a file,
 * allowing you to utilize file data in an mstream enabled scenario.
 * 
 */
class FIStream : public BaseMIStream, public BaseFStream<std::ifstream, std::ifstream::in | std::ifstream::binary> {
public:

    /**
     * @brief Seeks to the given position
     *
     * @param pos Position to seek to
     */
    void seek(int pos) final { this->get_stream()->seekg(pos); }

    /**
     * @brief Reads contents from a file
     * 
     * @param byts Char array to store results into
     * @param num Number of bytes to read
     */
    void read(char* byts, int num) final {

        // Read the data:
        get_stream()->read(byts, num);

        // Determine if we are in a bad state:

        if (!BaseFStream::good()) {

            // Invalid, just close:

            this->stop();
        }
    }

    /**
     * @brief Starts this mstream
     * 
     */
    void start() final {

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

    /**
     * @brief Stops this mstream
     * 
     */
    void stop() final {

        // Call parent stop method:

        BaseMIStream::stop();

        // Close the fstream:

        close();
    }
};

/**
 * @brief mstream for writing file contents
 * 
 * This mstream writes info to a file,
 * allowing you to write file data in mstream enabled scenarios.
 */
class FOStream : public BaseMOStream, public BaseFStream<std::ofstream, std::ofstream::out | std::ofstream::binary> {
public:

    /**
     * @brief Seeks to the given position
     *
     * @param pos Position to seek to
     */
    void seek(int pos) final { this->get_stream()->seekp(pos); }

    /**
     * @brief Writes content to a file
     * 
     * @param byts Bytes to write to a file
     * @param num Number of bytes to be written
     */
    void write(char* byts, int num) final {

        // Write the bytes:
        this->get_stream()->write(byts, num);

        // Determine if we are in a bad state:

        if (!BaseFStream::good()) {

            // Invalid, just close:

            this->stop();
        }
    }

    /**
     * @brief Starts this mstream
     * 
     */
    void start() final {

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

    /**
     * @brief Stops this mstream
     * 
     */
    void stop() final {

        // Call parent stop method:

        BaseMOStream::stop();

        // Close the fstream:

        close();
    }
};
