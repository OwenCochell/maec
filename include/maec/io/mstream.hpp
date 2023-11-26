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
 * TODO: NEEDS to add flush() capabilities,
 * for flushing any internal values and writing/reading them.
 * We must make it clear that closing will ALWAYS flush, unless an error has occurred.
 * 
 * We should also include mechanisms for keeping track of how much has been read/written.
 * Not sure if these components will do so, maybe create wrapper class for counting?
 * 
 * Some components that can help determine when the stream is now invalid?
 * Such as, error, EOF, closed socket?
 */
template<bool Input = false, bool Output = false>
class BaseMStream {
public:
    /// State definition
    enum mstate { init, started, stopped, err };

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
     * Te state of the mstream can be utilize
     * to understand the current status of the mstream.
     * 
     * @return Current state of mstream
     */
    mstate get_state() const { return this->state; }

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
    BaseFStream(std::string path) : filepath(path) {}

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
     * @brief Reads contents from a file
     * 
     * @param byts Char array to store results into
     * @param num Number of bytes to read
     */
    void read(char* byts, int num) final { get_stream()->read(byts, num); }

    /**
     * @brief Starts this mstream
     * 
     */
    void start() final {

        // Call parent start method:

        BaseMIStream::start();

        // Open the fstream:

        open();
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

    void get(char& blah) { this->get_stream()->get(blah); }
};

class FOStream : public BaseMOStream, public BaseFStream<std::ofstream, std::ofstream::out | std::ofstream::binary> {
public:

    /**
     * @brief Writes content to a file
     * 
     * @param byts Bytes to write to a file
     * @param num Number of bytes to be written
     */
    void write(char* byts, int num) final { this->get_stream()->write(byts, num); }

    /**
     * @brief Starts this mstream
     * 
     */
    void start() final {

        // Call parent start method:

        BaseMOStream::start();

        // Open the fstream:

        open();
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
